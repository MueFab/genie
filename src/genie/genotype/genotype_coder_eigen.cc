#include "genotype_coder_eigen.h"
#include <genie/util/runtime_exception.h>
#include <codecs/include/mpegg-codecs.h>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace genie::genotype::detail::eigen {

using EigenBinMat = Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using EigenInt8Mat = Eigen::Matrix<int8_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using EigenUIntVec = Eigen::Matrix<uint32_t, Eigen::Dynamic, 1>;

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

    detail::eigen::bin_mat_to_bytes(bin_mat, &raw_data, raw_data_len);

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

    detail::eigen::bin_mat_from_bytes(raw_data, raw_data_len, nrows, ncols, bin_mat);
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

    EigenInt8Mat xt_allele_mat(static_cast<Eigen::Index>(block_size), static_cast<Eigen::Index>(num_samples * max_ploidy));
    xt_allele_mat.setConstant(-2);

    EigenBinMat xt_phasing_mat(static_cast<Eigen::Index>(block_size), static_cast<Eigen::Index>(num_samples * (max_ploidy > 0 ? max_ploidy - 1 : 0)));
    xt_phasing_mat.setConstant(false);

    for (size_t i = 0; i < block_size; ++i) {
        auto& rec = recs[i];
        auto& alleles = rec.GetAlleles();
        auto& phasing = rec.GetPhasing();
        uint8_t rec_ploidy = static_cast<uint8_t>(rec.GetNumberOfAllelesPerSample());

        for (uint32_t j = 0; j < num_samples; ++j) {
            for (uint8_t k = 0; k < rec_ploidy; ++k) {
                xt_allele_mat(static_cast<Eigen::Index>(i), static_cast<Eigen::Index>(j * max_ploidy + k)) = static_cast<int8_t>(alleles[j][k]);
            }
            if (max_ploidy > 1) {
                for (uint8_t k = 0; k < rec_ploidy - 1; ++k) {
                    xt_phasing_mat(static_cast<Eigen::Index>(i), static_cast<Eigen::Index>(j * (max_ploidy - 1) + k)) = phasing[j][k];
                }
            }
        }
    }
    
    allele_mat.assign(block_size, std::vector<int8_t>(num_samples * max_ploidy));
    for (size_t i = 0; i < block_size; ++i)
        for (size_t j = 0; j < (size_t)xt_allele_mat.cols(); ++j)
            allele_mat[i][j] = xt_allele_mat(i, j);
            
    phasing_mat.assign(block_size, std::vector<bool>(xt_phasing_mat.cols()));
    for (size_t i = 0; i < block_size; ++i)
        for (size_t j = 0; j < (size_t)xt_phasing_mat.cols(); ++j)
            phasing_mat[i][j] = xt_phasing_mat(i, j);
}

void transform_max_value(
    Int8MatDtype& allele_mat,
    bool& no_ref_flag,
    bool& not_avail_flag
) {
    if (allele_mat.empty()) return;
    Eigen::Index nrows = allele_mat.size();
    Eigen::Index ncols = allele_mat[0].size();
    EigenInt8Mat xt_allele_mat(nrows, ncols);
    for (int i=0; i<nrows; ++i) for (int j=0; j<ncols; ++j) xt_allele_mat(i, j) = allele_mat[i][j];

    no_ref_flag = (xt_allele_mat.array() == -1).any();
    not_avail_flag = (xt_allele_mat.array() == -2).any();

    int8_t current_max = xt_allele_mat.maxCoeff();
    if (current_max < 0) current_max = 0;

    if (no_ref_flag) {
        current_max++;
        int8_t target = current_max;
        for(int i=0; i<xt_allele_mat.rows(); ++i)
            for(int j=0; j<xt_allele_mat.cols(); ++j)
                if (xt_allele_mat(i,j) == -1) xt_allele_mat(i,j) = target;
    }

    if (not_avail_flag) {
        current_max++;
        int8_t target = current_max;
        for(int i=0; i<xt_allele_mat.rows(); ++i)
            for(int j=0; j<xt_allele_mat.cols(); ++j)
                if (xt_allele_mat(i,j) == -2) xt_allele_mat(i,j) = target;
    }
    
    for (int i=0; i<nrows; ++i) for (int j=0; j<ncols; ++j) allele_mat[i][j] = xt_allele_mat(i, j);
}

