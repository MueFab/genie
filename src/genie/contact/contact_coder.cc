/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "contact_coder.h"
#include <codecs/include/mpegg-codecs.h>
#include <genie/core/record/contact/record.h>
#include <genie/util/runtime_exception.h>
#include <cstdint>
#include <cstring>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <iostream>
#include "contact_matrix_parameters.h"
#include "contact_matrix_tile_payload.h"
#include "subcontact_matrix_parameters.h"
#include "subcontact_matrix_payload.h"
#include "subcontact_matrix_mask_payload.h"
#include "genie/backend/backend.h"
#include <unordered_map>

namespace genie::contact {

namespace detail {

struct PairHash {
    size_t operator()(const std::pair<size_t, size_t>& p) const {
        return p.first * 31 + p.second;
    }
};

struct PairHash64 {
    size_t operator()(const std::pair<uint64_t, uint64_t>& p) const {
        return static_cast<size_t>(p.first * 31 + p.second);
    }
};

inline void assign_vec_to_arr(UInt64VecDtype& dest, const std::vector<uint64_t>& src) {
    ::genie::backend::resize_arr(dest, src.size());
    for (size_t i = 0; i < src.size(); ++i) {
        ::genie::backend::set_arr_element(dest, i, src[i]);
    }
}
inline void assign_vec_to_arr(UIntVecDtype& dest, const std::vector<uint32_t>& src) {
    ::genie::backend::resize_arr(dest, src.size());
    for (size_t i = 0; i < src.size(); ++i) {
        ::genie::backend::set_arr_element(dest, i, src[i]);
    }
}

}

// ---------------------------------------------------------------------------------------------------------------------

void set_rle_information_from_mask(
    RunLengthEncodingData& rleData,
    const BinVecDtype& scm_mask
) {
    size_t nelems = ::genie::backend::get_arr_size(scm_mask);
    if (nelems == 0) {
        rleData.maxCount = 0;
        rleData.transformID = TransformID::ID_0;
        return;
    }

    rleData.firstVal = ::genie::backend::get_arr_element(scm_mask, 0);
    rleData.rl_entries.clear();

    uint32_t current_count = 0;
    bool current_val = rleData.firstVal;

    for (size_t i = 0; i < nelems; ++i) {
        bool val = ::genie::backend::get_arr_element(scm_mask, i);
        if (val == current_val) {
            current_count++;
        } else {
            rleData.rl_entries.push_back(current_count);
            current_val = val;
            current_count = 1;
        }
    }
    rleData.rl_entries.push_back(current_count);

    if (!rleData.rl_entries.empty()) {
        rleData.maxCount = *std::max_element(rleData.rl_entries.begin(), rleData.rl_entries.end());
    } else {
        rleData.maxCount = 0;
    }

    if (rleData.maxCount <= 255) {
        rleData.transformID = TransformID::ID_1;
    } else if (rleData.maxCount <= 65535) {
        rleData.transformID = TransformID::ID_2;
    } else {
        rleData.transformID = TransformID::ID_3;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void compute_masks(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    size_t nrows,
    size_t ncols,
    bool is_intra_scm,
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
) {
    UTILS_DIE_IF(::genie::backend::get_arr_size(row_ids) != ::genie::backend::get_arr_size(col_ids),
                 "The size of row_ids and col_ids must be same!");

    if (is_intra_scm) {
        UTILS_DIE_IF(nrows != ncols, "For intra-SCM, nrows and ncols must be equal!");
        ::genie::backend::resize_arr(row_mask, nrows);
        ::genie::backend::assign_arr(row_mask, nrows, false);
        
        size_t n = ::genie::backend::get_arr_size(row_ids);
        for (size_t i = 0; i < n; ++i) {
            uint64_t r = ::genie::backend::get_arr_element(row_ids, i);
            uint64_t c = ::genie::backend::get_arr_element(col_ids, i);
            if (r < (uint64_t)nrows) ::genie::backend::set_arr_element(row_mask, (size_t)r, true);
            if (c < (uint64_t)ncols) ::genie::backend::set_arr_element(row_mask, (size_t)c, true);
        }
        col_mask = row_mask;
    } else {
        ::genie::backend::compute_mask(row_ids, nrows, row_mask);
        ::genie::backend::compute_mask(col_ids, ncols, col_mask);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_scm_masks(
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    const SubcontactMatrixPayload& scm_payload,
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
) {
    auto row_nentries = cm_param.GetNumBinEntries(scm_param.GetChr1ID());
    auto col_nentries = cm_param.GetNumBinEntries(scm_param.GetChr2ID());

    if (scm_param.GetRowMaskExistsFlag()) {
        decode_scm_mask_payload(scm_payload.GetRowMaskPayload(), row_nentries, row_mask);
    } else {
        ::genie::backend::resize_arr(row_mask, row_nentries);
        ::genie::backend::assign_arr(row_mask, row_nentries, true);
    }

    if (scm_param.IsIntraSCM()) {
        col_mask = row_mask;
    } else if (scm_param.GetColMaskExistsFlag()) {
        decode_scm_mask_payload(scm_payload.GetColMaskPayload(), col_nentries, col_mask);
    } else {
        ::genie::backend::resize_arr(col_mask, col_nentries);
        ::genie::backend::assign_arr(col_mask, col_nentries, true);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_scm_mask_payload(
    const SubcontactMatrixMaskPayload& mask_payload,
    size_t num_entries,
    BinVecDtype& mask
) {
    auto transform_ID = mask_payload.GetTransformID();
    if (transform_ID == TransformID::ID_0) {
        auto mask_array = mask_payload.GetMaskArray();
        ::genie::backend::resize_arr(mask, mask_array.size());
        for(size_t i=0; i<mask_array.size(); ++i) ::genie::backend::set_arr_element(mask, i, mask_array[i]);
        return;
    }

    ::genie::backend::resize_arr(mask, num_entries);
    ::genie::backend::assign_arr(mask, num_entries, false);
    
    bool current_val = mask_payload.GetFirstVal();
    const auto& rl_entries = mask_payload.GetRlEntries();

    size_t start_idx = 0;
    for (uint32_t rl_entry : rl_entries) {
        size_t end_idx = start_idx + rl_entry;
        for(size_t i = start_idx; i < std::min(end_idx, num_entries); ++i) {
            ::genie::backend::set_arr_element(mask, i, current_val);
        }
        start_idx = end_idx;
        current_val = !current_val;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void remove_unaligned(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts,
    bool is_intra_tile,
    const BinVecDtype& row_mask,
    const BinVecDtype& col_mask,
    size_t row_offset,
    size_t row_size,
    size_t col_offset,
    size_t col_size,
    size_t& row_aligned_count,
    size_t& col_aligned_count
) {
    size_t n = ::genie::backend::get_arr_size(row_ids);
    size_t nrows_mask = ::genie::backend::get_arr_size(row_mask);
    size_t ncols_mask = ::genie::backend::get_arr_size(col_mask);

    std::vector<uint64_t> row_map(row_size, 0);
    row_aligned_count = 0;
    for (size_t i = 0; i < row_size; ++i) {
        size_t global_idx = row_offset + i;
        if (global_idx < nrows_mask && ::genie::backend::get_arr_element(row_mask, global_idx)) {
            row_map[i] = row_aligned_count++;
        }
    }

    std::vector<uint64_t> col_map(col_size, 0);
    col_aligned_count = 0;
    for (size_t i = 0; i < col_size; ++i) {
        size_t global_idx = col_offset + i;
        if (global_idx < ncols_mask && ::genie::backend::get_arr_element(col_mask, global_idx)) {
            col_map[i] = col_aligned_count++;
        }
    }

    std::vector<uint64_t> tmp_rows, tmp_cols;
    std::vector<uint32_t> tmp_counts;
    
    for (size_t i = 0; i < n; ++i) {
        uint64_t r_tile = ::genie::backend::get_arr_element(row_ids, i);
        uint64_t c_tile = ::genie::backend::get_arr_element(col_ids, i);
        
        if (r_tile < row_size && c_tile < col_size) {
            size_t global_r = row_offset + (size_t)r_tile;
            size_t global_c = col_offset + (size_t)c_tile;
            
            bool r_aligned = (global_r < nrows_mask) && ::genie::backend::get_arr_element(row_mask, global_r);
            bool c_aligned = (global_c < ncols_mask) && ::genie::backend::get_arr_element(col_mask, global_c);
            
            if (r_aligned && c_aligned) {
                tmp_rows.push_back(row_map[(size_t)r_tile]);
                tmp_cols.push_back(col_map[(size_t)c_tile]);
                tmp_counts.push_back(::genie::backend::get_arr_element(counts, i));
            }
        }
    }
    
    ::genie::backend::resize_arr(row_ids, tmp_rows.size());
    ::genie::backend::resize_arr(col_ids, tmp_cols.size());
    ::genie::backend::resize_arr(counts, tmp_counts.size());
    for (size_t i = 0; i < tmp_rows.size(); ++i) {
        ::genie::backend::set_arr_element(row_ids, i, tmp_rows[i]);
        ::genie::backend::set_arr_element(col_ids, i, tmp_cols[i]);
        ::genie::backend::set_arr_element(counts, i, tmp_counts[i]);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void insert_unaligned(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    bool is_intra_tile,
    const BinVecDtype& row_mask,
    const BinVecDtype& col_mask,
    size_t row_offset,
    size_t row_size,
    size_t col_offset,
    size_t col_size
) {
    size_t n = ::genie::backend::get_arr_size(row_ids);
    size_t nrows_mask = ::genie::backend::get_arr_size(row_mask);
    size_t ncols_mask = ::genie::backend::get_arr_size(col_mask);

    std::vector<uint64_t> row_inv_map;
    for (size_t i = 0; i < row_size; ++i) {
        size_t global_idx = row_offset + i;
        if (global_idx < nrows_mask && ::genie::backend::get_arr_element(row_mask, global_idx)) {
            row_inv_map.push_back(i);
        }
    }

    std::vector<uint64_t> col_inv_map;
    for (size_t i = 0; i < col_size; ++i) {
        size_t global_idx = col_offset + i;
        if (global_idx < ncols_mask && ::genie::backend::get_arr_element(col_mask, global_idx)) {
            col_inv_map.push_back(i);
        }
    }

    for (size_t i = 0; i < n; ++i) {
        uint64_t r_compact = ::genie::backend::get_arr_element(row_ids, i);
        uint64_t c_compact = ::genie::backend::get_arr_element(col_ids, i);
        
        if (r_compact < row_inv_map.size()) {
            ::genie::backend::set_arr_element(row_ids, i, row_inv_map[(size_t)r_compact]);
        }
        if (c_compact < col_inv_map.size()) {
            ::genie::backend::set_arr_element(col_ids, i, col_inv_map[(size_t)c_compact]);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void sparse_to_dense(
    const UInt64VecDtype& row_ids,
    const UInt64VecDtype& col_ids,
    const UIntVecDtype& counts,
    size_t nrows,
    size_t ncols,
    UIntMatDtype& mat
) {
    ::genie::backend::resize_mat(mat, std::vector<size_t>{nrows, ncols});
    for(size_t i=0; i<nrows; ++i) for(size_t j=0; j<ncols; ++j) ::genie::backend::set_mat_element(mat, i, j, 0u);

    size_t n = ::genie::backend::get_arr_size(row_ids);
    for (size_t i = 0; i < n; ++i) {
        uint64_t r = ::genie::backend::get_arr_element(row_ids, i);
        uint64_t c = ::genie::backend::get_arr_element(col_ids, i);
        uint32_t v = ::genie::backend::get_arr_element(counts, i);
        if (r < (uint64_t)nrows && c < (uint64_t)ncols) {
            uint32_t current_val = ::genie::backend::get_mat_element(mat, (size_t)r, (size_t)c);
            ::genie::backend::set_mat_element(mat, (size_t)r, (size_t)c, current_val + v);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void dense_to_sparse(
    const UIntMatDtype& mat,
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts
) {
    size_t nrows = ::genie::backend::get_mat_shape(mat, 0);
    size_t ncols = ::genie::backend::get_mat_shape(mat, 1);
    
    std::vector<uint64_t> tmp_rows, tmp_cols;
    std::vector<uint32_t> tmp_counts;
    
    for (size_t i = 0; i < nrows; ++i) {
        for (size_t j = 0; j < ncols; ++j) {
            uint32_t v = ::genie::backend::get_mat_element(mat, i, j);
            if (v > 0) {
                tmp_rows.push_back(i);
                tmp_cols.push_back(j);
                tmp_counts.push_back(v);
            }
        }
    }
    
    ::genie::backend::resize_arr(row_ids, tmp_rows.size());
    ::genie::backend::resize_arr(col_ids, tmp_cols.size());
    ::genie::backend::resize_arr(counts, tmp_counts.size());
    
    for (size_t i = 0; i < tmp_rows.size(); ++i) {
        ::genie::backend::set_arr_element(row_ids, i, tmp_rows[i]);
        ::genie::backend::set_arr_element(col_ids, i, tmp_cols[i]);
        ::genie::backend::set_arr_element(counts, i, tmp_counts[i]);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::vector<int64_t> get_diag_ids(size_t nrows, size_t ncols, DiagonalTransformMode mode) {
    std::vector<int64_t> diag_ids;
    if (mode == DiagonalTransformMode::MODE_1) {
        diag_ids.push_back(0);
        auto ndiags = std::max((int64_t)nrows, (int64_t)ncols);
        for (int64_t diag_id = 1; diag_id < ndiags; diag_id++) {
            if (diag_id < (int64_t)ncols) diag_ids.push_back(diag_id);
            if (diag_id < (int64_t)nrows) diag_ids.push_back(-diag_id);
        }
    } else if (mode == DiagonalTransformMode::MODE_2) {
        for (int64_t i = -(int64_t)nrows + 1; i < (int64_t)ncols; ++i) diag_ids.push_back(i);
    } else if (mode == DiagonalTransformMode::MODE_3) {
        for (int64_t i = (int64_t)ncols - 1; i > -(int64_t)nrows; --i) diag_ids.push_back(i);
    }
    return diag_ids;
}

// ---------------------------------------------------------------------------------------------------------------------

void diag_transform(UIntMatDtype& mat, DiagonalTransformMode mode) {
    if (mode == DiagonalTransformMode::NONE) return;
    size_t nrows = ::genie::backend::get_mat_shape(mat, 0);
    size_t ncols = ::genie::backend::get_mat_shape(mat, 1);
    if (nrows == 0) return;

    UIntMatDtype trans_mat;
    if (mode == DiagonalTransformMode::MODE_0) {
        UTILS_DIE_IF(nrows != ncols, "Matrix must be square for MODE_0!");
        size_t new_nrows = nrows / 2 + 1;
        ::genie::backend::resize_mat(trans_mat, std::vector<size_t>{new_nrows, ncols});
        for(size_t i=0; i<new_nrows; ++i) for(size_t j=0; j<ncols; ++j) ::genie::backend::set_mat_element(trans_mat, i, j, 0u);

        size_t o = 0;
        for (size_t k_diag = 0; k_diag < nrows; ++k_diag) {
            for (size_t i = 0; i < (nrows - k_diag); ++i) {
                size_t j = i + k_diag;
                uint32_t v = ::genie::backend::get_mat_element(mat, i, j);
                size_t target_i = o / ncols;
                size_t target_j = o % ncols;
                if (target_i < new_nrows && target_j < ncols) {
                    ::genie::backend::set_mat_element(trans_mat, target_i, target_j, v);
                }
                o++;
            }
        }
        mat = std::move(trans_mat);
    } else {
        ::genie::backend::resize_mat(trans_mat, std::vector<size_t>{nrows, ncols});
        for(size_t i=0; i<nrows; ++i) for(size_t j=0; j<ncols; ++j) ::genie::backend::set_mat_element(trans_mat, i, j, 0u);
        
        auto diag_ids = get_diag_ids(nrows, ncols, mode);
        size_t o = 0;
        for (auto diag_id : diag_ids) {
            int64_t i_offset = (diag_id >= 0) ? 0 : -diag_id;
            int64_t j_offset = (diag_id >= 0) ? diag_id : 0;
            int64_t nelems = std::max((int64_t)nrows, (int64_t)ncols) - std::abs(diag_id);
            
            for (int64_t k = 0; k < nelems; ++k) {
                int64_t i = k + i_offset;
                int64_t j = k + j_offset;
                if (i >= (int64_t)nrows || j >= (int64_t)ncols) break;
                
                uint32_t v = ::genie::backend::get_mat_element(mat, (size_t)i, (size_t)j);
                size_t target_i = o / ncols;
                size_t target_j = o % ncols;
                if (target_i < nrows && target_j < ncols) {
                    ::genie::backend::set_mat_element(trans_mat, target_i, target_j, v);
                }
                o++;
            }
        }
        mat = std::move(trans_mat);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_diag_transform(UIntMatDtype& mat, DiagonalTransformMode mode) {
    if (mode == DiagonalTransformMode::NONE) return;
    size_t nrows = ::genie::backend::get_mat_shape(mat, 0);
    size_t ncols = ::genie::backend::get_mat_shape(mat, 1);
    if (nrows == 0) return;

    UIntMatDtype orig_mat;
    if (mode == DiagonalTransformMode::MODE_0) {
        size_t n = ncols;
        ::genie::backend::resize_mat(orig_mat, std::vector<size_t>{n, n});
        for(size_t i=0; i<n; ++i) for(size_t j=0; j<n; ++j) ::genie::backend::set_mat_element(orig_mat, i, j, 0u);

        size_t o = 0;
        for (size_t k_diag = 0; k_diag < n; ++k_diag) {
            for (size_t i = 0; i < (n - k_diag); ++i) {
                size_t j = i + k_diag;
                size_t src_i = o / ncols;
                size_t src_j = o % ncols;
                if (src_i < nrows && src_j < ncols) {
                    uint32_t v = ::genie::backend::get_mat_element(mat, src_i, src_j);
                    ::genie::backend::set_mat_element(orig_mat, i, j, v);
                }
                o++;
            }
        }
        mat = std::move(orig_mat);
    } else {
        // MODE_1, 2, 3 logic
        auto diag_ids = get_diag_ids(nrows, ncols, mode);
        ::genie::backend::resize_mat(orig_mat, std::vector<size_t>{nrows, ncols});
        for(size_t i=0; i<nrows; ++i) for(size_t j=0; j<ncols; ++j) ::genie::backend::set_mat_element(orig_mat, i, j, 0u);

        size_t o = 0;
        for (auto diag_id : diag_ids) {
            int64_t i_offset = (diag_id >= 0) ? 0 : -diag_id;
            int64_t j_offset = (diag_id >= 0) ? diag_id : 0;
            int64_t nelems = std::max((int64_t)nrows, (int64_t)ncols) - std::abs(diag_id);
            
            for (int64_t k = 0; k < nelems; ++k) {
                int64_t i = k + i_offset;
                int64_t j = k + j_offset;
                if (i >= (int64_t)nrows || j >= (int64_t)ncols) break;
                
                size_t src_i = o / ncols;
                size_t src_j = o % ncols;
                if (src_i < nrows && src_j < ncols) {
                    uint32_t v = ::genie::backend::get_mat_element(mat, src_i, src_j);
                    ::genie::backend::set_mat_element(orig_mat, (size_t)i, (size_t)j, v);
                }
                o++;
            }
        }
        mat = std::move(orig_mat);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void transform_row_bin(const UIntMatDtype& mat, BinMatDtype& bin_mat) {
    size_t nrows = ::genie::backend::get_mat_shape(mat, 0);
    size_t ncols = ::genie::backend::get_mat_shape(mat, 1);
    if (nrows == 0) return;

    std::vector<uint8_t> nbits_per_row(nrows);
    uint64_t total_bin_rows = 0;
    for (size_t i = 0; i < nrows; ++i) {
        uint32_t row_max = 0;
        for (size_t j = 0; j < ncols; ++j) {
            uint32_t v = ::genie::backend::get_mat_element(mat, i, j);
            if (v > row_max) row_max = v;
        }
        nbits_per_row[i] = (row_max == 0) ? 1 : static_cast<uint8_t>(std::ceil(std::log2(row_max + 1u)));
        total_bin_rows += nbits_per_row[i];
    }

    ::genie::backend::resize_mat(bin_mat, std::vector<size_t>{static_cast<size_t>(total_bin_rows), ncols + 1});
    for(size_t i=0; i<total_bin_rows; ++i) for(size_t j=0; j<ncols+1; ++j) ::genie::backend::set_mat_element(bin_mat, i, j, false);

    size_t current_row = 0;
    for (size_t i = 0; i < nrows; ++i) {
        for (size_t bit = 0; bit < nbits_per_row[i]; ++bit) {
            for (size_t j = 0; j < ncols; ++j) {
                if (::genie::backend::get_mat_element(mat, i, j) & (1u << bit)) {
                    ::genie::backend::set_mat_element(bin_mat, current_row, j + 1, true);
                }
            }
            current_row++;
        }
        if (current_row > 0) ::genie::backend::set_mat_element(bin_mat, current_row - 1, 0, true);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_row_bin(const BinMatDtype& bin_mat, UIntMatDtype& mat) {
    size_t bin_nrows = ::genie::backend::get_mat_shape(bin_mat, 0);
    size_t bin_ncols = ::genie::backend::get_mat_shape(bin_mat, 1);
    if (bin_nrows == 0) return;

    size_t ncols = bin_ncols - 1;
    
    // Count how many rows we have (by counting column 0 == true)
    size_t nrows = 0;
    for (size_t i = 0; i < bin_nrows; ++i) {
        if (::genie::backend::get_mat_element(bin_mat, i, 0)) nrows++;
    }

    ::genie::backend::resize_mat(mat, std::vector<size_t>{nrows, ncols});
    for(size_t i=0; i<nrows; ++i) for(size_t j=0; j<ncols; ++j) ::genie::backend::set_mat_element(mat, i, j, 0u);

    size_t current_row = 0;
    size_t current_bit = 0;
    for (size_t i = 0; i < bin_nrows; ++i) {
        bool is_last_bit = ::genie::backend::get_mat_element(bin_mat, i, 0);
        for (size_t j = 0; j < ncols; ++j) {
            if (::genie::backend::get_mat_element(bin_mat, i, j + 1)) {
                uint32_t v = ::genie::backend::get_mat_element(mat, current_row, j);
                v |= (1u << current_bit);
                ::genie::backend::set_mat_element(mat, current_row, j, v);
            }
        }
        if (is_last_bit) {
            current_row++;
            current_bit = 0;
        } else {
            current_bit++;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void comp_start_end_ids(size_t num_entries, size_t tile_size, size_t tile_idx, size_t& start_idx, size_t& end_idx) {
    start_idx = tile_idx * tile_size;
    end_idx = std::min(start_idx + tile_size, num_entries);
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_cm_tile(const BinMatDtype& bin_mat, core::AlgoID codec_ID, ContactMatrixTilePayload& tile_payload) {
    uint8_t* raw_data = nullptr;
    size_t raw_data_len = 0;
    ::genie::backend::bin_mat_to_bytes(bin_mat, &raw_data, raw_data_len);

    size_t nrows = ::genie::backend::get_mat_shape(bin_mat, 0);
    size_t ncols = ::genie::backend::get_mat_shape(bin_mat, 1);

    uint8_t* compressed_data = nullptr;
    size_t compressed_data_len = 0;

    if (codec_ID == core::AlgoID::JBIG) {
        mpegg_jbig_compress_default(&compressed_data, &compressed_data_len, raw_data, raw_data_len, (unsigned long)nrows, (unsigned long)ncols);
    } else {
        UTILS_DIE("Codec not supported in unified contact coder!");
    }

    std::vector<uint8_t> compressed_vec(compressed_data, compressed_data + compressed_data_len);
    tile_payload = ContactMatrixTilePayload(codec_ID, static_cast<uint32_t>(nrows), static_cast<uint32_t>(ncols), std::move(compressed_vec));
    
    if (raw_data) free(raw_data);
    if (compressed_data) free(compressed_data);
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_cm_tile(const ContactMatrixTilePayload& tile_payload, core::AlgoID codec_ID, BinMatDtype& bin_mat) {
    uint8_t* raw_data = nullptr;
    size_t raw_data_len = 0;
    unsigned long nrows = tile_payload.GetTileNRows();
    unsigned long ncols = tile_payload.GetTileNCols();

    if (codec_ID == core::AlgoID::JBIG) {
        mpegg_jbig_decompress_default(&raw_data, &raw_data_len, tile_payload.GetPayload().data(), tile_payload.GetPayloadSize(), &nrows, &ncols);
    } else {
        UTILS_DIE("Codec not supported in unified contact coder!");
    }

    ::genie::backend::bin_mat_from_bytes(raw_data, raw_data_len, (size_t)nrows, (size_t)ncols, bin_mat);
    if (raw_data) free(raw_data);
}

// ---------------------------------------------------------------------------------------------------------------------

void conv_noop_on_sparse_mat(UInt64VecDtype& tile_row_ids, UInt64VecDtype& tile_col_ids, UIntVecDtype& tile_counts, uint32_t bin_size_mult, bool sort_output) {
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_scm(ContactMatrixParameters& cm_param, core::record::ContactRecord& rec, SubcontactMatrixParameters& scm_param, SubcontactMatrixPayload& scm_payload, bool remove_unaligned_region, bool transform_mask, bool ena_diag_transform, bool ena_binarization, bool norm_as_weight, bool multiplicative_norm, core::AlgoID codec_ID) {
    UTILS_DIE_IF(!ena_binarization, "Binarization must be activated!");

    BinVecDtype row_mask;
    BinVecDtype col_mask;

    auto interval = cm_param.GetBinSize();
    auto tile_size = cm_param.GetTileSize();
    auto chr1_ID = rec.GetChr1ID();
    auto chr1_num_bin_entries = cm_param.GetNumBinEntries(chr1_ID);
    auto ntiles_in_row = cm_param.GetNumTiles(chr1_ID);
    auto chr2_ID = rec.GetChr2ID();
    auto chr2_num_bin_entries = cm_param.GetNumBinEntries(chr2_ID);
    auto ntiles_in_col = cm_param.GetNumTiles(chr2_ID);


    cm_param.UpsertSample(rec.GetSampleID(), rec.GetSampleName());
    scm_payload.SetSampleID(rec.GetSampleID());

    scm_param.SetChr1ID(chr1_ID);
    scm_payload.SetChr1ID(chr1_ID);
    scm_param.SetChr2ID(chr2_ID);
    scm_payload.SetChr2ID(chr2_ID);

    auto is_intra_scm = scm_param.IsIntraSCM();
    scm_param.SetCodecID(codec_ID);
    scm_param.SetNumTiles(ntiles_in_row, ntiles_in_col);
    scm_payload.SetNumTiles(ntiles_in_row, ntiles_in_col);

    UInt64VecDtype row_ids, col_ids;
    UIntVecDtype counts;
    
    auto rec_row_ids = rec.GetStartPos1();
    auto rec_col_ids = rec.GetStartPos2();
    auto rec_counts = rec.GetCounts();
    
    ::genie::backend::resize_arr(row_ids, rec_row_ids.size());
    ::genie::backend::resize_arr(col_ids, rec_col_ids.size());
    ::genie::backend::resize_arr(counts, rec_counts.size());
    
    for(size_t k=0; k<rec_row_ids.size(); ++k) {
        uint64_t r = rec_row_ids[k] / interval;
        uint64_t c = rec_col_ids[k] / interval;
        if (is_intra_scm && r > c) std::swap(r, c);
        ::genie::backend::set_arr_element(row_ids, k, r);
        ::genie::backend::set_arr_element(col_ids, k, c);
        ::genie::backend::set_arr_element(counts, k, rec_counts[k]);
    }

    if (remove_unaligned_region) {
        compute_masks(row_ids, col_ids, chr1_num_bin_entries, chr2_num_bin_entries, is_intra_scm, row_mask, col_mask);
        if (transform_mask) {
            RunLengthEncodingData rowRLE;
            set_rle_information_from_mask(rowRLE, row_mask);
            scm_payload.SetRowMaskPayload(SubcontactMatrixMaskPayload(rowRLE.transformID, rowRLE.firstVal, rowRLE.rl_entries));
            scm_param.SetRowMaskExistsFlag(true);
            if (!is_intra_scm) {
                RunLengthEncodingData colRLE;
                set_rle_information_from_mask(colRLE, col_mask);
                scm_payload.SetColMaskPayload(SubcontactMatrixMaskPayload(colRLE.transformID, colRLE.firstVal, colRLE.rl_entries));
                scm_param.SetColMaskExistsFlag(true);
            } else {
                scm_param.SetColMaskExistsFlag(false);
            }
        } else {
            auto row_mask_copy = row_mask;
            scm_payload.SetRowMaskPayload(SubcontactMatrixMaskPayload(std::move(row_mask_copy)));
            scm_param.SetRowMaskExistsFlag(true);
            if (!is_intra_scm) {
                auto col_mask_copy = col_mask;
                scm_payload.SetColMaskPayload(SubcontactMatrixMaskPayload(std::move(col_mask_copy)));
                scm_param.SetColMaskExistsFlag(true);
            }
        }
    }

    size_t total_entries_in_tiles = 0;
    const size_t n = ::genie::backend::get_arr_size(row_ids);

    std::unordered_map<std::pair<size_t, size_t>, size_t, detail::PairHash> tile_counts;

    for(size_t i=0; i<n; ++i) {
        uint64_t r = ::genie::backend::get_arr_element(row_ids, i);
        uint64_t c = ::genie::backend::get_arr_element(col_ids, i);

        if (r >= chr1_num_bin_entries || c >= chr2_num_bin_entries) continue;
        if (is_intra_scm && r > c) continue;

        size_t i_tile = r / tile_size;
        size_t j_tile = c / tile_size;
        ++tile_counts[{i_tile, j_tile}];
    }

    std::unordered_map<std::pair<size_t, size_t>, std::vector<uint64_t>, detail::PairHash> tile_rows;
    std::unordered_map<std::pair<size_t, size_t>, std::vector<uint64_t>, detail::PairHash> tile_cols;
    std::unordered_map<std::pair<size_t, size_t>, std::vector<uint32_t>, detail::PairHash> tile_counts_data;

    for (auto& [key, count] : tile_counts) {
        tile_rows[key].reserve(count);
        tile_cols[key].reserve(count);
        tile_counts_data[key].reserve(count);
    }

    for(size_t i=0; i<n; ++i) {
        uint64_t r = ::genie::backend::get_arr_element(row_ids, i);
        uint64_t c = ::genie::backend::get_arr_element(col_ids, i);

        if (r >= chr1_num_bin_entries || c >= chr2_num_bin_entries) continue;
        if (is_intra_scm && r > c) continue;

        size_t i_tile = r / tile_size;
        size_t j_tile = c / tile_size;
        auto key = std::make_pair(i_tile, j_tile);
        tile_rows[key].push_back(r - i_tile * tile_size);
        tile_cols[key].push_back(c - j_tile * tile_size);
        tile_counts_data[key].push_back(::genie::backend::get_arr_element(counts, i));
    }

    for (size_t i_tile = 0u; i_tile < ntiles_in_row; i_tile++) {
        for (size_t j_tile = 0u; j_tile < ntiles_in_col; j_tile++) {
            if (i_tile > j_tile && is_intra_scm) continue;

            auto key = std::make_pair(i_tile, j_tile);
            auto it = tile_counts.find(key);
            if (it == tile_counts.end()) {
                scm_payload.SetTilePayload(i_tile, j_tile, ContactMatrixTilePayload(codec_ID, 0, 0, std::vector<uint8_t>{}));
                scm_param.SetTileParameter(i_tile, j_tile, {DiagonalTransformMode::NONE, BinarizationMode::ROW_BINARIZATION});
                continue;
            }

            const size_t tile_count = it->second;
            total_entries_in_tiles += tile_count;

            UInt64VecDtype t_row_ids, t_col_ids;
            UIntVecDtype t_counts;
            detail::assign_vec_to_arr(t_row_ids, tile_rows[key]);
            detail::assign_vec_to_arr(t_col_ids, tile_cols[key]);
            detail::assign_vec_to_arr(t_counts, tile_counts_data[key]);

            size_t row_count = tile_size;
            size_t col_count = tile_size;
            if (remove_unaligned_region) {
                remove_unaligned(t_row_ids, t_col_ids, t_counts, is_intra_scm, row_mask, col_mask, i_tile * tile_size, tile_size, j_tile * tile_size, tile_size, row_count, col_count);
            }

            if (::genie::backend::get_arr_size(t_counts) == 0) {
                scm_payload.SetTilePayload(i_tile, j_tile, ContactMatrixTilePayload(codec_ID, 0, 0, std::vector<uint8_t>{}));
                scm_param.SetTileParameter(i_tile, j_tile, {DiagonalTransformMode::NONE, BinarizationMode::ROW_BINARIZATION});
                continue;
            }

            DiagonalTransformMode mode = DiagonalTransformMode::NONE;
            if (ena_diag_transform && is_intra_scm && i_tile == j_tile) {
                mode = DiagonalTransformMode::MODE_0;
            }

            UIntMatDtype tile_mat;
            sparse_to_dense(t_row_ids, t_col_ids, t_counts, row_count, col_count, tile_mat);
            if (mode != DiagonalTransformMode::NONE) diag_transform(tile_mat, mode);

            BinMatDtype bin_mat;
            transform_row_bin(tile_mat, bin_mat);

            ContactMatrixTilePayload tile_pay;
            encode_cm_tile(bin_mat, codec_ID, tile_pay);
            scm_payload.SetTilePayload(i_tile, j_tile, std::move(tile_pay));
            scm_param.SetTileParameter(i_tile, j_tile, {mode, BinarizationMode::ROW_BINARIZATION});
        }
    }

}

void decode_scm(ContactMatrixParameters& cm_param, SubcontactMatrixParameters& scm_param, SubcontactMatrixPayload& scm_payload, core::record::ContactRecord& rec, uint32_t bin_size_mult) {
    BinVecDtype row_mask, col_mask;
    decode_scm_masks(cm_param, scm_param, scm_payload, row_mask, col_mask);

    auto chr1_ID = scm_payload.GetChr1ID();
    auto chr2_ID = scm_payload.GetChr2ID();
    auto tile_size = cm_param.GetTileSize();
    auto bin_size = cm_param.GetBinSize();
    auto target_bin_size = bin_size * bin_size_mult;

    auto ntiles_in_row = scm_param.GetNTilesInRow();
    auto ntiles_in_col = scm_param.GetNTilesInCol();

    std::unordered_map<std::pair<uint64_t, uint64_t>, uint32_t, detail::PairHash> merged_entries;

    for (size_t i = 0; i < ntiles_in_row; i++) {
        for (size_t j = 0; j < ntiles_in_col; j++) {
            if (i > j && scm_param.IsIntraSCM()) continue;

            auto& tile_payload = scm_payload.GetTilePayload(i, j);
            if (tile_payload.GetPayloadSize() == 0) continue;

            BinMatDtype bin_mat;
            decode_cm_tile(tile_payload, scm_param.GetCodecID(), bin_mat);

            UIntMatDtype tile_mat;
            inverse_transform_row_bin(bin_mat, tile_mat);

            auto tile_param = scm_param.GetTileParameter(i, j);
            if (tile_param.diag_tranform_mode != DiagonalTransformMode::NONE) {
                inverse_diag_transform(tile_mat, tile_param.diag_tranform_mode);
            }

            UInt64VecDtype t_row_ids, t_col_ids;
            UIntVecDtype t_counts;
            dense_to_sparse(tile_mat, t_row_ids, t_col_ids, t_counts);
            insert_unaligned(t_row_ids, t_col_ids, scm_param.IsIntraSCM(), row_mask, col_mask, i * tile_size, tile_size, j * tile_size, tile_size);

            size_t n = ::genie::backend::get_arr_size(t_row_ids);
            for (size_t k = 0; k < n; ++k) {
                uint64_t r = (::genie::backend::get_arr_element(t_row_ids, k) + i * tile_size);
                uint64_t c = (::genie::backend::get_arr_element(t_col_ids, k) + j * tile_size);
                uint32_t v = ::genie::backend::get_arr_element(t_counts, k);
                
                uint64_t lr_r = (r / bin_size_mult) * target_bin_size;
                uint64_t lr_c = (c / bin_size_mult) * target_bin_size;
                
                merged_entries[{lr_r, lr_c}] += v;
            }
        }
    }

    std::vector<uint64_t> all_row_ids, all_end1, all_col_ids, all_end2;
    std::vector<uint32_t> all_counts;
    all_row_ids.reserve(merged_entries.size());
    all_end1.reserve(merged_entries.size());
    all_col_ids.reserve(merged_entries.size());
    all_end2.reserve(merged_entries.size());
    all_counts.reserve(merged_entries.size());

    uint64_t chr1_len = cm_param.GetChromosomeLength(chr1_ID);
    uint64_t chr2_len = cm_param.GetChromosomeLength(chr2_ID);

    for (auto const& [pos, count] : merged_entries) {
        all_row_ids.push_back(pos.first);
        all_end1.push_back(std::min(pos.first + target_bin_size, chr1_len));
        all_col_ids.push_back(pos.second);
        all_end2.push_back(std::min(pos.second + target_bin_size, chr2_len));
        all_counts.push_back(count);
    }

    rec.SetChr1ID(chr1_ID);
    rec.SetChr2ID(chr2_ID);
    rec.SetBinSize(target_bin_size);
    rec.SetCMValues(std::move(all_row_ids), std::move(all_end1), std::move(all_col_ids), std::move(all_end2), std::move(all_counts));
}

} // namespace genie::contact
