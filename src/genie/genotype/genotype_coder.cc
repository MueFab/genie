#include "genie/genotype/genotype_coder.h"

#include <genie/util/runtime_exception.h>
#include <genie/backend/backend.h>
#include <codecs/include/mpegg-codecs.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <random>
#include <numeric>

namespace genie::genotype {

void decompose(
    std::vector<core::record::genotype::Record>& recs,
    uint8_t& max_ploidy,
    Int8MatDtype& allele_mat,
    BinMatDtype& phasing_mat,
    size_t block_size
) {
    UTILS_DIE_IF(recs.empty(), "No records found for the process!");
    block_size = std::min(block_size, recs.size());
    uint32_t num_samples = recs.front().GetSampleCount();

    max_ploidy = 0;
    for (size_t idx_i = 0; idx_i < block_size; ++idx_i) {
        max_ploidy = std::max(max_ploidy, static_cast<uint8_t>(recs[idx_i].GetNumberOfAllelesPerSample()));
    }

    ::genie::backend::resize_mat(allele_mat, std::vector<size_t>{block_size, static_cast<size_t>(num_samples * max_ploidy)});
    ::genie::backend::replace_value(allele_mat, static_cast<int8_t>(0), static_cast<int8_t>(-2)); // Placeholder initialization

    ::genie::backend::resize_mat(phasing_mat, std::vector<size_t>{block_size, static_cast<size_t>(num_samples * (max_ploidy > 0 ? max_ploidy - 1 : 0))});

    for (size_t idx_i = 0; idx_i < block_size; ++idx_i) {
        auto& rec = recs[idx_i];
        auto& alleles = rec.GetAlleles();
        auto& phasing = rec.GetPhasing();
        uint8_t rec_ploidy = static_cast<uint8_t>(rec.GetNumberOfAllelesPerSample());

        for (uint32_t idx_j = 0; idx_j < num_samples; ++idx_j) {
            for (uint8_t idx_k = 0; idx_k < rec_ploidy; ++idx_k) {
                ::genie::backend::set_mat_element(allele_mat, idx_i, idx_j * max_ploidy + idx_k, static_cast<int8_t>(alleles[idx_j][idx_k]));
            }
            if (max_ploidy > 1) {
                for (uint8_t idx_k = 0; idx_k < rec_ploidy - 1; ++idx_k) {
                    ::genie::backend::set_mat_element(phasing_mat, idx_i, idx_j * (max_ploidy - 1) + idx_k, phasing[idx_j][idx_k]);
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
    no_ref_flag = ::genie::backend::any_equal(allele_mat, static_cast<int8_t>(-1));
    not_avail_flag = ::genie::backend::any_equal(allele_mat, static_cast<int8_t>(-2));

    int8_t current_max = ::genie::backend::max_coeff(allele_mat);
    if (current_max < 0) current_max = 0;

    if (no_ref_flag) {
        current_max++;
        ::genie::backend::replace_value(allele_mat, static_cast<int8_t>(-1), current_max);
    }

    if (not_avail_flag) {
        current_max++;
        ::genie::backend::replace_value(allele_mat, static_cast<int8_t>(-2), current_max);
    }
}

void inverse_transform_max_val(Int8MatDtype& allele_mat, bool no_ref_flag, bool not_avail_flag) {
    if (::genie::backend::get_mat_shape(allele_mat, 0) == 0) return;
    int8_t current_max = ::genie::backend::max_coeff(allele_mat);

    if (not_avail_flag) {
        ::genie::backend::replace_value(allele_mat, current_max, static_cast<int8_t>(-2));
        current_max--;
    }

    if (no_ref_flag) {
        ::genie::backend::replace_value(allele_mat, current_max, static_cast<int8_t>(-1));
    }
}

void binarize_bit_plane(
    Int8MatDtype& allele_mat,
    std::vector<BinMatDtype>& bin_mats,
    uint8_t& num_bit_planes,
    const ConcatAxis concat_axis
) {
    size_t nrows = ::genie::backend::get_mat_shape(allele_mat, 0);
    size_t ncols = ::genie::backend::get_mat_shape(allele_mat, 1);

    if (nrows == 0) {
        num_bit_planes = 1;
        bin_mats.clear();
        return;
    }

    int8_t max_val = ::genie::backend::max_coeff(allele_mat);
    num_bit_planes = static_cast<uint8_t>(std::ceil(std::log2(max_val + 1)));
    if (num_bit_planes == 0) num_bit_planes = 1;

    bin_mats.resize(num_bit_planes);
    for (uint8_t idx_k = 0; idx_k < num_bit_planes; ++idx_k) {
        ::genie::backend::get_bit_plane(allele_mat, idx_k, bin_mats[idx_k]);
    }

    if (concat_axis != ConcatAxis::DO_NOT_CONCAT) {
        BinMatDtype combined;
        if (concat_axis == ConcatAxis::CONCAT_ROW_DIR) {
            ::genie::backend::resize_mat(combined, std::vector<size_t>{nrows * num_bit_planes, ncols});
            for (uint8_t idx_k = 0; idx_k < num_bit_planes; ++idx_k) {
                for (size_t idx_i = 0; idx_i < nrows; ++idx_i) {
                    for (size_t idx_j = 0; idx_j < ncols; ++idx_j) {
                        ::genie::backend::set_mat_element(combined, idx_k * nrows + idx_i, idx_j, ::genie::backend::get_mat_element(bin_mats[idx_k], idx_i, idx_j));
                    }
                }
            }
        } else if (concat_axis == ConcatAxis::CONCAT_COL_DIR) {
            ::genie::backend::resize_mat(combined, std::vector<size_t>{nrows, ncols * num_bit_planes});
            for (uint8_t idx_k = 0; idx_k < num_bit_planes; ++idx_k) {
                for (size_t idx_i = 0; idx_i < nrows; ++idx_i) {
                    for (size_t idx_j = 0; idx_j < ncols; ++idx_j) {
                        ::genie::backend::set_mat_element(combined, idx_i, idx_k * ncols + idx_j, ::genie::backend::get_mat_element(bin_mats[idx_k], idx_i, idx_j));
                    }
                }
            }
        }
        bin_mats.clear();
        bin_mats.push_back(std::move(combined));
    }
    ::genie::backend::resize_mat(allele_mat, std::vector<size_t>{0, 0});
}

void debinarize_bit_plane(
    std::vector<BinMatDtype>& bin_mats,
    uint8_t num_bit_planes,
    const ConcatAxis concat_axis,
    Int8MatDtype& allele_mat
) {
    if (bin_mats.empty()) return;

    size_t nrows, ncols;
    if (concat_axis == ConcatAxis::CONCAT_ROW_DIR) {
        auto& combined = bin_mats.front();
        size_t total_rows = ::genie::backend::get_mat_shape(combined, 0);
        nrows = total_rows / num_bit_planes;
        ncols = ::genie::backend::get_mat_shape(combined, 1);
        std::vector<BinMatDtype> split_mats(num_bit_planes);
        for (uint8_t idx_k = 0; idx_k < num_bit_planes; ++idx_k) {
            ::genie::backend::resize_mat(split_mats[idx_k], std::vector<size_t>{nrows, ncols});
            for (size_t idx_i = 0; idx_i < nrows; ++idx_i) {
                for (size_t idx_j = 0; idx_j < ncols; ++idx_j) {
                    ::genie::backend::set_mat_element(split_mats[idx_k], idx_i, idx_j, ::genie::backend::get_mat_element(combined, idx_k * nrows + idx_i, idx_j));
                }
            }
        }
        bin_mats = std::move(split_mats);
    } else if (concat_axis == ConcatAxis::CONCAT_COL_DIR) {
        auto& combined = bin_mats.front();
        nrows = ::genie::backend::get_mat_shape(combined, 0);
        size_t total_cols = ::genie::backend::get_mat_shape(combined, 1);
        ncols = total_cols / num_bit_planes;
        std::vector<BinMatDtype> split_mats(num_bit_planes);
        for (uint8_t idx_k = 0; idx_k < num_bit_planes; ++idx_k) {
            ::genie::backend::resize_mat(split_mats[idx_k], std::vector<size_t>{nrows, ncols});
            for (size_t idx_i = 0; idx_i < nrows; ++idx_i) {
                for (size_t idx_j = 0; idx_j < ncols; ++idx_j) {
                    ::genie::backend::set_mat_element(split_mats[idx_k], idx_i, idx_j, ::genie::backend::get_mat_element(combined, idx_i, idx_k * ncols + idx_j));
                }
            }
        }
        bin_mats = std::move(split_mats);
    }

    nrows = ::genie::backend::get_mat_shape(bin_mats[0], 0);
    ncols = ::genie::backend::get_mat_shape(bin_mats[0], 1);
    ::genie::backend::resize_mat(allele_mat, std::vector<size_t>{nrows, ncols});
    for(size_t idx_i=0; idx_i<nrows; ++idx_i) for(size_t idx_j=0; idx_j<ncols; ++idx_j) ::genie::backend::set_mat_element(allele_mat, idx_i, idx_j, (int8_t)0);

    for (size_t idx_k = 0; idx_k < bin_mats.size(); ++idx_k) {
        for (size_t idx_i = 0; idx_i < nrows; ++idx_i) {
            for (size_t idx_j = 0; idx_j < ncols; ++idx_j) {
                if (::genie::backend::get_mat_element(bin_mats[idx_k], idx_i, idx_j)) {
                    int8_t val = ::genie::backend::get_mat_element(allele_mat, idx_i, idx_j);
                    ::genie::backend::set_mat_element(allele_mat, idx_i, idx_j, static_cast<int8_t>(val | (1 << idx_k)));
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
    size_t nrows = ::genie::backend::get_mat_shape(allele_mat, 0);
    size_t ncols = ::genie::backend::get_mat_shape(allele_mat, 1);
    if (nrows == 0) return;

    ::genie::backend::resize_arr(amax_vec, nrows);
    size_t total_bin_rows = 0;
    for (size_t idx_i = 0; idx_i < nrows; ++idx_i) {
        int8_t row_max = 0;
        for (size_t idx_j = 0; idx_j < ncols; ++idx_j) {
            int8_t val = ::genie::backend::get_mat_element(allele_mat, idx_i, idx_j);
            if (val > row_max) row_max = val;
        }
        uint32_t bits = static_cast<uint32_t>(std::ceil(std::log2(row_max + 1)));
        if (bits == 0) bits = 1;
        ::genie::backend::set_arr_element(amax_vec, idx_i, bits);
        total_bin_rows += bits;
    }

    BinMatDtype bin_mat;
    ::genie::backend::resize_mat(bin_mat, std::vector<size_t>{total_bin_rows, ncols});
    size_t current_row = 0;
    for (size_t idx_i = 0; idx_i < nrows; ++idx_i) {
        uint32_t bits = ::genie::backend::get_arr_element(amax_vec, idx_i);
        for (uint32_t idx_k = 0; idx_k < bits; ++idx_k) {
            for (size_t idx_j = 0; idx_j < ncols; ++idx_j) {
                bool bit_val = (::genie::backend::get_mat_element(allele_mat, idx_i, idx_j) & (1 << idx_k)) != 0;
                ::genie::backend::set_mat_element(bin_mat, current_row, idx_j, bit_val);
            }
            current_row++;
        }
    }
    bin_mats.push_back(std::move(bin_mat));
    ::genie::backend::resize_mat(allele_mat, std::vector<size_t>{0, 0});
}

void debinarize_row_bin(
    std::vector<BinMatDtype>& bin_mats,
    UIntVecDtype& amax_vec,
    Int8MatDtype& allele_mat
) {
    if (bin_mats.empty()) return;
    auto& bin_mat = bin_mats.front();
    size_t nrows = ::genie::backend::get_arr_size(amax_vec);
    size_t ncols = ::genie::backend::get_mat_shape(bin_mat, 1);

    ::genie::backend::resize_mat(allele_mat, std::vector<size_t>{nrows, ncols});
    // Manual zero init as resize_mat might not zero out
    for(size_t idx_i=0; idx_i<nrows; ++idx_i) for(size_t idx_j=0; idx_j<ncols; ++idx_j) ::genie::backend::set_mat_element(allele_mat, idx_i, idx_j, (int8_t)0);

    size_t current_row = 0;
    for (size_t idx_i = 0; idx_i < nrows; ++idx_i) {
        uint32_t bits = ::genie::backend::get_arr_element(amax_vec, idx_i);
        for (uint32_t idx_k = 0; idx_k < bits; ++idx_k) {
            for (size_t idx_j = 0; idx_j < ncols; ++idx_j) {
                if (::genie::backend::get_mat_element(bin_mat, current_row, idx_j)) {
                    int8_t val = ::genie::backend::get_mat_element(allele_mat, idx_i, idx_j);
                    ::genie::backend::set_mat_element(allele_mat, idx_i, idx_j, static_cast<int8_t>(val | (1 << idx_k)));
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
    size_t n_ids = ::genie::backend::get_arr_size(ids);
    if (n_ids == 0 || ::genie::backend::get_mat_shape(bin_mat, 0) == 0) return;

    if (axis == 0) {
        size_t ncols = ::genie::backend::get_mat_shape(bin_mat, 1);
        BinMatDtype temp;
        ::genie::backend::resize_mat(temp, std::vector<size_t>{n_ids, ncols});
        for (size_t idx_i = 0; idx_i < n_ids; ++idx_i) {
            uint32_t src_idx = ::genie::backend::get_arr_element(ids, idx_i);
            for (size_t idx_j = 0; idx_j < ncols; ++idx_j) {
                ::genie::backend::set_mat_element(temp, idx_i, idx_j, ::genie::backend::get_mat_element(bin_mat, src_idx, idx_j));
            }
        }
        bin_mat = std::move(temp);
    } else if (axis == 1) {
        size_t nrows = ::genie::backend::get_mat_shape(bin_mat, 0);
        BinMatDtype temp;
        ::genie::backend::resize_mat(temp, std::vector<size_t>{nrows, n_ids});
        for (size_t idx_i = 0; idx_i < nrows; ++idx_i) {
            for (size_t idx_j = 0; idx_j < n_ids; ++idx_j) {
                uint32_t src_idx = ::genie::backend::get_arr_element(ids, idx_j);
                ::genie::backend::set_mat_element(temp, idx_i, idx_j, ::genie::backend::get_mat_element(bin_mat, idx_i, src_idx));
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
    size_t n = ::genie::backend::get_mat_shape(bin_mat, axis);
    std::vector<uint32_t> p(n);
    std::iota(p.begin(), p.end(), 0);

    static std::random_device rd;
    static std::mt19937 g(rd());
    std::shuffle(p.begin(), p.end(), g);

    UIntVecDtype p_vec;
    ::genie::backend::resize_arr(p_vec, n);
    for(size_t idx_i=0; idx_i<n; ++idx_i) ::genie::backend::set_arr_element(p_vec, idx_i, p[idx_i]);

    sort_matrix(bin_mat, p_vec, axis);

    ::genie::backend::resize_arr(ids, n);
    for (uint32_t idx_i = 0; idx_i < n; ++idx_i) {
        ::genie::backend::set_arr_element(ids, p[idx_i], idx_i);
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
        ::genie::backend::resize_arr(row_ids, 1);
        ::genie::backend::set_arr_element(row_ids, 0, 0);
    } else if (sort_row_method == SortingAlgoID::RANDOM_SORT) {
        random_sort_bin_mat(bin_mat, row_ids, 0);
    } else {
        UTILS_DIE("Sort method not implemented in unified coder");
    }

    if (sort_col_method == SortingAlgoID::NO_SORTING) {
        ::genie::backend::resize_arr(col_ids, 1);
        ::genie::backend::set_arr_element(col_ids, 0, 0);
    } else if (sort_col_method == SortingAlgoID::RANDOM_SORT) {
        random_sort_bin_mat(bin_mat, col_ids, 1);
    } else {
        UTILS_DIE("Sort method not implemented in unified coder");
    }
}

void invert_sort_bin_mat(
    BinMatDtype& bin_mat,
    UIntVecDtype& row_ids,
    UIntVecDtype& col_ids
) {
    if (::genie::backend::get_arr_size(row_ids) != 1) {
        sort_matrix(bin_mat, row_ids, 0);
        ::genie::backend::resize_arr(row_ids, 1);
        ::genie::backend::set_arr_element(row_ids, 0, 0);
    }
    if (::genie::backend::get_arr_size(col_ids) != 1) {
        sort_matrix(bin_mat, col_ids, 1);
        ::genie::backend::resize_arr(col_ids, 1);
        ::genie::backend::set_arr_element(col_ids, 0, 0);
    }
}

void bin_mat_to_bytes(
    const BinMatDtype& bin_mat,
    uint8_t** payload,
    size_t& payload_len
) {
    ::genie::backend::bin_mat_to_bytes(bin_mat, payload, payload_len);
}

void bin_mat_from_bytes(
    const uint8_t* payload,
    size_t payload_len,
    size_t nrows,
    size_t ncols,
    BinMatDtype& bin_mat
) {
    ::genie::backend::bin_mat_from_bytes(payload, payload_len, nrows, ncols, bin_mat);
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

    size_t nrows = ::genie::backend::get_mat_shape(bin_mat, 0);
    size_t ncols = ::genie::backend::get_mat_shape(bin_mat, 1);

    switch (codec_ID) {
        case genie::core::AlgoID::JBIG: {
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
        std::vector<uint32_t> p(::genie::backend::get_arr_size(row_ids));
        for(size_t idx_i=0; idx_i<p.size(); ++idx_i) p[idx_i] = ::genie::backend::get_arr_element(row_ids, idx_i);
        sorted_bin_mat_payload.SetRowIdsPayload(RowColIdsPayload(std::move(p)));
    }
    if (sort_cols_flag) {
        std::vector<uint32_t> p(::genie::backend::get_arr_size(col_ids));
        for(size_t idx_i=0; idx_i<p.size(); ++idx_i) p[idx_i] = ::genie::backend::get_arr_element(col_ids, idx_i);
        sorted_bin_mat_payload.SetColIdsPayload(RowColIdsPayload(std::move(p)));
    }

    std::vector<uint8_t> payload;
    entropy_encode_bin_mat(bin_mat, codec_ID, payload);
    sorted_bin_mat_payload.SetBinMatPayload(BinMatPayload(
        codec_ID, std::move(payload),
        static_cast<uint32_t>(::genie::backend::get_mat_shape(bin_mat, 0)),
        static_cast<uint32_t>(::genie::backend::get_mat_shape(bin_mat, 1))));
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
        UIntVecDtype row_ids;
        ::genie::backend::resize_arr(row_ids, p.size());
        for(size_t idx_i=0; idx_i<p.size(); ++idx_i) ::genie::backend::set_arr_element(row_ids, idx_i, p[idx_i]);
        sort_matrix(bin_mat, row_ids, 0);
    }
    if (sort_cols_flag) {
        auto p = sorted_bin_mat_payload.GetColIdsPayload()->GetRowColIdsElements();
        UIntVecDtype col_ids;
        ::genie::backend::resize_arr(col_ids, p.size());
        for(size_t idx_i=0; idx_i<p.size(); ++idx_i) ::genie::backend::set_arr_element(col_ids, idx_i, p[idx_i]);
        sort_matrix(bin_mat, col_ids, 1);
    }
}

void encode_genotype(
    std::vector<core::record::genotype::Record>& recs,
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
        size_t n = ::genie::backend::get_arr_size(amax_vec);
        std::vector<uint64_t> amax_u64(n);
        for(size_t idx_i=0; idx_i<n; ++idx_i) amax_u64[idx_i] = ::genie::backend::get_arr_element(amax_vec, idx_i);
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
        UIntVecDtype amax_vec;
        ::genie::backend::resize_arr(amax_vec, amax_u64.size());
        for(size_t idx_i=0; idx_i<amax_u64.size(); ++idx_i) ::genie::backend::set_arr_element(amax_vec, idx_i, static_cast<uint32_t>(amax_u64[idx_i]));
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

} // namespace genie::genotype
