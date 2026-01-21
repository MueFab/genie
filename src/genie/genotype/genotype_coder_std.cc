#include "genotype_coder_std.h"
#include <genie/util/runtime_exception.h>
#include <codecs/include/mpegg-codecs.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <random>
#include <numeric>

namespace genie::genotype::detail::std_lib {

void decompose(
    std::vector<core::record::VariantGenotype>& recs,
    uint8_t& max_ploidy,
    Int8MatDtype& allele_mat,
    BinMatDtype& phasing_mat,
    size_t block_size
) {
    UTILS_DIE_IF(recs.empty(), "No records found for the process!");
    block_size = std::min(block_size, recs.size());
    uint32_t num_samples = recs.front().GetSampleCount();

    max_ploidy = 0;
    for (size_t i = 0; i < block_size; ++i) {
        UTILS_DIE_IF(num_samples != recs[i].GetSampleCount(), "Inconsistent sample count in block");
        max_ploidy = std::max(max_ploidy, static_cast<uint8_t>(recs[i].GetNumberOfAllelesPerSample()));
    }

    allele_mat.assign(block_size, std::vector<int8_t>(num_samples * max_ploidy, -2));
    phasing_mat.assign(block_size, std::vector<bool>(num_samples * (max_ploidy > 0 ? max_ploidy - 1 : 0), false));

    for (size_t i = 0; i < block_size; ++i) {
        auto& rec = recs[i];
        auto& alleles = rec.GetAlleles();
        auto& phasing = rec.GetPhasing();
        uint8_t rec_ploidy = static_cast<uint8_t>(rec.GetNumberOfAllelesPerSample());

        for (uint32_t j = 0; j < num_samples; ++j) {
            for (uint8_t k = 0; k < rec_ploidy; ++k) {
                allele_mat[i][j * max_ploidy + k] = static_cast<int8_t>(alleles[j][k]);
            }
            if (max_ploidy > 1) {
                for (uint8_t k = 0; k < rec_ploidy - 1; ++k) {
                    phasing_mat[i][j * (max_ploidy - 1) + k] = phasing[j][k];
                }
            }
        }
    }
}

void transform_max_value(
    Int8MatDtype& allele_mat,
    bool& no_ref_flag,
    bool& not_avail_flag
) {
    no_ref_flag = false;
    not_avail_flag = false;
    int8_t current_max = -128;

    for (const auto& row : allele_mat) {
        for (int8_t val : row) {
            if (val == -1) no_ref_flag = true;
            else if (val == -2) not_avail_flag = true;
            else if (val > current_max) current_max = val;
        }
    }

    if (no_ref_flag) {
        current_max++;
        int8_t target = current_max;
        for (auto& row : allele_mat) {
            for (auto& val : row) if (val == -1) val = target;
        }
    }

    if (not_avail_flag) {
        current_max++;
        int8_t target = current_max;
        for (auto& row : allele_mat) {
            for (auto& val : row) if (val == -2) val = target;
        }
    }
}

void inverse_transform_max_val(Int8MatDtype& allele_mat, bool no_ref_flag, bool not_avail_flag) {
    if (allele_mat.empty()) return;
    int8_t current_max = -128;
    for (const auto& row : allele_mat) {
        for (int8_t val : row) if (val > current_max) current_max = val;
    }

    if (not_avail_flag) {
        for (auto& row : allele_mat) {
            for (auto& val : row) if (val == current_max) val = -2;
        }
        current_max--;
    }

    if (no_ref_flag) {
        for (auto& row : allele_mat) {
            for (auto& val : row) if (val == current_max) val = -1;
        }
    }
}

void binarize_bit_plane(
    Int8MatDtype& allele_mat,
    std::vector<BinMatDtype>& bin_mats,
    uint8_t& num_bit_planes,
    const ConcatAxis concat_axis
) {
    if (allele_mat.empty()) {
        num_bit_planes = 1;
        bin_mats.clear();
        return;
    }
    int8_t max_val = 0;
    for (const auto& row : allele_mat) {
        for (int8_t val : row) if (val > max_val) max_val = val;
    }
    num_bit_planes = static_cast<uint8_t>(std::ceil(std::log2(max_val + 1)));
    if (num_bit_planes == 0) num_bit_planes = 1;

    size_t nrows = allele_mat.size();
    size_t ncols = allele_mat[0].size();

    bin_mats.resize(num_bit_planes, std::vector<std::vector<bool>>(nrows, std::vector<bool>(ncols)));

    for (uint8_t k = 0; k < num_bit_planes; ++k) {
        for (size_t i = 0; i < nrows; ++i) {
            for (size_t j = 0; j < ncols; ++j) {
                bin_mats[k][i][j] = (allele_mat[i][j] & (1 << k)) != 0;
            }
        }
    }

    if (concat_axis != ConcatAxis::DO_NOT_CONCAT) {
        BinMatDtype combined;
        if (concat_axis == ConcatAxis::CONCAT_ROW_DIR) {
            combined.reserve(nrows * num_bit_planes);
            for (uint8_t k = 0; k < num_bit_planes; ++k) {
                combined.insert(combined.end(), bin_mats[k].begin(), bin_mats[k].end());
            }
        } else if (concat_axis == ConcatAxis::CONCAT_COL_DIR) {
            combined.resize(nrows, std::vector<bool>(ncols * num_bit_planes));
            for (size_t i = 0; i < nrows; ++i) {
                for (uint8_t k = 0; k < num_bit_planes; ++k) {
                    std::copy(bin_mats[k][i].begin(), bin_mats[k][i].end(), combined[i].begin() + k * ncols);
                }
            }
        }
        bin_mats.clear();
        bin_mats.push_back(std::move(combined));
    }
    allele_mat.clear();
}

void debinarize_bit_plane(
    std::vector<BinMatDtype>& bin_mats,
    uint8_t num_bit_planes,
    const ConcatAxis concat_axis,
    Int8MatDtype& allele_mat
) {
    if (bin_mats.empty()) return;

    if (concat_axis == ConcatAxis::CONCAT_ROW_DIR) {
        auto& combined = bin_mats.front();
        size_t total_rows = combined.size();
        size_t nrows = total_rows / num_bit_planes;
        size_t ncols = combined[0].size();
        std::vector<BinMatDtype> split_mats;
        for (uint8_t k = 0; k < num_bit_planes; ++k) {
            BinMatDtype bm(combined.begin() + k * nrows, combined.begin() + (k + 1) * nrows);
            split_mats.push_back(std::move(bm));
        }
        bin_mats = std::move(split_mats);
    } else if (concat_axis == ConcatAxis::CONCAT_COL_DIR) {
        auto& combined = bin_mats.front();
        size_t nrows = combined.size();
        size_t total_cols = combined[0].size();
        size_t ncols = total_cols / num_bit_planes;
        std::vector<BinMatDtype> split_mats(num_bit_planes, BinMatDtype(nrows, std::vector<bool>(ncols)));
        for (size_t i = 0; i < nrows; ++i) {
            for (uint8_t k = 0; k < num_bit_planes; ++k) {
                std::copy(combined[i].begin() + k * ncols, combined[i].begin() + (k + 1) * ncols, split_mats[k][i].begin());
            }
        }
        bin_mats = std::move(split_mats);
    }

    size_t nrows = bin_mats[0].size();
    size_t ncols = bin_mats[0][0].size();
    allele_mat.assign(nrows, std::vector<int8_t>(ncols, 0));

    for (size_t k = 0; k < bin_mats.size(); ++k) {
        for (size_t i = 0; i < nrows; ++i) {
            for (size_t j = 0; j < ncols; ++j) {
                if (bin_mats[k][i][j]) {
                    allele_mat[i][j] |= (1 << k);
                }
            }
        }
    }
}

void binarize_row_bin(
    Int8MatDtype& allele_mat,
    std::vector<BinMatDtype>& bin_mats,
    UIntVecDtype& amax_vec
) {
    if (allele_mat.empty()) return;
    size_t nrows = allele_mat.size();
    size_t ncols = allele_mat[0].size();

    amax_vec.resize(nrows);
    size_t total_bin_rows = 0;
    for (size_t i = 0; i < nrows; ++i) {
        int8_t row_max = 0;
        for (int8_t val : allele_mat[i]) if (val > row_max) row_max = val;
        amax_vec[i] = static_cast<uint32_t>(std::ceil(std::log2(row_max + 1)));
        if (amax_vec[i] == 0) amax_vec[i] = 1;
        total_bin_rows += amax_vec[i];
    }

    BinMatDtype bin_mat(total_bin_rows, std::vector<bool>(ncols, false));
    size_t current_row = 0;
    for (size_t i = 0; i < nrows; ++i) {
        for (uint32_t k = 0; k < amax_vec[i]; ++k) {
            for (size_t j = 0; j < ncols; ++j) {
                bin_mat[current_row][j] = (allele_mat[i][j] & (1 << k)) != 0;
            }
            current_row++;
        }
    }
    bin_mats.push_back(std::move(bin_mat));
    allele_mat.clear();
}

void debinarize_row_bin(
    std::vector<BinMatDtype>& bin_mats,
    UIntVecDtype& amax_vec,
    Int8MatDtype& allele_mat
) {
    if (bin_mats.empty()) return;
    auto& bin_mat = bin_mats.front();
    size_t nrows = amax_vec.size();
    size_t ncols = bin_mat[0].size();

    allele_mat.assign(nrows, std::vector<int8_t>(ncols, 0));
    size_t current_row = 0;
    for (size_t i = 0; i < nrows; ++i) {
        for (uint32_t k = 0; k < amax_vec[i]; ++k) {
            for (size_t j = 0; j < ncols; ++j) {
                if (bin_mat[current_row][j]) {
                    allele_mat[i][j] |= (1 << k);
                }
            }
            current_row++;
        }
    }
}

void binarize_allele_mat(
    Int8MatDtype& allele_mat,
    std::vector<BinMatDtype>& bin_mats,
    uint8_t& num_bit_planes,
    UIntVecDtype& amax_vec,
    BinarizationID binarization_ID,
    ConcatAxis concat_axis
) {
    if (binarization_ID == BinarizationID::BIT_PLANE) {
        binarize_bit_plane(allele_mat, bin_mats, num_bit_planes, concat_axis);
    } else if (binarization_ID == BinarizationID::ROW_BIN) {
        num_bit_planes = 1;
        binarize_row_bin(allele_mat, bin_mats, amax_vec);
    } else {
        UTILS_DIE("Invalid binarization_ID");
    }
}

void sort_matrix(
    BinMatDtype& bin_mat,
    const UIntVecDtype& ids,
    uint8_t axis
) {
    if (ids.empty() || bin_mat.empty()) return;
    if (axis == 0) {
        BinMatDtype temp(ids.size(), std::vector<bool>(bin_mat[0].size()));
        for (size_t i = 0; i < ids.size(); ++i) {
            std::copy(bin_mat[ids[i]].begin(), bin_mat[ids[i]].end(), temp[i].begin());
        }
        bin_mat = std::move(temp);
    } else if (axis == 1) {
        size_t nrows = bin_mat.size();
        size_t ncols = ids.size();
        BinMatDtype temp(nrows, std::vector<bool>(ncols));
        for (size_t i = 0; i < nrows; ++i) {
            for (size_t j = 0; j < ncols; ++j) {
                temp[i][j] = bin_mat[i][ids[j]];
            }
        }
        bin_mat = std::move(temp);
    } else {
        UTILS_DIE("Invalid axis");
    }
}

void random_sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& ids,
    uint8_t axis
) {
    size_t n = (axis == 0) ? bin_mat.size() : (bin_mat.empty() ? 0 : bin_mat[0].size());
    ids.resize(n);
    std::iota(ids.begin(), ids.end(), 0);
    
    static std::random_device rd;
    static std::mt19937 g(rd());
    std::shuffle(ids.begin(), ids.end(), g);
    
    sort_matrix(bin_mat, ids, axis);
    
    std::vector<uint32_t> inverse_ids(n);
    for (uint32_t i = 0; i < n; ++i) {
        inverse_ids[ids[i]] = i;
    }
    ids = std::move(inverse_ids);
}

void sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& row_ids,
    UIntVecDtype& col_ids,
    SortingAlgoID sort_row_method,
    SortingAlgoID sort_col_method
) {
    if (sort_row_method == SortingAlgoID::NO_SORTING) {
        row_ids = {0};
    } else if (sort_row_method == SortingAlgoID::RANDOM_SORT) {
        random_sort_bin_mat(bin_mat, row_ids, 0);
    } else {
        UTILS_DIE("Sort method not implemented for STD");
    }

    if (sort_col_method == SortingAlgoID::NO_SORTING) {
        col_ids = {0};
    } else if (sort_col_method == SortingAlgoID::RANDOM_SORT) {
        random_sort_bin_mat(bin_mat, col_ids, 1);
    } else {
        UTILS_DIE("Sort method not implemented for STD");
    }
}

void invert_sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& row_ids,
    UIntVecDtype& col_ids
) {
    if (row_ids.size() != 1) {
        sort_matrix(bin_mat, row_ids, 0);
        row_ids = {0};
    }
    if (col_ids.size() != 1) {
        sort_matrix(bin_mat, col_ids, 1);
        col_ids = {0};
    }
}

void bin_mat_to_bytes(
    const BinMatDtype& bin_mat,
    uint8_t** payload,
    size_t& payload_len
) {
    if (bin_mat.empty()) {
        *payload = nullptr;
        payload_len = 0;
        return;
    }
    size_t nrows = bin_mat.size();
    size_t ncols = bin_mat[0].size();

    size_t bpl = (ncols >> 3u) + ((ncols & 7u) > 0u);
    payload_len = bpl * nrows;
    *payload = static_cast<uint8_t*>(calloc(payload_len, sizeof(uint8_t)));

    for (size_t i = 0; i < nrows; ++i) {
        size_t row_offset = i * bpl;
        for (size_t j = 0; j < ncols; ++j) {
            if (bin_mat[i][j]) {
                size_t byte_offset = row_offset + (j >> 3u);
                uint8_t shift = static_cast<uint8_t>(7u - (j & 7u));
                (*payload)[byte_offset] |= (1u << shift);
            }
        }
    }
}

