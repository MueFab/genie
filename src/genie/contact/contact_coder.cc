/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "contact_coder.h"
#include "contact_types.h"
#include <codecs/include/mpegg-codecs.h>
#include <genie/core/contact_record/record.h>
#include <genie/util/runtime_exception.h>
#include <cstdint>
#include <cstring>
#include <map>
#include <algorithm>
#include <cmath>
#include <numeric>

#include "contact_matrix_parameters.h"
#include "contact_matrix_tile_payload.h"
#include "subcontact_matrix_parameters.h"
#include "subcontact_matrix_mask_payload.h"

namespace genie::contact {

// ---------------------------------------------------------------------------------------------------------------------

void set_rle_information_from_mask(
    RunLengthEncodingData& rleData,
    const BinVecDtype& scm_mask
) {
    auto nelems = ::genie::backend::get_arr_size(scm_mask);
    if (nelems == 0) {
        rleData.maxCount = 0;
        rleData.transformID = TransformID::ID_0;
        return;
    }

    rleData.firstVal = ::genie::backend::get_arr_element(scm_mask, 0);
    ::genie::backend::clear_arr(rleData.rl_entries);

    uint32_t current_count = 0;
    bool current_val = rleData.firstVal;

    for (size_t idx_i = 0; idx_i < nelems; ++idx_i) {
        bool val = ::genie::backend::get_arr_element(scm_mask, idx_i);
        if (val == current_val) {
            current_count++;
        } else {
            ::genie::backend::append_arr_element(rleData.rl_entries, current_count);
            current_val = val;
            current_count = 1;
        }
    }
    ::genie::backend::append_arr_element(rleData.rl_entries, current_count);

    uint32_t max_c = 0;
    auto nrl = ::genie::backend::get_arr_size(rleData.rl_entries);
    for (size_t idx_i = 0; idx_i < nrl; ++idx_i) {
        max_c = std::max(max_c, ::genie::backend::get_arr_element(rleData.rl_entries, idx_i));
    }
    rleData.maxCount = max_c;

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
    const bool is_intra_scm,
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
){
    auto nids = ::genie::backend::get_arr_size(row_ids);
    UTILS_DIE_IF(nids != ::genie::backend::get_arr_size(col_ids),
                 "The size of row_ids and col_ids must be same!");

    if (is_intra_scm){
        UTILS_DIE_IF(nrows != ncols,
            "Both nentries must be the same for intra SCM!"
        );

        UInt64VecDtype combined_ids;
        ::genie::backend::resize_arr(combined_ids, nids * 2);
        for (size_t idx_i = 0; idx_i < nids; ++idx_i) {
            ::genie::backend::set_arr_element(combined_ids, idx_i, ::genie::backend::get_arr_element(row_ids, idx_i));
            ::genie::backend::set_arr_element(combined_ids, idx_i + nids, ::genie::backend::get_arr_element(col_ids, idx_i));
        }

        ::genie::backend::compute_mask(combined_ids, nrows, row_mask);
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
){
    auto row_nentries = cm_param.GetNumBinEntries(scm_param.GetChr1ID());
    auto col_nentries = cm_param.GetNumBinEntries(scm_param.GetChr2ID());

    if (scm_param.GetRowMaskExistsFlag()){
        decode_scm_mask_payload(scm_payload.GetRowMaskPayload(), row_nentries, row_mask);
    } else {
        ::genie::backend::assign_arr(row_mask, row_nentries, true);
    }

    if (scm_param.IsIntraSCM()){
        col_mask = row_mask;
    } else if (scm_param.GetColMaskExistsFlag()){
        decode_scm_mask_payload(scm_payload.GetColMaskPayload(), col_nentries, col_mask);
    } else {
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
    if (transform_ID == TransformID::ID_0){
        const auto& m_vec = mask_payload.GetMaskArray();
        ::genie::backend::resize_arr(mask, m_vec.size());
        for (size_t idx_i = 0; idx_i < m_vec.size(); ++idx_i) {
            ::genie::backend::set_arr_element(mask, idx_i, m_vec[idx_i]);
        }
        UTILS_DIE_IF(
            num_entries != ::genie::backend::get_arr_size(mask),
            "num_entries and the size of mask_array_ differ!"
        );
    } else {
        ::genie::backend::assign_arr(mask, num_entries, false);

        bool first_val = mask_payload.GetFirstVal();
        const auto& rl_entries = mask_payload.GetRlEntries();
        auto nrl = ::genie::backend::get_arr_size(rl_entries);

        size_t start_idx = 0;
        for (size_t idx_i = 0; idx_i < nrl; ++idx_i){
            uint32_t rl_entry = ::genie::backend::get_arr_element(rl_entries, idx_i);
            size_t end_idx = start_idx + rl_entry;
            ::genie::backend::fill_arr(mask, start_idx, std::min(end_idx, num_entries), first_val);
            start_idx = end_idx;
            first_val = !first_val;
        }
        UTILS_DIE_IF(
            start_idx > num_entries,
            "start_idx value must be smaller than num_entries!"
        );
        if (start_idx < num_entries) {
            ::genie::backend::fill_arr(mask, start_idx, num_entries, first_val);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void remove_unaligned(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    bool is_intra_tile,
    const BinVecDtype& row_mask,
    const BinVecDtype& col_mask
){
    auto nids = ::genie::backend::get_arr_size(row_ids);
    UTILS_DIE_IF(nids != ::genie::backend::get_arr_size(col_ids),
                 "The size of row_ids and col_ids must be same!");

    if (is_intra_tile){
        auto nmask = ::genie::backend::get_arr_size(row_mask);
        std::vector<uint64_t> mapping(nmask);
        uint64_t new_id = 0u;
        for (size_t idx_i = 0u; idx_i < nmask; idx_i++){
            mapping[idx_i] = new_id;
            if (::genie::backend::get_arr_element(row_mask, idx_i)) {
                new_id++;
            }
        }

        for (size_t idx_i = 0u; idx_i < nids; idx_i++){
            ::genie::backend::set_arr_element(row_ids, idx_i, mapping[::genie::backend::get_arr_element(row_ids, idx_i)]);
            ::genie::backend::set_arr_element(col_ids, idx_i, mapping[::genie::backend::get_arr_element(col_ids, idx_i)]);
        }
    } else {
        auto row_mapping_len = ::genie::backend::get_arr_size(row_mask);
        std::vector<uint64_t> row_mapping(row_mapping_len);
        {
            uint64_t new_id = 0u;
            for (size_t idx_i = 0u; idx_i < row_mapping_len; idx_i++){
                row_mapping[idx_i] = new_id;
                if (::genie::backend::get_arr_element(row_mask, idx_i)) {
                    new_id++;
                }
            }
        }

        auto col_mapping_len = ::genie::backend::get_arr_size(col_mask);
        std::vector<uint64_t> col_mapping(col_mapping_len);
        {
            uint64_t new_id = 0u;
            for (size_t idx_i = 0u; idx_i < col_mapping_len; idx_i++){
                col_mapping[idx_i] = new_id;
                if (::genie::backend::get_arr_element(col_mask, idx_i)) {
                    new_id++;
                }
            }
        }

        for (size_t idx_i = 0u; idx_i < nids; idx_i++){
            ::genie::backend::set_arr_element(row_ids, idx_i, row_mapping[::genie::backend::get_arr_element(row_ids, idx_i)]);
            ::genie::backend::set_arr_element(col_ids, idx_i, col_mapping[::genie::backend::get_arr_element(col_ids, idx_i)]);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void insert_unaligned(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    bool is_intra_tile,
    const BinVecDtype& row_mask,
    const BinVecDtype& col_mask
){
    auto nids = ::genie::backend::get_arr_size(row_ids);
    UTILS_DIE_IF(nids != ::genie::backend::get_arr_size(col_ids),
                 "The size of row_ids and col_ids must be same!");

    if (is_intra_tile){
        auto nmask = ::genie::backend::get_arr_size(row_mask);
        std::vector<uint64_t> mapping;
        for(size_t idx_i=0; idx_i<nmask; ++idx_i) {
            if(::genie::backend::get_arr_element(row_mask, idx_i)) {
                mapping.push_back(idx_i);
            }
        }

        for (size_t idx_i = 0u; idx_i < nids; idx_i++){
            ::genie::backend::set_arr_element(row_ids, idx_i, mapping[::genie::backend::get_arr_element(row_ids, idx_i)]);
            ::genie::backend::set_arr_element(col_ids, idx_i, mapping[::genie::backend::get_arr_element(col_ids, idx_i)]);
        }

    } else {
        auto nrow_mask = ::genie::backend::get_arr_size(row_mask);
        std::vector<uint64_t> row_mapping;
        for(size_t idx_i=0; idx_i<nrow_mask; ++idx_i) {
            if(::genie::backend::get_arr_element(row_mask, idx_i)) {
                row_mapping.push_back(idx_i);
            }
        }
        for (size_t idx_i = 0u; idx_i < nids; idx_i++){
            ::genie::backend::set_arr_element(row_ids, idx_i, row_mapping[::genie::backend::get_arr_element(row_ids, idx_i)]);
        }

        auto ncol_mask = ::genie::backend::get_arr_size(col_mask);
        std::vector<uint64_t> col_mapping;
        for(size_t idx_i=0; idx_i<ncol_mask; ++idx_i) {
            if(::genie::backend::get_arr_element(col_mask, idx_i)) {
                col_mapping.push_back(idx_i);
            }
        }
        for (size_t idx_i = 0u; idx_i < nids; idx_i++){
            ::genie::backend::set_arr_element(col_ids, idx_i, col_mapping[::genie::backend::get_arr_element(col_ids, idx_i)]);
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
){
    auto ncounts = ::genie::backend::get_arr_size(counts);
    ::genie::backend::resize_mat(mat, MatShapeDtype{nrows, ncols});
    for(size_t idx_i=0; idx_i<nrows; ++idx_i) for(size_t idx_j=0; idx_j<ncols; ++idx_j) ::genie::backend::set_mat_element(mat, idx_i, idx_j, (uint32_t)0);

    for (size_t idx_i = 0u; idx_i < ncounts; idx_i++){
        auto r = ::genie::backend::get_arr_element(row_ids, idx_i);
        auto c = ::genie::backend::get_arr_element(col_ids, idx_i);
        auto v = ::genie::backend::get_arr_element(counts, idx_i);
        ::genie::backend::set_mat_element(mat, r, c, v);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void dense_to_sparse(
    const UIntMatDtype& mat,
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts
){
    ::genie::backend::clear_arr(row_ids);
    ::genie::backend::clear_arr(col_ids);
    ::genie::backend::clear_arr(counts);

    auto nrows = ::genie::backend::get_mat_shape(mat, 0);
    auto ncols = ::genie::backend::get_mat_shape(mat, 1);

    for (size_t idx_i = 0; idx_i < nrows; ++idx_i) {
        for (size_t idx_j = 0; idx_j < ncols; ++idx_j) {
            auto v = ::genie::backend::get_mat_element(mat, idx_i, idx_j);
            if (v > 0) {
                ::genie::backend::append_arr_element(row_ids, (uint64_t)idx_i);
                ::genie::backend::append_arr_element(col_ids, (uint64_t)idx_j);
                ::genie::backend::append_arr_element(counts, (uint32_t)v);
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_diag_transform(
    UIntMatDtype& mat,
    DiagonalTransformMode mode
){
    if (mode == DiagonalTransformMode::NONE) {
        return;
    }

    auto nrows = ::genie::backend::get_mat_shape(mat, 0);
    if (nrows == 0) return;
    auto ncols = ::genie::backend::get_mat_shape(mat, 1);

    UIntMatDtype trans_mat;

    if (mode == DiagonalTransformMode::MODE_0) {
        auto source_ncols = ncols;
        auto target_nrows = source_ncols;
        auto source_nrows = nrows;

        ::genie::backend::resize_mat(trans_mat, MatShapeDtype{target_nrows, target_nrows});
        for(size_t idx_i=0; idx_i<target_nrows; ++idx_i) for(size_t idx_j=0; idx_j<target_nrows; ++idx_j) ::genie::backend::set_mat_element(trans_mat, idx_i, idx_j, (uint32_t)0);

        size_t o = 0u;
        for (int64_t k_diag = 0; k_diag < static_cast<int64_t>(target_nrows); ++k_diag) {
            for (int64_t i_target = 0; i_target < static_cast<int64_t>(target_nrows - k_diag); ++i_target) {
                int64_t j_target = i_target + k_diag;

                size_t src_i = o / source_ncols;
                size_t src_j = o % source_ncols;

                if (src_i < source_nrows) {
                    ::genie::backend::set_mat_element(trans_mat, i_target, j_target, ::genie::backend::get_mat_element(mat, src_i, src_j));
                }
                o++;
            }
        }
        mat = std::move(trans_mat);
    } else {
        ::genie::backend::resize_mat(trans_mat, MatShapeDtype{nrows, ncols});
        for(size_t idx_i=0; idx_i<nrows; ++idx_i) for(size_t idx_j=0; idx_j<ncols; ++idx_j) ::genie::backend::set_mat_element(trans_mat, idx_i, idx_j, (uint32_t)0);

        std::vector<int64_t> diag_ids;
        if (mode == DiagonalTransformMode::MODE_1){
            diag_ids.push_back(0);
            auto ndiags = std::max(nrows, ncols);
            for (int64_t diag_id = 1; diag_id < static_cast<int64_t>(ndiags); diag_id++){
                if (static_cast<size_t>(diag_id) < ncols) diag_ids.push_back(diag_id);
                if (static_cast<size_t>(diag_id) < nrows) diag_ids.push_back(-diag_id);
            }
        } else if (mode == DiagonalTransformMode::MODE_2){
            for (int64_t diag_id = -static_cast<int64_t>(nrows)+1; diag_id < static_cast<int64_t>(ncols); diag_id++) diag_ids.push_back(diag_id);
        } else if (mode == DiagonalTransformMode::MODE_3){
            for (int64_t diag_id = static_cast<int64_t>(ncols)-1; diag_id > -static_cast<int64_t>(nrows); diag_id--) diag_ids.push_back(diag_id);
        }

        size_t o = 0u;
        for (auto diag_id : diag_ids){
            int64_t i_start = (diag_id >= 0) ? 0 : -diag_id;
            int64_t j_start = (diag_id >= 0) ? diag_id : 0;
            auto nelems_in_diag = std::min(static_cast<int64_t>(nrows) - i_start, static_cast<int64_t>(ncols) - j_start);

            for (int64_t idx_k = 0; idx_k < nelems_in_diag; ++idx_k) {
                size_t i_src = o / ncols;
                size_t j_src = o % ncols;

                if (i_src < nrows) {
                    ::genie::backend::set_mat_element(trans_mat, i_start + idx_k, j_start + idx_k, ::genie::backend::get_mat_element(mat, i_src, j_src));
                }
                o++;
            }
        }
        mat = std::move(trans_mat);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void diag_transform(
    UIntMatDtype& mat,
    DiagonalTransformMode mode
){
    if (mode == DiagonalTransformMode::NONE){
        return;
    }

    auto nrows = ::genie::backend::get_mat_shape(mat, 0);
    if (nrows == 0) return;
    auto ncols = ::genie::backend::get_mat_shape(mat, 1);

    UIntMatDtype trans_mat;

    if (mode == DiagonalTransformMode::MODE_0) {
        auto target_nrows = nrows / 2 + 1;
        auto target_ncols = ncols;
        ::genie::backend::resize_mat(trans_mat, MatShapeDtype{target_nrows, target_ncols});
        for(size_t idx_i=0; idx_i<target_nrows; ++idx_i) for(size_t idx_j=0; idx_j<target_ncols; ++idx_j) ::genie::backend::set_mat_element(trans_mat, idx_i, idx_j, (uint32_t)0);
        size_t o = 0u;
        for (int64_t k_diag = 0; k_diag < static_cast<int64_t>(nrows); ++k_diag) {
            for (int64_t idx_i = 0; idx_i < static_cast<int64_t>(nrows - k_diag); ++idx_i) {
                int64_t idx_j = idx_i + k_diag;
                auto v = ::genie::backend::get_mat_element(mat, idx_i, idx_j);
                size_t target_i = o / target_ncols;
                size_t target_j = o % target_ncols;
                if (target_i < target_nrows) {
                    ::genie::backend::set_mat_element(trans_mat, target_i, target_j, v);
                }
                o++;
            }
        }
        mat = std::move(trans_mat);
    } else {
        ::genie::backend::resize_mat(trans_mat, MatShapeDtype{nrows, ncols});
        for(size_t idx_i=0; idx_i<nrows; ++idx_i) for(size_t idx_j=0; idx_j<ncols; ++idx_j) ::genie::backend::set_mat_element(trans_mat, idx_i, idx_j, (uint32_t)0);

        std::vector<int64_t> diag_ids;
        if (mode == DiagonalTransformMode::MODE_1){
            diag_ids.push_back(0);
            auto ndiags = std::max(nrows, ncols);
            for (int64_t diag_id = 1; diag_id < static_cast<int64_t>(ndiags); diag_id++){
                if (static_cast<size_t>(diag_id) < ncols) diag_ids.push_back(diag_id);
                if (static_cast<size_t>(diag_id) < nrows) diag_ids.push_back(-diag_id);
            }
        } else if (mode == DiagonalTransformMode::MODE_2){
            for (int64_t diag_id = -static_cast<int64_t>(nrows)+1; diag_id < static_cast<int64_t>(ncols); diag_id++) diag_ids.push_back(diag_id);
        } else if (mode == DiagonalTransformMode::MODE_3){
            for (int64_t diag_id = static_cast<int64_t>(ncols)-1; diag_id > -static_cast<int64_t>(nrows); diag_id--) diag_ids.push_back(diag_id);
        }

        size_t o = 0u;
        for (auto diag_id : diag_ids){
            int64_t i_start = (diag_id >= 0) ? 0 : -diag_id;
            int64_t j_start = (diag_id >= 0) ? diag_id : 0;
            auto nelems_in_diag = std::min(static_cast<int64_t>(nrows) - i_start, static_cast<int64_t>(ncols) - j_start);

            for (int64_t idx_k = 0; idx_k < nelems_in_diag; ++idx_k) {
                auto v = ::genie::backend::get_mat_element(mat, i_start + idx_k, j_start + idx_k);
                size_t target_i = o / ncols;
                size_t target_j = o % ncols;

                if (target_i < nrows) {
                    ::genie::backend::set_mat_element(trans_mat, target_i, target_j, v);
                }
                o++;
            }
        }
        mat = std::move(trans_mat);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_row_bin(
    const BinMatDtype& bin_mat,
    UIntMatDtype& mat
){
    size_t bin_mat_nrows = ::genie::backend::get_mat_shape(bin_mat, 0);
    if (bin_mat_nrows == 0) return;
    size_t bin_mat_ncols = ::genie::backend::get_mat_shape(bin_mat, 1);

    UTILS_DIE_IF(bin_mat_nrows == 0, "Invalid bin_mat_nrows!");
    UTILS_DIE_IF(bin_mat_ncols == 0, "Invalid bin_mat_ncols!");

    size_t mat_ncols = bin_mat_ncols - 1;

    size_t mat_nrows = 0;
    for (size_t idx_i = 0; idx_i < bin_mat_nrows; ++idx_i) {
        if (::genie::backend::get_mat_element(bin_mat, idx_i, 0)) {
            mat_nrows++;
        }
    }

    UTILS_DIE_IF(mat_nrows == 0, "Invalid mat_nrows after transformation!");
    UTILS_DIE_IF(mat_ncols == 0, "Invalid mat_ncols after transformation!");

    ::genie::backend::resize_mat(mat, MatShapeDtype{mat_nrows, mat_ncols});
    for(size_t idx_i=0; idx_i<mat_nrows; ++idx_i) for(size_t idx_j=0; idx_j<mat_ncols; ++idx_j) ::genie::backend::set_mat_element(mat, idx_i, idx_j, (uint32_t)0);

    size_t target_i = 0;
    uint8_t bit_pos = 0;

    for (size_t idx_i = 0; idx_i < bin_mat_nrows; ++idx_i){
        for (size_t idx_j = 1; idx_j < bin_mat_ncols; ++idx_j) {
            if (::genie::backend::get_mat_element(bin_mat, idx_i, idx_j)) {
                auto v = ::genie::backend::get_mat_element(mat, target_i, idx_j-1);
                ::genie::backend::set_mat_element(mat, target_i, idx_j-1, v | (1u << bit_pos));
            }
        }

        if (::genie::backend::get_mat_element(bin_mat, idx_i, 0)){
            target_i++;
            bit_pos = 0;
        } else {
            bit_pos++;
        }
    }

    UTILS_DIE_IF(target_i != mat_nrows, "Not all of the mat rows are processed!");
}

// ---------------------------------------------------------------------------------------------------------------------

void transform_row_bin(
    const UIntMatDtype& mat,
    BinMatDtype& bin_mat
) {
    auto nrows = ::genie::backend::get_mat_shape(mat, 0);
    if (nrows == 0) return;
    auto ncols = ::genie::backend::get_mat_shape(mat, 1);

    std::vector<uint8_t> nbits_per_row(nrows);
    for(size_t idx_i=0; idx_i<nrows; ++idx_i) {
        uint32_t max_val = 0;
        for (size_t idx_j = 0; idx_j < ncols; ++idx_j) {
            max_val = std::max(max_val, ::genie::backend::get_mat_element(mat, idx_i, idx_j));
        }
        nbits_per_row[idx_i] = (max_val == 0) ? 1 : static_cast<uint8_t>(std::ceil(std::log2(max_val + 1u)));
        if (nbits_per_row[idx_i] == 0) nbits_per_row[idx_i] = 1;
    }

    uint64_t bin_mat_nrows = std::accumulate(nbits_per_row.begin(), nbits_per_row.end(), 0ULL);
    uint64_t bin_mat_ncols = ncols + 1;

    ::genie::backend::resize_mat(bin_mat, MatShapeDtype{(size_t)bin_mat_nrows, (size_t)bin_mat_ncols});
    // Initialize with false (0)
    for (size_t idx_i = 0; idx_i < bin_mat_nrows; ++idx_i) {
        for (size_t idx_j = 0; idx_j < bin_mat_ncols; ++idx_j) {
            ::genie::backend::set_mat_element(bin_mat, idx_i, idx_j, false);
        }
    }

    size_t current_bin_mat_row = 0;
    for (size_t idx_i = 0; idx_i < nrows; ++idx_i) {
        auto bitlength = nbits_per_row[idx_i];
        for (size_t i_bit = 0; i_bit < bitlength; ++i_bit) {
            for(size_t idx_j=0; idx_j<ncols; ++idx_j) {
                if(::genie::backend::get_mat_element(mat, idx_i, idx_j) & (1u << i_bit)) {
                    ::genie::backend::set_mat_element(bin_mat, current_bin_mat_row, idx_j+1, true);
                }
            }
            current_bin_mat_row++;
        }
        if (current_bin_mat_row > 0) {
            ::genie::backend::set_mat_element(bin_mat, current_bin_mat_row - 1, 0, true);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void comp_start_end_ids(
    size_t num_entries,
    size_t tile_size,
    size_t tile_idx,
    size_t& start_idx,
    size_t& end_idx
){
    start_idx = tile_idx * tile_size;
    end_idx = std::min(start_idx + tile_size, num_entries);
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_cm_tile(
    const genie::contact::ContactMatrixTilePayload& tile_payload,
    core::AlgoID codec_ID,
    BinMatDtype& bin_mat
){
    uint8_t* raw_data;
    size_t raw_data_len;
    uint8_t* compressed_data;
    size_t compressed_data_len;

    unsigned long tile_nrows;
    unsigned long tile_ncols;

    if (codec_ID == core::AlgoID::JBIG){
        compressed_data_len = tile_payload.GetPayloadSize();
        auto& payload = tile_payload.GetPayload();

        compressed_data = (uint8_t*)malloc(compressed_data_len * sizeof(uint8_t));
        memcpy(compressed_data, payload.data(), compressed_data_len);

        mpegg_jbig_decompress_default(
            &raw_data,
            &raw_data_len,
            compressed_data,
            compressed_data_len,
            &tile_nrows,
            &tile_ncols
        );

        free(compressed_data);

        ::genie::backend::bin_mat_from_bytes(raw_data, raw_data_len, static_cast<size_t>(tile_nrows), static_cast<size_t>(tile_ncols), bin_mat);

        free(raw_data);

    } else {
        tile_nrows = tile_payload.GetTileNRows();
        tile_ncols = tile_payload.GetTileNCols();
        UTILS_DIE("Not yet implemented");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_cm_tile(
    const BinMatDtype& bin_mat,
    const core::AlgoID codec_ID,
    genie::contact::ContactMatrixTilePayload& tile_payload
) {
    uint8_t* payload = nullptr;
    size_t payload_len = 0;
    uint8_t* compressed_payload = nullptr;
    size_t compressed_payload_len = 0;

    auto tile_nrows = static_cast<uint32_t>(::genie::backend::get_mat_shape(bin_mat, 0));
    auto tile_ncols = static_cast<uint32_t>(::genie::backend::get_mat_shape(bin_mat, 1));

    if (codec_ID == genie::core::AlgoID::JBIG) {
        if (tile_nrows > 0 && tile_ncols > 0) {
            ::genie::backend::bin_mat_to_bytes(bin_mat, &payload, payload_len);
            if (payload_len > 0) {
                mpegg_jbig_compress_default(
                    &compressed_payload,
                    &compressed_payload_len,
                    payload,
                    payload_len,
                    tile_nrows,
                    tile_ncols
                );
                free(payload);
            }
        }
    } else {
        UTILS_DIE("Not yet implemented for other codec!");
    }

    if (compressed_payload != nullptr) {
        auto _tile_payload = ContactMatrixTilePayload(
            codec_ID,
            tile_nrows,
            tile_ncols,
            &compressed_payload,
            compressed_payload_len
        );
        tile_payload = std::move(_tile_payload);
    } else {
        tile_payload = ContactMatrixTilePayload(
            codec_ID,
            tile_nrows,
            tile_ncols,
            std::vector<uint8_t>{}
        );
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void conv_noop_on_sparse_mat(
    UInt64VecDtype& tile_row_ids,
    UInt64VecDtype& tile_col_ids,
    UIntVecDtype& tile_counts,
    uint32_t bin_size_mult,
    bool sort_output
){
    size_t num_entries = ::genie::backend::get_arr_size(tile_counts);

    std::map<std::pair<uint64_t, uint64_t>, uint32_t> lr_sparse_tile_map;
    for (size_t idx_i = 0u; idx_i<num_entries; idx_i++){
        auto lr_row_id = ::genie::backend::get_arr_element(tile_row_ids, idx_i) / bin_size_mult;
        auto lr_col_id = ::genie::backend::get_arr_element(tile_col_ids, idx_i) / bin_size_mult;
        auto count = ::genie::backend::get_arr_element(tile_counts, idx_i);

        lr_sparse_tile_map[{lr_row_id, lr_col_id}] += count;
    }

    size_t lr_num_entries = lr_sparse_tile_map.size();
    ::genie::backend::resize_arr(tile_row_ids, lr_num_entries);
    ::genie::backend::resize_arr(tile_col_ids, lr_num_entries);
    ::genie::backend::resize_arr(tile_counts, lr_num_entries);

    size_t i_entry = 0u;
    for (const auto & pair_entry : lr_sparse_tile_map){
        ::genie::backend::set_arr_element(tile_row_ids, i_entry, (pair_entry.first).first);
        ::genie::backend::set_arr_element(tile_col_ids, i_entry, (pair_entry.first).second);
        ::genie::backend::set_arr_element(tile_counts, i_entry, pair_entry.second);
        i_entry++;
    }

    if (sort_output){
        ::genie::backend::sort_sparse_mat_inplace(
            tile_row_ids,
            tile_col_ids,
            tile_counts
        );
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_scm(
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    genie::contact::SubcontactMatrixPayload& scm_payload,
    core::record::ContactRecord& rec,
    uint32_t bin_size_mult
){
    BinVecDtype row_mask;
    BinVecDtype col_mask;
    std::vector<uint64_t> start1_vec;
    std::vector<uint64_t> end1_vec;
    std::vector<uint64_t> start2_vec;
    std::vector<uint64_t> end2_vec;
    std::vector<uint32_t> counts_vec;

    auto bin_size = cm_param.GetBinSize();
    auto target_bin_size = bin_size * bin_size_mult;
    auto tile_size = cm_param.GetTileSize();

    UTILS_DIE_IF(
        !cm_param.IsBinSizeMultiplierValid(bin_size_mult),
        "Bin size multiplier is invalid!"
    );

    auto chr1_ID = scm_param.GetChr1ID();
    auto chr2_ID = scm_param.GetChr2ID();
    auto is_intra_scm = scm_param.IsIntraSCM();

    auto codec_ID = scm_param.GetCodecID();
    bool row_mask_exists = scm_param.GetRowMaskExistsFlag();
    bool col_mask_exists = scm_param.GetColMaskExistsFlag();

    auto chr1_len = cm_param.GetChromosomeLength(chr1_ID);
    auto chr1_num_bin_entries = cm_param.GetNumBinEntries(chr1_ID);
    auto chr2_len = cm_param.GetChromosomeLength(chr2_ID);
    auto chr2_num_bin_entries = cm_param.GetNumBinEntries(chr2_ID);
    auto ntiles_in_row = cm_param.GetNumTiles(chr1_ID);
    auto ntiles_in_col = cm_param.GetNumTiles(chr2_ID);

    if (row_mask_exists || col_mask_exists){
        decode_scm_masks(
            cm_param,
            scm_param,
            scm_payload,
            row_mask,
            col_mask
        );
    }

    for (size_t i_tile = 0u; i_tile < ntiles_in_row; i_tile++) {
        for (size_t j_tile = 0u; j_tile < ntiles_in_col; j_tile++) {
            if (i_tile > j_tile && is_intra_scm){
                continue;
            }

            size_t start1_idx, end1_idx, start2_idx, end2_idx;
            comp_start_end_ids(chr1_num_bin_entries, tile_size, i_tile, start1_idx, end1_idx);
            comp_start_end_ids(chr2_num_bin_entries, tile_size, j_tile, start2_idx, end2_idx);
            
            auto& tile_param = scm_param.GetTileParameter(i_tile, j_tile);
            auto& tile_payload = scm_payload.GetTilePayload(i_tile, j_tile);
            auto binarization_mode = tile_param.binarization_mode;
            auto diag_transform_mode = tile_param.diag_tranform_mode;

            if (tile_payload.GetPayloadSize() == 0) {
                continue;
            }

            UIntMatDtype tile_mat;
            if (binarization_mode == BinarizationMode::ROW_BINARIZATION){
                BinMatDtype bin_mat;
                decode_cm_tile(tile_payload, codec_ID, bin_mat);
                inverse_transform_row_bin(bin_mat, tile_mat);
            } else {
                UTILS_DIE("no binarization is not supported yet!");
            }

            inverse_diag_transform(tile_mat, diag_transform_mode);

            UInt64VecDtype tile_row_ids;
            UInt64VecDtype tile_col_ids;
            UIntVecDtype tile_counts;

            dense_to_sparse(tile_mat, tile_row_ids, tile_col_ids, tile_counts);

            size_t nt = ::genie::backend::get_arr_size(tile_counts);
            for(size_t idx_i=0; idx_i<nt; ++idx_i) {
                start1_vec.push_back(::genie::backend::get_arr_element(tile_row_ids, idx_i) + start1_idx);
                start2_vec.push_back(::genie::backend::get_arr_element(tile_col_ids, idx_i) + start2_idx);
                counts_vec.push_back(::genie::backend::get_arr_element(tile_counts, idx_i));
            }
        }
    }

    if (row_mask_exists || col_mask_exists){
        // We need to convert start1_vec to UInt64VecDtype for insert_unaligned
        UInt64VecDtype start1_unified, start2_unified;
        ::genie::backend::resize_arr(start1_unified, start1_vec.size());
        ::genie::backend::resize_arr(start2_unified, start2_vec.size());
        for (size_t idx_i = 0; idx_i < start1_vec.size(); ++idx_i) {
            ::genie::backend::set_arr_element(start1_unified, idx_i, start1_vec[idx_i]);
            ::genie::backend::set_arr_element(start2_unified, idx_i, start2_vec[idx_i]);
        }

        insert_unaligned(start1_unified, start2_unified, is_intra_scm, row_mask, col_mask);

        for (size_t idx_i = 0; idx_i < start1_vec.size(); ++idx_i) {
            start1_vec[idx_i] = ::genie::backend::get_arr_element(start1_unified, idx_i);
            start2_vec[idx_i] = ::genie::backend::get_arr_element(start2_unified, idx_i);
        }
    }

    if (bin_size_mult != 1){
        UInt64VecDtype start1_unified, start2_unified;
        UIntVecDtype counts_unified;
        ::genie::backend::resize_arr(start1_unified, start1_vec.size());
        ::genie::backend::resize_arr(start2_unified, start2_vec.size());
        ::genie::backend::resize_arr(counts_unified, counts_vec.size());
        for (size_t idx_i = 0; idx_i < start1_vec.size(); ++idx_i) {
            ::genie::backend::set_arr_element(start1_unified, idx_i, start1_vec[idx_i]);
            ::genie::backend::set_arr_element(start2_unified, idx_i, start2_vec[idx_i]);
            ::genie::backend::set_arr_element(counts_unified, idx_i, counts_vec[idx_i]);
        }

        conv_noop_on_sparse_mat(start1_unified, start2_unified, counts_unified, bin_size_mult);

        start1_vec.resize(::genie::backend::get_arr_size(start1_unified));
        start2_vec.resize(::genie::backend::get_arr_size(start2_unified));
        counts_vec.resize(::genie::backend::get_arr_size(counts_unified));
        for (size_t idx_i = 0; idx_i < start1_vec.size(); ++idx_i) {
            start1_vec[idx_i] = ::genie::backend::get_arr_element(start1_unified, idx_i);
            start2_vec[idx_i] = ::genie::backend::get_arr_element(start2_unified, idx_i);
            counts_vec[idx_i] = ::genie::backend::get_arr_element(counts_unified, idx_i);
        }
    }
    
    auto sample_ID = scm_payload.GetSampleID();
    rec.SetSampleId(sample_ID);
    auto sample_name = std::string(cm_param.GetSampleName(sample_ID));
    rec.SetSampleName(std::move(sample_name));
    rec.SetChr1ID(chr1_ID);
    rec.SetChr2ID(chr2_ID);
    rec.SetBinSize(bin_size);
    
    end1_vec.resize(start1_vec.size());
    end2_vec.resize(start2_vec.size());

    for(size_t idx_i=0; idx_i<start1_vec.size(); ++idx_i) {
        start1_vec[idx_i] *= target_bin_size;
        end1_vec[idx_i] = std::min(start1_vec[idx_i] + target_bin_size, chr1_len);
        start2_vec[idx_i] *= target_bin_size;
        end2_vec[idx_i] = std::min(start2_vec[idx_i] + target_bin_size, chr2_len);
    }
    
    rec.SetCMValues(
        std::move(start1_vec),
        std::move(end1_vec),
        std::move(start2_vec),
        std::move(end2_vec),
        std::move(counts_vec)
    );
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_scm(
    ContactMatrixParameters& cm_param,
    core::record::ContactRecord& rec,
    SubcontactMatrixParameters& scm_param,
    genie::contact::SubcontactMatrixPayload& scm_payload,
    [[maybe_unused]] bool remove_unaligned_region,
    [[maybe_unused]] bool transform_mask,
    [[maybe_unused]] bool ena_diag_transform,
    [[maybe_unused]] bool ena_binarization,
    [[maybe_unused]] bool norm_as_weight,
    [[maybe_unused]] bool multiplicative_norm,
    core::AlgoID codec_ID
) {
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

    auto r_ids_vec = rec.GetStartPos1();
    auto c_ids_vec = rec.GetStartPos2();
    auto cnts_vec = rec.GetCounts();
    UInt64VecDtype row_ids, col_ids;
    UIntVecDtype counts;
    ::genie::backend::resize_arr(row_ids, r_ids_vec.size());
    ::genie::backend::resize_arr(col_ids, c_ids_vec.size());
    ::genie::backend::resize_arr(counts, cnts_vec.size());
    for (size_t idx_i = 0; idx_i < r_ids_vec.size(); ++idx_i) {
        ::genie::backend::set_arr_element(row_ids, idx_i, r_ids_vec[idx_i] / interval);
        ::genie::backend::set_arr_element(col_ids, idx_i, c_ids_vec[idx_i] / interval);
        ::genie::backend::set_arr_element(counts, idx_i, cnts_vec[idx_i]);
    }

    if (remove_unaligned_region){
        compute_masks(row_ids, col_ids, chr1_num_bin_entries, chr2_num_bin_entries, is_intra_scm, row_mask, col_mask);

        if (transform_mask){
            RunLengthEncodingData rowRLEData;
            set_rle_information_from_mask(rowRLEData, row_mask);
            
            if(is_intra_scm) {
                scm_payload.SetRowMaskPayload(SubcontactMatrixMaskPayload(rowRLEData.transformID, rowRLEData.firstVal, rowRLEData.rl_entries));
                scm_param.SetRowMaskExistsFlag(true);
                scm_param.SetColMaskExistsFlag(false);
            } else {
                RunLengthEncodingData colRLEData;
                set_rle_information_from_mask(colRLEData, col_mask);
                scm_payload.SetRowMaskPayload(SubcontactMatrixMaskPayload(rowRLEData.transformID, rowRLEData.firstVal, rowRLEData.rl_entries));
                scm_param.SetRowMaskExistsFlag(true);
                scm_payload.SetColMaskPayload(SubcontactMatrixMaskPayload(colRLEData.transformID, colRLEData.firstVal, colRLEData.rl_entries));
                scm_param.SetColMaskExistsFlag(true);
            }
        }

        remove_unaligned(row_ids, col_ids, is_intra_scm, row_mask, col_mask);

        if (scm_param.GetRowMaskExistsFlag() && is_intra_scm) {
            scm_param.SetColMaskExistsFlag(false);
        } else if (!scm_param.GetRowMaskExistsFlag()) {
            scm_payload.SetRowMaskPayload(SubcontactMatrixMaskPayload(std::move(row_mask)));
            scm_param.SetRowMaskExistsFlag(true);
        }
        if (!is_intra_scm && !scm_param.GetColMaskExistsFlag()) {
            scm_payload.SetColMaskPayload(SubcontactMatrixMaskPayload(std::move(col_mask)));
            scm_param.SetColMaskExistsFlag(true);
        }
    } else {
        scm_param.SetRowMaskExistsFlag(false);
        scm_param.SetColMaskExistsFlag(false);
    }

    auto nids = ::genie::backend::get_arr_size(row_ids);

    for (size_t i_tile = 0u; i_tile < ntiles_in_row; i_tile++) {
        for (size_t j_tile = 0u; j_tile < ntiles_in_col; j_tile++) {
            if (i_tile > j_tile && is_intra_scm) {
                continue;
            }

            size_t start1_idx, end1_idx, start2_idx, end2_idx;
            comp_start_end_ids(chr1_num_bin_entries, tile_size, i_tile, start1_idx, end1_idx);
            comp_start_end_ids(chr2_num_bin_entries, tile_size, j_tile, start2_idx, end2_idx);
            
            UInt64VecDtype tile_row_ids;
            UInt64VecDtype tile_col_ids;
            UIntVecDtype tile_counts;

            for(size_t idx_i=0; idx_i<nids; ++idx_i) {
                auto r = ::genie::backend::get_arr_element(row_ids, idx_i);
                auto c = ::genie::backend::get_arr_element(col_ids, idx_i);
                if (r >= start1_idx && r < end1_idx && c >= start2_idx && c < end2_idx) {
                    ::genie::backend::append_arr_element(tile_row_ids, r - start1_idx);
                    ::genie::backend::append_arr_element(tile_col_ids, c - start2_idx);
                    ::genie::backend::append_arr_element(tile_counts, ::genie::backend::get_arr_element(counts, idx_i));
                }
            }
            
            if (::genie::backend::get_arr_size(tile_counts) == 0) {
                scm_payload.SetTilePayload(i_tile, j_tile, ContactMatrixTilePayload(codec_ID, 0, 0, std::vector<uint8_t>{}));
                scm_param.SetTileParameter(i_tile, j_tile, {DiagonalTransformMode::NONE, BinarizationMode::ROW_BINARIZATION});
                continue;
            }
            
            UIntMatDtype tile_mat;
            sparse_to_dense(tile_row_ids, tile_col_ids, tile_counts, end1_idx - start1_idx, end2_idx - start2_idx, tile_mat);

            bool is_intra_tile = is_intra_scm && (i_tile == j_tile);
            if (ena_diag_transform && is_intra_tile) {
                diag_transform(tile_mat, DiagonalTransformMode::MODE_0);
            }
            
            BinMatDtype bin_mat;
            transform_row_bin(tile_mat, bin_mat);

            ContactMatrixTilePayload cm_tile_payload;
            encode_cm_tile(bin_mat, codec_ID, cm_tile_payload);

            scm_param.SetTileParameter(i_tile, j_tile, {
                (ena_diag_transform && is_intra_tile) ? DiagonalTransformMode::MODE_0 : DiagonalTransformMode::NONE,
                BinarizationMode::ROW_BINARIZATION
            });
            scm_payload.SetTilePayload(i_tile, j_tile, std::move(cm_tile_payload));
        }
    }
}

} // namespace genie::contact
