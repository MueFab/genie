#include "genotype_coder_eigen.h"
#include <genie/util/runtime_exception.h>
#include <codecs/include/mpegg-codecs.h>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace genie::genotype::detail::eigen {

void bin_mat_to_bytes(
    const BinMatDtype& bin_mat,
    uint8_t** payload,
    size_t& payload_len
) {
    if (bin_mat.size() == 0) {
        *payload = nullptr;
        payload_len = 0;
        return;
    }
    size_t nrows = static_cast<size_t>(bin_mat.rows());
    size_t ncols = static_cast<size_t>(bin_mat.cols());

    size_t bpl = (ncols >> 3u) + ((ncols & 7u) > 0u);
    payload_len = bpl * nrows;
    *payload = static_cast<uint8_t*>(calloc(payload_len, sizeof(uint8_t)));

    for (size_t i = 0; i < nrows; ++i) {
        size_t row_offset = i * bpl;
        for (size_t j = 0; j < ncols; ++j) {
            if (bin_mat(static_cast<Eigen::Index>(i), static_cast<Eigen::Index>(j))) {
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

    bin_mat.resize(static_cast<Eigen::Index>(nrows), static_cast<Eigen::Index>(ncols));
    bin_mat.setConstant(false);

    for (size_t i = 0; i < nrows; ++i) {
        size_t row_offset = i * bpl;
        for (size_t j = 0; j < ncols; ++j) {
            size_t byte_offset = row_offset + (j >> 3u);
            uint8_t shift = static_cast<uint8_t>(7u - (j & 7u));
            bin_mat(static_cast<Eigen::Index>(i), static_cast<Eigen::Index>(j)) = (payload[byte_offset] >> shift) & 1u;
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
            mpegg_jbig_compress_default(
                &compressed_data, &compressed_data_len,
                raw_data, raw_data_len,
                static_cast<unsigned long>(bin_mat.rows()),
                static_cast<unsigned long>(bin_mat.cols())
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
        max_ploidy = std::max(max_ploidy, static_cast<uint8_t>(recs[i].GetNumberOfAllelesPerSample()));
    }

    allele_mat.resize(static_cast<Eigen::Index>(block_size), static_cast<Eigen::Index>(num_samples * max_ploidy));
    allele_mat.setConstant(-2);

    phasing_mat.resize(static_cast<Eigen::Index>(block_size), static_cast<Eigen::Index>(num_samples * (max_ploidy > 0 ? max_ploidy - 1 : 0)));
    phasing_mat.setConstant(false);

    for (size_t i = 0; i < block_size; ++i) {
        auto& rec = recs[i];
        auto& alleles = rec.GetAlleles();
        auto& phasing = rec.GetPhasing();
        uint8_t rec_ploidy = static_cast<uint8_t>(rec.GetNumberOfAllelesPerSample());

        for (uint32_t j = 0; j < num_samples; ++j) {
            for (uint8_t k = 0; k < rec_ploidy; ++k) {
                allele_mat(static_cast<Eigen::Index>(i), static_cast<Eigen::Index>(j * max_ploidy + k)) = static_cast<int8_t>(alleles[j][k]);
            }
            if (max_ploidy > 1) {
                for (uint8_t k = 0; k < rec_ploidy - 1; ++k) {
                    phasing_mat(static_cast<Eigen::Index>(i), static_cast<Eigen::Index>(j * (max_ploidy - 1) + k)) = phasing[j][k];
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
    no_ref_flag = (allele_mat.array() == -1).any();
    not_avail_flag = (allele_mat.array() == -2).any();

    int8_t current_max = allele_mat.maxCoeff();
    if (current_max < 0) current_max = 0; // Handle case where only -1/-2 are present? Unlikely.

    if (no_ref_flag) {
        current_max++;
        int8_t target = current_max;
        for(int i=0; i<allele_mat.rows(); ++i)
            for(int j=0; j<allele_mat.cols(); ++j)
                if (allele_mat(i,j) == -1) allele_mat(i,j) = target;
    }

    if (not_avail_flag) {
        current_max++;
        int8_t target = current_max;
        for(int i=0; i<allele_mat.rows(); ++i)
            for(int j=0; j<allele_mat.cols(); ++j)
                if (allele_mat(i,j) == -2) allele_mat(i,j) = target;
    }
}

void inverse_transform_max_val(Int8MatDtype& allele_mat, bool no_ref_flag, bool not_avail_flag) {
    if (allele_mat.size() == 0) return;
    int8_t current_max = allele_mat.maxCoeff();

    if (not_avail_flag) {
        for(int i=0; i<allele_mat.rows(); ++i)
            for(int j=0; j<allele_mat.cols(); ++j)
                if (allele_mat(i,j) == current_max) allele_mat(i,j) = -2;
        current_max--;
    }

    if (no_ref_flag) {
        for(int i=0; i<allele_mat.rows(); ++i)
            for(int j=0; j<allele_mat.cols(); ++j)
                if (allele_mat(i,j) == current_max) allele_mat(i,j) = -1;
    }
}

void binarize_bit_plane(
    Int8MatDtype& allele_mat,
    std::vector<BinMatDtype>& bin_mats,
    uint8_t& num_bit_planes,
    const ConcatAxis concat_axis
) {
    if (allele_mat.size() == 0) {
        num_bit_planes = 1;
        bin_mats.clear();
        return;
    }
    int8_t max_val = allele_mat.maxCoeff();
    num_bit_planes = static_cast<uint8_t>(std::ceil(std::log2(max_val + 1)));
    if (num_bit_planes == 0) num_bit_planes = 1;

    Eigen::Index nrows = allele_mat.rows();
    Eigen::Index ncols = allele_mat.cols();

    bin_mats.resize(num_bit_planes);
    for (uint8_t k = 0; k < num_bit_planes; ++k) {
        bin_mats[k].resize(nrows, ncols);
        for(int i=0; i<nrows; ++i)
            for(int j=0; j<ncols; ++j)
                bin_mats[k](i, j) = (allele_mat(i, j) & (1 << k)) != 0;
    }

    if (concat_axis != ConcatAxis::DO_NOT_CONCAT) {
        if (concat_axis == ConcatAxis::CONCAT_ROW_DIR) {
            BinMatDtype combined(nrows * num_bit_planes, ncols);
            for (uint8_t k = 0; k < num_bit_planes; ++k) {
                combined.middleRows(k * nrows, nrows) = bin_mats[k];
            }
            bin_mats = {std::move(combined)};
        } else if (concat_axis == ConcatAxis::CONCAT_COL_DIR) {
            BinMatDtype combined(nrows, ncols * num_bit_planes);
            for (uint8_t k = 0; k < num_bit_planes; ++k) {
                combined.middleCols(k * ncols, ncols) = bin_mats[k];
            }
            bin_mats = {std::move(combined)};
        }
    }
    allele_mat.resize(0, 0);
}

void debinarize_bit_plane(
    std::vector<BinMatDtype>& bin_mats,
    uint8_t num_bit_planes,
    const ConcatAxis concat_axis,
    Int8MatDtype& allele_mat
) {
    if (bin_mats.empty()) return;

    if (concat_axis == ConcatAxis::CONCAT_ROW_DIR) {
        BinMatDtype combined = std::move(bin_mats[0]);
        Eigen::Index nrows = combined.rows() / num_bit_planes;
        Eigen::Index ncols = combined.cols();
        bin_mats.clear();
        for (uint8_t k = 0; k < num_bit_planes; ++k) {
            bin_mats.push_back(combined.middleRows(k * nrows, nrows));
        }
    } else if (concat_axis == ConcatAxis::CONCAT_COL_DIR) {
        BinMatDtype combined = std::move(bin_mats[0]);
        Eigen::Index nrows = combined.rows();
        Eigen::Index ncols = combined.cols() / num_bit_planes;
        bin_mats.clear();
        for (uint8_t k = 0; k < num_bit_planes; ++k) {
            bin_mats.push_back(combined.middleCols(k * ncols, ncols));
        }
    }

    Eigen::Index nrows = bin_mats[0].rows();
    Eigen::Index ncols = bin_mats[0].cols();
    allele_mat.resize(nrows, ncols);
    allele_mat.setZero();

    for (uint8_t k = 0; k < static_cast<uint8_t>(bin_mats.size()); ++k) {
        for(int i=0; i<nrows; ++i)
            for(int j=0; j<ncols; ++j)
                if (bin_mats[k](i, j)) allele_mat(i, j) |= (1 << k);
    }
}

void binarize_row_bin(
    Int8MatDtype& allele_mat,
    std::vector<BinMatDtype>& bin_mats,
    UIntVecDtype& amax_vec
) {
    if (allele_mat.size() == 0) return;
    Eigen::Index nrows = allele_mat.rows();
    Eigen::Index ncols = allele_mat.cols();

    amax_vec.resize(nrows);
    Eigen::Index total_bin_rows = 0;
    for (int i = 0; i < nrows; ++i) {
        int8_t row_max = allele_mat.row(i).maxCoeff();
        amax_vec(i) = static_cast<uint32_t>(std::ceil(std::log2(row_max + 1)));
        if (amax_vec(i) == 0) amax_vec(i) = 1;
        total_bin_rows += amax_vec(i);
    }

    BinMatDtype bin_mat(total_bin_rows, ncols);
    Eigen::Index current_row = 0;
    for (int i = 0; i < nrows; ++i) {
        for (uint32_t k = 0; k < amax_vec(i); ++k) {
            for (int j = 0; j < ncols; ++j) {
                bin_mat(current_row, j) = (allele_mat(i, j) & (1 << k)) != 0;
            }
            current_row++;
        }
    }
    bin_mats.push_back(std::move(bin_mat));
    allele_mat.resize(0, 0);
}

void debinarize_row_bin(
    std::vector<BinMatDtype>& bin_mats,
    UIntVecDtype& amax_vec,
    Int8MatDtype& allele_mat
) {
    if (bin_mats.empty()) return;
    auto& bin_mat = bin_mats.front();
    Eigen::Index nrows = amax_vec.size();
    Eigen::Index ncols = bin_mat.cols();

    allele_mat.resize(nrows, ncols);
    allele_mat.setZero();
    Eigen::Index current_row = 0;
    for (int i = 0; i < nrows; ++i) {
        for (uint32_t k = 0; k < amax_vec(i); ++k) {
            for (int j = 0; j < ncols; ++j) {
                if (bin_mat(current_row, j)) {
                    allele_mat(i, j) |= (1 << k);
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
    if (ids.size() == 0 || bin_mat.size() == 0) return;
    if (axis == 0) {
        BinMatDtype temp(ids.size(), bin_mat.cols());
        for (int i = 0; i < ids.size(); ++i) {
            temp.row(i) = bin_mat.row(static_cast<Eigen::Index>(ids(i)));
        }
        bin_mat = std::move(temp);
    } else if (axis == 1) {
        BinMatDtype temp(bin_mat.rows(), ids.size());
        for (int j = 0; j < ids.size(); ++j) {
            temp.col(j) = bin_mat.col(static_cast<Eigen::Index>(ids(j)));
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
    Eigen::Index n = (axis == 0) ? bin_mat.rows() : bin_mat.cols();
    std::vector<uint32_t> p(static_cast<size_t>(n));
    std::iota(p.begin(), p.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(p.begin(), p.end(), g);
    
    UIntVecDtype p_eigen = Eigen::Map<UIntVecDtype>(p.data(), n);
    sort_matrix(bin_mat, p_eigen, axis);

    ids.resize(n);
    for (uint32_t i = 0; i < static_cast<uint32_t>(n); ++i) {
        ids(static_cast<Eigen::Index>(p[i])) = i;
    }
}

void sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& row_ids,
    UIntVecDtype& col_ids,
    SortingAlgoID sort_row_method,
    SortingAlgoID sort_col_method
) {
    if (sort_row_method == SortingAlgoID::NO_SORTING) {
        row_ids.resize(1); row_ids(0) = 0;
    } else if (sort_row_method == SortingAlgoID::RANDOM_SORT) {
        random_sort_bin_mat(bin_mat, row_ids, 0);
    } else {
        UTILS_DIE("Sort method not implemented for Eigen");
    }

    if (sort_col_method == SortingAlgoID::NO_SORTING) {
        col_ids.resize(1); col_ids(0) = 0;
    } else if (sort_col_method == SortingAlgoID::RANDOM_SORT) {
        random_sort_bin_mat(bin_mat, col_ids, 1);
    } else {
        UTILS_DIE("Sort method not implemented for Eigen");
    }
}

void invert_sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& row_ids,
    UIntVecDtype& col_ids
) {
    if (row_ids.size() != 1) {
        sort_matrix(bin_mat, row_ids, 0);
        row_ids.resize(1); row_ids(0) = 0;
    }
    if (col_ids.size() != 1) {
        sort_matrix(bin_mat, col_ids, 1);
        col_ids.resize(1); col_ids(0) = 0;
    }
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
        std::vector<uint32_t> p(static_cast<size_t>(row_ids.size()));
        Eigen::Map<UIntVecDtype>(p.data(), row_ids.size()) = row_ids;
        sorted_bin_mat_payload.SetRowIdsPayload(RowColIdsPayload(std::move(p)));
    }
    if (sort_cols_flag) {
        std::vector<uint32_t> p(static_cast<size_t>(col_ids.size()));
        Eigen::Map<UIntVecDtype>(p.data(), col_ids.size()) = col_ids;
        sorted_bin_mat_payload.SetColIdsPayload(RowColIdsPayload(std::move(p)));
    }

    std::vector<uint8_t> payload;
    entropy_encode_bin_mat(bin_mat, codec_ID, payload);
    sorted_bin_mat_payload.SetBinMatPayload(BinMatPayload(
        codec_ID, std::move(payload),
        static_cast<uint32_t>(bin_mat.rows()),
        static_cast<uint32_t>(bin_mat.cols())));
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
        auto p = sorted_bin_mat_payload.GetRowIdsPayload()->GetRowColIdsElements();
        UIntVecDtype row_ids = Eigen::Map<UIntVecDtype>(p.data(), p.size());
        sort_matrix(bin_mat, row_ids, 0);
    }
    if (sort_cols_flag) {
        auto p = sorted_bin_mat_payload.GetColIdsPayload()->GetRowColIdsElements();
        UIntVecDtype col_ids = Eigen::Map<UIntVecDtype>(p.data(), p.size());
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
        std::vector<uint64_t> amax_u64(static_cast<size_t>(amax_vec.size()));
        for(int i=0; i<amax_vec.size(); ++i) amax_u64[i] = amax_vec(i);
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
        UIntVecDtype amax_vec(static_cast<Eigen::Index>(amax_u64.size()));
        for(size_t i=0; i<amax_u64.size(); ++i) amax_vec(i) = static_cast<uint32_t>(amax_u64[i]);
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

} // namespace genie::genotype::detail::eigen