void bin_mat_from_bytes(
    const uint8_t* payload,
    size_t payload_len,
    size_t nrows,
    size_t ncols,
    BinMatDtype& bin_mat
) {
    size_t bpl = (ncols >> 3u) + ((ncols & 7u) > 0u);
    UTILS_DIE_IF(payload_len != nrows * bpl, "Invalid payload length for dimensions");

    bin_mat.assign(nrows, std::vector<bool>(ncols, false));

    for (size_t i = 0; i < nrows; ++i) {
        size_t row_offset = i * bpl;
        for (size_t j = 0; j < ncols; ++j) {
            size_t byte_offset = row_offset + (j >> 3u);
            uint8_t shift = static_cast<uint8_t>(7u - (j & 7u));
            bin_mat[i][j] = (payload[byte_offset] >> shift) & 1u;
        }
    }
}

void entropy_encode_bin_mat(
    const BinMatDtype& bin_mat,
    genie::core::AlgoID codec_ID,
    std::vector<uint8_t>& payload
) {
    uint8_t* raw_data = nullptr;
    size_t raw_data_len = 0;
    uint8_t* compressed_data = nullptr;
    size_t compressed_data_len = 0;

    bin_mat_to_bytes(bin_mat, &raw_data, raw_data_len);

    switch (codec_ID) {
        case genie::core::AlgoID::JBIG: {
            size_t nrows = bin_mat.size();
            size_t ncols = bin_mat.empty() ? 0 : bin_mat[0].size();
            mpegg_jbig_compress_default(
                &compressed_data, &compressed_data_len,
                raw_data, raw_data_len,
                static_cast<unsigned long>(nrows),
                static_cast<unsigned long>(ncols)
            );
        } break;
        case genie::core::AlgoID::ZSTD: {
            mpegg_zstd_compress(&compressed_data, &compressed_data_len, raw_data, raw_data_len, 3);
        } break;
        case genie::core::AlgoID::BSC: {
            mpegg_bsc_compress(&compressed_data, &compressed_data_len, raw_data, raw_data_len, 20, 8, 1, 1);
        } break;
        case genie::core::AlgoID::LZMA: {
            UTILS_DIE("LZMA not yet implemented");
        } break;
        default:
            UTILS_DIE("Invalid codec_ID");
    }

    if (compressed_data) {
        payload.assign(compressed_data, compressed_data + compressed_data_len);
        free(compressed_data);
    }
    if (raw_data) free(raw_data);
}