void inverse_transform_max_val(Int8MatDtype& allele_mat, bool no_ref_flag, bool not_avail_flag) {
    if (allele_mat.empty()) return;
    Eigen::Index nrows = allele_mat.size();
    Eigen::Index ncols = allele_mat[0].size();
    EigenInt8Mat xt_allele_mat(nrows, ncols);
    for (int i=0; i<nrows; ++i) for (int j=0; j<ncols; ++j) xt_allele_mat(i, j) = allele_mat[i][j];

    int8_t current_max = xt_allele_mat.maxCoeff();

    if (not_avail_flag) {
        for(int i=0; i<xt_allele_mat.rows(); ++i)
            for(int j=0; j<xt_allele_mat.cols(); ++j)
                if (xt_allele_mat(i,j) == current_max) xt_allele_mat(i,j) = -2;
        current_max--;
    }

    if (no_ref_flag) {
        for(int i=0; i<xt_allele_mat.rows(); ++i)
            for(int j=0; j<xt_allele_mat.cols(); ++j)
                if (xt_allele_mat(i,j) == current_max) xt_allele_mat(i,j) = -1;
    }
    
    for (int i=0; i<nrows; ++i) for (int j=0; j<ncols; ++j) allele_mat[i][j] = xt_allele_mat(i, j);
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
    Eigen::Index nrows = allele_mat.size();
    Eigen::Index ncols = allele_mat[0].size();
    EigenInt8Mat xt_allele_mat(nrows, ncols);
    for (int i=0; i<nrows; ++i) for (int j=0; j<ncols; ++j) xt_allele_mat(i, j) = allele_mat[i][j];

    int8_t max_val = xt_allele_mat.maxCoeff();
    num_bit_planes = static_cast<uint8_t>(std::ceil(std::log2(max_val + 1)));
    if (num_bit_planes == 0) num_bit_planes = 1;

    std::vector<EigenBinMat> xt_bin_mats(num_bit_planes);
    for (uint8_t k = 0; k < num_bit_planes; ++k) {
        xt_bin_mats[k].resize(nrows, ncols);
        for(int i=0; i<nrows; ++i)
            for(int j=0; j<ncols; ++j)
                xt_bin_mats[k](i, j) = (xt_allele_mat(i, j) & (1 << k)) != 0;
    }

    if (concat_axis != ConcatAxis::DO_NOT_CONCAT) {
        if (concat_axis == ConcatAxis::CONCAT_ROW_DIR) {
            EigenBinMat combined(nrows * num_bit_planes, ncols);
            for (uint8_t k = 0; k < num_bit_planes; ++k) {
                combined.middleRows(k * (Eigen::Index)nrows, (Eigen::Index)nrows) = xt_bin_mats[k];
            }
            xt_bin_mats = {std::move(combined)};
        } else if (concat_axis == ConcatAxis::CONCAT_COL_DIR) {
            EigenBinMat combined(nrows, ncols * num_bit_planes);
            for (uint8_t k = 0; k < num_bit_planes; ++k) {
                combined.middleCols(k * (Eigen::Index)ncols, (Eigen::Index)ncols) = xt_bin_mats[k];
            }
            xt_bin_mats = {std::move(combined)};
        }
    }
    
    bin_mats.clear();
    for (const auto& xt_bm : xt_bin_mats) {
        BinMatDtype bm(xt_bm.rows(), std::vector<bool>(xt_bm.cols()));
        for (int i=0; i<xt_bm.rows(); ++i) for (int j=0; j<xt_bm.cols(); ++j) bm[i][j] = xt_bm(i, j);
        bin_mats.push_back(std::move(bm));
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
    
    std::vector<EigenBinMat> xt_bin_mats;
    for (const auto& bm : bin_mats) {
        EigenBinMat xt_bm(bm.size(), bm[0].size());
        for (size_t i=0; i<bm.size(); ++i) for (size_t j=0; j<bm[0].size(); ++j) xt_bm(i, j) = bm[i][j];
        xt_bin_mats.push_back(std::move(xt_bm));
    }

    if (concat_axis == ConcatAxis::CONCAT_ROW_DIR) {
        EigenBinMat combined = std::move(xt_bin_mats[0]);
        Eigen::Index nrows = combined.rows() / num_bit_planes;
        Eigen::Index ncols = combined.cols();
        xt_bin_mats.clear();
        for (uint8_t k = 0; k < num_bit_planes; ++k) {
            xt_bin_mats.push_back(combined.middleRows(k * nrows, nrows));
        }
    } else if (concat_axis == ConcatAxis::CONCAT_COL_DIR) {
        EigenBinMat combined = std::move(xt_bin_mats[0]);
        Eigen::Index nrows = combined.rows();
        Eigen::Index ncols = combined.cols() / num_bit_planes;
        xt_bin_mats.clear();
        for (uint8_t k = 0; k < num_bit_planes; ++k) {
            xt_bin_mats.push_back(combined.middleCols(k * ncols, ncols));
        }
    }

    Eigen::Index nrows = xt_bin_mats[0].rows();
    Eigen::Index ncols = xt_bin_mats[0].cols();
    EigenInt8Mat xt_allele_mat(nrows, ncols);
    xt_allele_mat.setZero();

    for (uint8_t k = 0; k < static_cast<uint8_t>(xt_bin_mats.size()); ++k) {
        for(int i=0; i<nrows; ++i)
            for(int j=0; j<ncols; ++j)
                if (xt_bin_mats[k](i, j)) xt_allele_mat(i, j) |= (1 << k);
    }
    
    allele_mat.assign(nrows, std::vector<int8_t>(ncols));
    for (int i=0; i<nrows; ++i) for (int j=0; j<ncols; ++j) allele_mat[i][j] = xt_allele_mat(i, j);
}

void binarize_row_bin(
    Int8MatDtype& allele_mat,
    std::vector<BinMatDtype>& bin_mats,
    UIntVecDtype& amax_vec
) {
    if (allele_mat.empty()) return;
    Eigen::Index nrows = allele_mat.size();
    Eigen::Index ncols = allele_mat[0].size();
    EigenInt8Mat xt_allele_mat(nrows, ncols);
    for (int i=0; i<nrows; ++i) for (int j=0; j<ncols; ++j) xt_allele_mat(i, j) = allele_mat[i][j];

    amax_vec.resize(nrows);
    Eigen::Index total_bin_rows = 0;
    for (int i = 0; i < nrows; ++i) {
        int8_t row_max = xt_allele_mat.row(i).maxCoeff();
        amax_vec[i] = static_cast<uint32_t>(std::ceil(std::log2(row_max + 1)));
        if (amax_vec[i] == 0) amax_vec[i] = 1;
        total_bin_rows += amax_vec[i];
    }

    EigenBinMat xt_bin_mat(total_bin_rows, ncols);
    Eigen::Index current_row = 0;
    for (int i = 0; i < nrows; ++i) {
        for (uint32_t k = 0; k < amax_vec[i]; ++k) {
            for (int j = 0; j < ncols; ++j) {
                xt_bin_mat(current_row, j) = (xt_allele_mat(i, j) & (1 << k)) != 0;
            }
            current_row++;
        }
    }
    BinMatDtype bm(total_bin_rows, std::vector<bool>(ncols));
    for (int i=0; i<total_bin_rows; ++i) for (int j=0; j<ncols; ++j) bm[i][j] = xt_bin_mat(i, j);
    bin_mats.push_back(std::move(bm));
    allele_mat.clear();
}

void debinarize_row_bin(
    std::vector<BinMatDtype>& bin_mats,
    UIntVecDtype& amax_vec,
    Int8MatDtype& allele_mat
) {
    if (bin_mats.empty()) return;
    auto& bin_mat = bin_mats.front();
    Eigen::Index nrows = amax_vec.size();
    Eigen::Index ncols = bin_mat[0].size();

    EigenInt8Mat xt_allele_mat(nrows, ncols);
    xt_allele_mat.setZero();
    Eigen::Index current_row = 0;
    for (int i = 0; i < nrows; ++i) {
        for (uint32_t k = 0; k < amax_vec[i]; ++k) {
            for (int j = 0; j < ncols; ++j) {
                if (bin_mat[current_row][j]) {
                    xt_allele_mat(i, j) |= (1 << k);
                }
            }
            current_row++;
        }
    }
    allele_mat.assign(nrows, std::vector<int8_t>(ncols));
    for (int i=0; i<nrows; ++i) for (int j=0; j<ncols; ++j) allele_mat[i][j] = xt_allele_mat(i, j);
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
        detail::eigen::binarize_bit_plane(allele_mat, bin_mats, num_bit_planes, concat_axis);
    } else if (binarization_ID == BinarizationID::ROW_BIN) {
        num_bit_planes = 1;
        detail::eigen::binarize_row_bin(allele_mat, bin_mats, amax_vec);
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
    Eigen::Index nrows = bin_mat.size();
    Eigen::Index ncols = bin_mat[0].size();
    if (axis == 0) {
        BinMatDtype temp(ids.size());
        for (size_t i = 0; i < ids.size(); ++i) {
            temp[i] = bin_mat[ids[i]];
        }
        bin_mat = std::move(temp);
    } else if (axis == 1) {
        BinMatDtype temp(nrows, std::vector<bool>(ids.size()));
        for (size_t i = 0; i < (size_t)nrows; ++i) {
            for (size_t j = 0; j < ids.size(); ++j) {
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
    std::vector<uint32_t> p(n);
    std::iota(p.begin(), p.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(p.begin(), p.end(), g);
    
    detail::eigen::sort_matrix(bin_mat, p, axis);

    ids.assign(n, 0);
    for (uint32_t i = 0; i < (uint32_t)n; ++i) {
        ids[p[i]] = i;
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
        row_ids = {0};
    } else if (sort_row_method == SortingAlgoID::RANDOM_SORT) {
        detail::eigen::random_sort_bin_mat(bin_mat, row_ids, 0);
    } else {
        UTILS_DIE("Sort method not implemented for Eigen");
    }

    if (sort_col_method == SortingAlgoID::NO_SORTING) {
        col_ids = {0};
    } else if (sort_col_method == SortingAlgoID::RANDOM_SORT) {
        detail::eigen::random_sort_bin_mat(bin_mat, col_ids, 1);
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
        detail::eigen::sort_matrix(bin_mat, row_ids, 0);
        row_ids = {0};
    }
    if (col_ids.size() != 1) {
        detail::eigen::sort_matrix(bin_mat, col_ids, 1);
        col_ids = {0};
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
    detail::eigen::sort_bin_mat(bin_mat, row_ids, col_ids, sort_row_method, sort_col_method);

    if (sort_rows_flag) {
        sorted_bin_mat_payload.SetRowIdsPayload(RowColIdsPayload(row_ids));
    }
    if (sort_cols_flag) {
        sorted_bin_mat_payload.SetColIdsPayload(RowColIdsPayload(col_ids));
    }

    std::vector<uint8_t> payload;
    detail::eigen::entropy_encode_bin_mat(bin_mat, codec_ID, payload);
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
    detail::eigen::entropy_decode_bin_mat(bin_mat_payload.GetPayload(), codec_ID, bin_mat_payload.GetNRows(), bin_mat_payload.GetNCols(), bin_mat);

    if (sort_rows_flag) {
        auto row_ids = sorted_bin_mat_payload.GetRowIdsPayload()->GetRowColIdsElements();
        detail::eigen::sort_matrix(bin_mat, row_ids, 0);
    }
    if (sort_cols_flag) {
        auto col_ids = sorted_bin_mat_payload.GetColIdsPayload()->GetRowColIdsElements();
        detail::eigen::sort_matrix(bin_mat, col_ids, 1);
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
    detail::eigen::decompose(recs, max_ploidy, allele_mat, phasing_mat, block_size);

    payload.SetMaxPloidy(max_ploidy);

    bool dot_flag, na_flag;
    detail::eigen::transform_max_value(allele_mat, dot_flag, na_flag);
    payload.SetNoReferenceFlag(dot_flag);
    payload.SetNotAvailableFlag(na_flag);

    std::vector<BinMatDtype> allele_bin_mats;
    uint8_t num_bit_planes;
    UIntVecDtype amax_vec;
    detail::eigen::binarize_allele_mat(allele_mat, allele_bin_mats, num_bit_planes, amax_vec, binarization_ID, concat_axis);

    payload.SetNumBitPlanes(num_bit_planes);
    if (binarization_ID == BinarizationID::ROW_BIN) {
        std::vector<uint64_t> amax_u64(amax_vec.begin(), amax_vec.end());
        payload.SetVariantsAmaxPayload(AmaxPayload(std::move(amax_u64)));
    }

    for (auto& bm : allele_bin_mats) {
        SortedBinMatPayload sbm;
        detail::eigen::encode_and_sort_bin_mat(bm, sbm, sort_row_method, sort_col_method, codec_ID);
        payload.AddVariantsPayload(std::move(sbm));
    }

    if (params.GetEncodePhasesDataFlag()) {
        SortedBinMatPayload ph_sbm;
        detail::eigen::encode_and_sort_bin_mat(phasing_mat, ph_sbm, sort_row_method, sort_col_method, codec_ID);
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
        detail::eigen::decode_and_inverse_sort_bin_mat(sbm, bm, params.GetVariantsCodecID(), params.GetSortVariantsRowsFlag(), params.GetSortVariantsColsFlag());
        bin_mats.push_back(std::move(bm));
    }

    if (params.GetBinarizationID() == BinarizationID::ROW_BIN) {
        auto amax_u64 = payload.GetVariantsAmaxPayload()->GetAmaxElements();
        UIntVecDtype amax_vec(amax_u64.begin(), amax_u64.end());
        detail::eigen::debinarize_row_bin(bin_mats, amax_vec, allele_mat);
    } else {
        detail::eigen::debinarize_bit_plane(bin_mats, payload.GetNumBitPlanes(), params.GetConcatAxis(), allele_mat);
    }

    auto ph_sbm = payload.GetPhasesPayload();
    if (ph_sbm) {
        detail::eigen::decode_and_inverse_sort_bin_mat(*ph_sbm, phasing_mat, params.GetPhasesCodecID(), params.GetSortPhasesRowsFlag(), params.GetSortPhasesColsFlag());
    }

    detail::eigen::inverse_transform_max_val(allele_mat, payload.GetNoReferenceFlag(), payload.GetNotAvailableFlag());
}

} // namespace genie::genotype::detail::eigen