void entropy_decode_bin_mat(
    const std::vector<uint8_t>& payload,
    genie::core::AlgoID codec_ID,
    size_t nrows,
    size_t ncols,
    BinMatDtype& bin_mat
) {
    uint8_t* raw_data = nullptr;
    size_t raw_data_len = 0;

    switch (codec_ID) {
        case genie::core::AlgoID::JBIG: {
            mpegg_jbig_decompress_default(
                &raw_data, &raw_data_len,
                payload.data(), payload.size(),
                reinterpret_cast<unsigned long*>(&nrows),
                reinterpret_cast<unsigned long*>(&ncols)
            );
        } break;
        case genie::core::AlgoID::ZSTD: {
            mpegg_zstd_decompress(&raw_data, &raw_data_len, payload.data(), payload.size());
        } break;
        case genie::core::AlgoID::BSC: {
            mpegg_bsc_decompress(&raw_data, &raw_data_len, payload.data(), payload.size());
        } break;
        case genie::core::AlgoID::LZMA: {
            UTILS_DIE("LZMA not yet implemented");
        } break;
        default:
            UTILS_DIE("Invalid codec_ID");
    }

    bin_mat_from_bytes(raw_data, raw_data_len, nrows, ncols, bin_mat);
    if (raw_data) free(raw_data);
}

void encode_and_sort_bin_mat(
    BinMatDtype& bin_mat,
    SortedBinMatPayload& sorted_bin_mat_payload,
    SortingAlgoID sort_row_method,
    SortingAlgoID sort_col_method,
    genie::core::AlgoID codec_ID
) {
    auto sort_rows_flag = sort_row_method != SortingAlgoID::NO_SORTING;
    auto sort_cols_flag = sort_col_method != SortingAlgoID::NO_SORTING;

    UIntVecDtype row_ids, col_ids;
    sort_bin_mat(bin_mat, row_ids, col_ids, sort_row_method, sort_col_method);

    if (sort_rows_flag) {
        sorted_bin_mat_payload.SetRowIdsPayload(RowColIdsPayload(std::move(row_ids)));
    }
    if (sort_cols_flag) {
        sorted_bin_mat_payload.SetColIdsPayload(RowColIdsPayload(std::move(col_ids)));
    }

    std::vector<uint8_t> payload;
    entropy_encode_bin_mat(bin_mat, codec_ID, payload);
    sorted_bin_mat_payload.SetBinMatPayload(BinMatPayload(
        codec_ID, std::move(payload),
        static_cast<uint32_t>(bin_mat.size()),
        static_cast<uint32_t>(bin_mat.empty() ? 0 : bin_mat[0].size())));
}

void decode_and_inverse_sort_bin_mat(
    const SortedBinMatPayload& sorted_bin_mat_payload,
    BinMatDtype& bin_mat,
    genie::core::AlgoID codec_ID,
    bool sort_rows_flag,
    bool sort_cols_flag
) {
    auto& bin_mat_payload = sorted_bin_mat_payload.GetBinMatPayload();
    entropy_decode_bin_mat(bin_mat_payload.GetPayload(), codec_ID, bin_mat_payload.GetNRows(), bin_mat_payload.GetNCols(), bin_mat);

    if (sort_rows_flag) {
        auto row_ids = sorted_bin_mat_payload.GetRowIdsPayload()->GetRowColIdsElements();
        sort_matrix(bin_mat, row_ids, 0);
    }
    if (sort_cols_flag) {
        auto col_ids = sorted_bin_mat_payload.GetColIdsPayload()->GetRowColIdsElements();
        sort_matrix(bin_mat, col_ids, 1);
    }
}

void encode_genotype(
    std::vector<core::record::VariantGenotype>& recs,
    GenotypeParameters& params,
    GenotypePayload& payload,
    size_t block_size,
    BinarizationID binarization_ID,
    ConcatAxis concat_axis,
    bool transpose_mat,
    SortingAlgoID sort_row_method,
    SortingAlgoID sort_col_method,
    genie::core::AlgoID codec_ID
) {
    params = GenotypeParameters(
        binarization_ID, concat_axis,
        sort_row_method != SortingAlgoID::NO_SORTING,
        sort_col_method != SortingAlgoID::NO_SORTING,
        transpose_mat, codec_ID, true,
        sort_row_method != SortingAlgoID::NO_SORTING,
        sort_col_method != SortingAlgoID::NO_SORTING,
        transpose_mat, codec_ID
    );

    uint8_t max_ploidy;
    Int8MatDtype allele_mat;
    BinMatDtype phasing_mat;
    decompose(recs, max_ploidy, allele_mat, phasing_mat, block_size);

    payload.SetMaxPloidy(max_ploidy);

    bool dot_flag, na_flag;
    transform_max_value(allele_mat, dot_flag, na_flag);
    payload.SetNoReferenceFlag(dot_flag);
    payload.SetNotAvailableFlag(na_flag);

    std::vector<BinMatDtype> allele_bin_mats;
    uint8_t num_bit_planes;
    UIntVecDtype amax_vec;
    binarize_allele_mat(allele_mat, allele_bin_mats, num_bit_planes, amax_vec, binarization_ID, concat_axis);

    payload.SetNumBitPlanes(num_bit_planes);
    if (binarization_ID == BinarizationID::ROW_BIN) {
        std::vector<uint64_t> amax_u64(amax_vec.begin(), amax_vec.end());
        payload.SetVariantsAmaxPayload(AmaxPayload(std::move(amax_u64)));
    }

    for (auto& bm : allele_bin_mats) {
        SortedBinMatPayload sbm;
        encode_and_sort_bin_mat(bm, sbm, sort_row_method, sort_col_method, codec_ID);
        payload.AddVariantsPayload(std::move(sbm));
    }

    if (params.GetEncodePhasesDataFlag()) {
        SortedBinMatPayload ph_sbm;
        encode_and_sort_bin_mat(phasing_mat, ph_sbm, sort_row_method, sort_col_method, codec_ID);
        payload.SetPhasesPayload(std::move(ph_sbm));
    } else {
        payload.SetPhasesValue(true);
    }
}

void decode_genotype(
    const GenotypeParameters& params,
    const GenotypePayload& payload,
    Int8MatDtype& allele_mat,
    BinMatDtype& phasing_mat
) {
    std::vector<BinMatDtype> bin_mats;
    for (const auto& sbm : payload.GetVariantsPayloads()) {
        BinMatDtype bm;
        decode_and_inverse_sort_bin_mat(sbm, bm, params.GetVariantsCodecID(), params.GetSortVariantsRowsFlag(), params.GetSortVariantsColsFlag());
        bin_mats.push_back(std::move(bm));
    }

    if (params.GetBinarizationID() == BinarizationID::ROW_BIN) {
        auto amax_u64 = payload.GetVariantsAmaxPayload()->GetAmaxElements();
        UIntVecDtype amax_vec(amax_u64.begin(), amax_u64.end());
        debinarize_row_bin(bin_mats, amax_vec, allele_mat);
    } else {
        debinarize_bit_plane(bin_mats, payload.GetNumBitPlanes(), params.GetConcatAxis(), allele_mat);
    }

    auto ph_sbm = payload.GetPhasesPayload();
    if (ph_sbm) {
        decode_and_inverse_sort_bin_mat(*ph_sbm, phasing_mat, params.GetPhasesCodecID(), params.GetSortPhasesRowsFlag(), params.GetSortPhasesColsFlag());
    }

    inverse_transform_max_val(allele_mat, payload.GetNoReferenceFlag(), payload.GetNotAvailableFlag());
}

} // namespace genie::genotype::detail::std_lib
