/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "contact_coder_xtensor.h"
#include "contact_types.h"
#include <codecs/include/mpegg-codecs.h>
#include <genie/core/contact_record/record.h>
#include <genie/util/runtime_exception.h>
#include <cstdint>
#include <cstring>
#include <xtensor/xadapt.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xsort.hpp>
#include "contact_matrix_parameters.h"
#include "contact_matrix_tile_payload.h"
#include "subcontact_matrix_parameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::contact::detail::xtensor {

// ---------------------------------------------------------------------------------------------------------------------

void compute_mask(
    // input
    UInt64VecDtype& ids,
    size_t nelems,
    // Output
    BinVecDtype& mask
){
    auto xt_ids = xt::adapt(ids, {ids.size()});
    auto xt_mask = xt::xtensor<bool, 1>({nelems}, false);

    auto unique_ids = xt::unique(xt_ids);
    for (auto id : unique_ids) {
        size_t idx = static_cast<size_t>(id);
        if (idx < nelems) xt_mask.at(idx) = true;
    }
    
    mask.assign(xt_mask.begin(), xt_mask.end());
}

// ---------------------------------------------------------------------------------------------------------------------

void compute_masks(
    // Inputs
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    size_t nrows,
    size_t ncols,
    const bool is_intra_scm,
    // Outputs:
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
){
    UTILS_DIE_IF(row_ids.size() != col_ids.size(),
                 "The size of row_ids and col_ids must be same!");

    if (is_intra_scm){
        UTILS_DIE_IF(nrows != ncols,
            "Both nentries must be the same for intra SCM!"
        );

        auto xt_row_ids = xt::adapt(row_ids, {row_ids.size()});
        auto xt_col_ids = xt::adapt(col_ids, {col_ids.size()});

        // Handle the symmetry of intra SCM
        auto ids = xt::concatenate(xt::xtuple(xt_row_ids, xt_col_ids));
        auto unique_ids = xt::unique(ids);
        
        auto xt_mask = xt::xtensor<bool, 1>({nrows}, false);
        for (auto id : unique_ids) {
            size_t idx = static_cast<size_t>(id);
            if (idx < nrows) xt_mask.at(idx) = true;
        }

        row_mask.assign(xt_mask.begin(), xt_mask.end());
        col_mask = row_mask;
    } else {
        detail::xtensor::compute_mask(row_ids, nrows, row_mask);
        detail::xtensor::compute_mask(col_ids, ncols, col_mask);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_scm_masks(
    // Inputs
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    const SubcontactMatrixPayload& scm_payload,
    // Outputs
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
){
    auto row_nentries = cm_param.GetNumBinEntries(scm_param.GetChr1ID());
    auto col_nentries = cm_param.GetNumBinEntries(scm_param.GetChr2ID());

    if (scm_param.GetRowMaskExistsFlag()){
        detail::xtensor::decode_scm_mask_payload(scm_payload.GetRowMaskPayload(), row_nentries, row_mask);
    } else {
        row_mask.assign(row_nentries, true);
    }

    if (scm_param.IsIntraSCM()){
        col_mask = row_mask;
    } else if (scm_param.GetColMaskExistsFlag()){
        detail::xtensor::decode_scm_mask_payload(scm_payload.GetColMaskPayload(), col_nentries, col_mask);
    } else {
        col_mask.assign(col_nentries, true);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_scm_mask_payload(
    // Inputs
    const SubcontactMatrixMaskPayload& mask_payload,
    size_t num_entries,
    // Outputs
    BinVecDtype& mask
) {
    auto transform_ID = mask_payload.GetTransformID();
    if (transform_ID == TransformID::ID_0){
        auto& mask_array = mask_payload.GetMaskArray();
        UTILS_DIE_IF(
            num_entries != mask_array.size(),
            "num_entries and the size of mask_array_ differ!"
        );
        mask = mask_array;
    } else {
        mask.assign(num_entries, false);

        bool first_val = mask_payload.GetFirstVal();
        auto& rl_entries = mask_payload.GetRlEntries();

        size_t start_idx = 0;
        size_t end_idx = 0;
        for (const auto& rl_entry: rl_entries){
            end_idx += rl_entry;
            std::fill(mask.begin() + start_idx, mask.begin() + std::min(end_idx, num_entries), first_val);
            start_idx = end_idx;
            first_val = !first_val;
        }
        UTILS_DIE_IF(
            start_idx > num_entries,
            "start_idx value must be smaller than num_entries!"
        );
        if (start_idx < num_entries) {
            std::fill(mask.begin() + start_idx, mask.end(), first_val);
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
    UTILS_DIE_IF(row_ids.size() != col_ids.size(),
                 "The size of row_ids and col_ids must be same!");


    if (is_intra_tile){
        //TODO(yeremia): to be deleted!
        UTILS_DIE_IF(row_mask != col_mask, "row_mask and col_mask are different!");

        auto num_entries = row_ids.size();
        auto mapping_len = row_mask.size();
        std::vector<uint64_t> mapping(mapping_len);
        uint64_t new_id = 0u;
        for (size_t i = 0u; i<mapping_len; i++){
            mapping[i] = new_id;
            if (row_mask[i]){
                new_id++;
            }
        }

        for (size_t i = 0u; i<num_entries; i++){
            row_ids[i] = mapping[row_ids[i]];
            col_ids[i] = mapping[col_ids[i]];
        }
    } else {
        auto row_mapping_len = row_mask.size();
        std::vector<uint64_t> row_mapping(row_mapping_len);
        {
            uint64_t new_id = 0u;
            for (size_t i = 0u; i<row_mapping_len; i++){
                row_mapping[i] = new_id;
                if (row_mask[i]){
                    new_id++;
                }
            }
        }
        for (auto& row_id : row_ids) row_id = row_mapping[row_id];

        auto col_mapping_len = col_mask.size();
        std::vector<uint64_t> col_mapping(col_mapping_len);
        {
            uint64_t new_id = 0u;
            for (size_t i = 0u; i<col_mapping_len; i++){
                col_mapping[i] = new_id;
                if (col_mask[i]){
                    new_id++;
                }
            }
        }
        for (auto& col_id : col_ids) col_id = col_mapping[col_id];
    }

}

// ---------------------------------------------------------------------------------------------------------------------

void insert_unaligned(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    bool is_intra_tile,
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
){
    UTILS_DIE_IF(row_ids.size() != col_ids.size(),
                 "The size of row_ids and col_ids must be same!");


    if (is_intra_tile){
        UTILS_DIE_IF(row_mask != col_mask, "row_mask and col_mask are different!");

        std::vector<uint64_t> mapping;
        for (size_t i = 0; i < row_mask.size(); ++i) if (row_mask[i]) mapping.push_back(i);

        for (size_t i = 0u; i<row_ids.size(); i++){
            row_ids[i] = mapping[row_ids[i]];
            col_ids[i] = mapping[col_ids[i]];
        }

    } else {
        // Handle row_mask
        {
            std::vector<uint64_t> mapping;
            for (size_t i = 0; i < row_mask.size(); ++i) if (row_mask[i]) mapping.push_back(i);
            for (size_t i = 0u; i<row_ids.size(); i++){
                row_ids[i] = mapping[row_ids[i]];
            }
        }

        // Handle col_mask
        {
            std::vector<uint64_t> mapping;
            for (size_t i = 0; i < col_mask.size(); ++i) if (col_mask[i]) mapping.push_back(i);
            for (size_t i = 0u; i<col_ids.size(); i++){
                col_ids[i] = mapping[col_ids[i]];
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void sparse_to_dense(
    // Inputs
    const UInt64VecDtype& row_ids,
    const UInt64VecDtype& col_ids,
    const UIntVecDtype& counts,
    size_t nrows,
    size_t ncols,
    // Outputs
    UIntMatDtype& mat
){
    mat.assign(nrows, std::vector<uint32_t>(ncols, 0));

    for (size_t i = 0u; i< counts.size(); i++){
        mat[row_ids[i]][col_ids[i]] = counts[i];
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void dense_to_sparse(
    // Inputs
    const UIntMatDtype& mat,
    // Outputs
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts
){
    row_ids.clear();
    col_ids.clear();
    counts.clear();
    for (size_t i = 0; i < mat.size(); ++i) {
        for (size_t j = 0; j < mat[i].size(); ++j) {
            if (mat[i][j] > 0) {
                row_ids.push_back(i);
                col_ids.push_back(j);
                counts.push_back(mat[i][j]);
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void sort_by_row_ids(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts
){
    size_t num_entries = row_ids.size();
    std::vector<size_t> p(num_entries);
    std::iota(p.begin(), p.end(), 0);
    std::stable_sort(p.begin(), p.end(),
                     [&](size_t i, size_t j) { return row_ids[i] < row_ids[j]; });

    UInt64VecDtype sorted_row_ids(num_entries);
    UInt64VecDtype sorted_col_ids(num_entries);
    UIntVecDtype sorted_counts(num_entries);
    for (size_t i = 0; i < num_entries; ++i) {
        sorted_row_ids[i] = row_ids[p[i]];
        sorted_col_ids[i] = col_ids[p[i]];
        sorted_counts[i] = counts[p[i]];
    }
    row_ids = std::move(sorted_row_ids);
    col_ids = std::move(sorted_col_ids);
    counts = std::move(sorted_counts);
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_diag_transform(
    UIntMatDtype& mat,
    DiagonalTransformMode mode
){
    if (mode == DiagonalTransformMode::NONE) {
        return;
    }

    auto nrows = mat.size();
    if (nrows == 0) return;
    auto ncols = mat[0].size();

    // Adapter for internal xtensor usage
    std::vector<uint32_t> flat_mat;
    flat_mat.reserve(nrows * ncols);
    for (const auto& row : mat) flat_mat.insert(flat_mat.end(), row.begin(), row.end());
    auto xt_mat = xt::adapt(flat_mat, std::vector<size_t>{nrows, ncols});

    if (mode == DiagonalTransformMode::MODE_0) {
        auto target_nrows = ncols;
        auto xt_trans_mat = xt::xtensor<uint32_t, 2>({target_nrows, ncols}, 0);

        auto o = 0u;
        for (size_t k_diag = 0u; k_diag < ncols; k_diag++) {
            for (size_t target_i = 0u; target_i < (target_nrows - k_diag); target_i++) {
                size_t target_j = target_i + k_diag;
                size_t i = o / target_nrows;
                size_t j = o % target_nrows;
                xt_trans_mat.at(target_i, target_j) = xt_mat(i, j);
                o++;
            }
        }
        
        mat.assign(target_nrows, std::vector<uint32_t>(ncols));
        for (size_t i = 0; i < target_nrows; ++i) {
            for (size_t j = 0; j < ncols; ++j) {
                mat[i][j] = xt_trans_mat(i, j);
            }
        }
    } else {
        auto xt_trans_mat = xt::empty<uint32_t>({nrows, ncols});
        Int64VecDtype diag_ids;

        if (mode == DiagonalTransformMode::MODE_1){
            std::vector<int64_t> diag_ids_vec;
            diag_ids_vec.push_back(0);
            auto ndiags = std::max((int64_t)nrows, (int64_t)ncols);
            for (int64_t diag_id = 1; diag_id < ndiags; diag_id++){
                if (diag_id < (int64_t)ncols) diag_ids_vec.push_back(diag_id);
                if (diag_id < (int64_t)nrows) diag_ids_vec.push_back(-diag_id);
            }
            diag_ids = std::move(diag_ids_vec);
        } else if (mode == DiagonalTransformMode::MODE_2){
            auto diag_ids_xt = xt::arange(-(int64_t)nrows+1, (int64_t)ncols, 1);
            diag_ids.assign(diag_ids_xt.begin(), diag_ids_xt.end());
        } else if (mode == DiagonalTransformMode::MODE_3){
            auto diag_ids_xt = xt::arange<int64_t>((int64_t)ncols-1, -(int64_t)nrows, -1);
            diag_ids.assign(diag_ids_xt.begin(), diag_ids_xt.end());
        }

        int64_t target_i, target_j;
        int64_t i_offset, j_offset;
        int64_t nelems_in_diag;
        auto o = 0u;
        for (auto diag_id : diag_ids){
            if (diag_id >= 0) {
                nelems_in_diag = std::max((int64_t)nrows, (int64_t)ncols) - diag_id;
                i_offset = 0;
                j_offset = diag_id;
            } else {
                nelems_in_diag = std::max((int64_t)nrows, (int64_t)ncols) + diag_id;
                i_offset = -diag_id;
                j_offset = 0;
            }
            for (int64_t k_diag = 0; k_diag < nelems_in_diag; k_diag++){
                target_i = k_diag + i_offset;
                target_j = k_diag + j_offset;
                if (target_i >= (int64_t)nrows || target_j >= (int64_t)ncols) break;

                size_t i = o / ncols;
                size_t j = o % ncols;
                xt_trans_mat.at(target_i, target_j) = xt_mat(i, j);
                o++;
            }
        }
        
        mat.assign(nrows, std::vector<uint32_t>(ncols));
        for (size_t i = 0; i < nrows; ++i) {
            for (size_t j = 0; j < ncols; ++j) {
                mat[i][j] = xt_trans_mat(i, j);
            }
        }
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

    auto nrows = mat.size();
    if (nrows == 0) return;
    auto ncols = mat[0].size();

    // Adapter for internal xtensor usage
    std::vector<uint32_t> flat_mat;
    flat_mat.reserve(nrows * ncols);
    for (const auto& row : mat) flat_mat.insert(flat_mat.end(), row.begin(), row.end());
    auto xt_mat = xt::adapt(flat_mat, std::vector<size_t>{nrows, ncols});

    if (mode == DiagonalTransformMode::MODE_0) {
        UTILS_DIE_IF(nrows != ncols, "Matrix must be a square!");

        auto new_nrows = nrows / 2 + 1;
        auto xt_trans_mat = xt::xtensor<uint32_t, 2>({new_nrows, nrows}, 0);

        auto o = 0u;
        for (size_t k_diag = 0u; k_diag < nrows; k_diag++) {
            for (size_t i = 0u; i < (nrows - k_diag); i++) {
                size_t j = i + k_diag;
                auto v = xt_mat(i, j);
                if (v != 0) {
                    size_t target_i = o / nrows;
                    size_t target_j = o % nrows;
                    xt_trans_mat.at(target_i, target_j) = v;
                }
                o++;
            }
        }
        
        mat.assign(new_nrows, std::vector<uint32_t>(nrows));
        for (size_t i = 0; i < new_nrows; ++i) {
            for (size_t j = 0; j < nrows; ++j) {
                mat[i][j] = xt_trans_mat(i, j);
            }
        }
    } else {
        auto xt_trans_mat = xt::xtensor<uint32_t, 2>({nrows, ncols}, 0);
        Int64VecDtype diag_ids;

        if (mode == DiagonalTransformMode::MODE_1){
            std::vector<int64_t> diag_ids_vec;
            diag_ids_vec.push_back(0);
            auto ndiags = std::max((int64_t)nrows, (int64_t)ncols);
            for (auto diag_id = 1; diag_id < ndiags; diag_id++){
                if (diag_id < (int64_t)ncols) diag_ids_vec.push_back(diag_id);
                if (diag_id < (int64_t)nrows) diag_ids_vec.push_back(-diag_id);
            }
            diag_ids = std::move(diag_ids_vec);
        } else if (mode == DiagonalTransformMode::MODE_2){
            auto diag_ids_xt = xt::arange(-(int64_t)nrows+1, (int64_t)ncols, 1);
            diag_ids.assign(diag_ids_xt.begin(), diag_ids_xt.end());
        } else if (mode == DiagonalTransformMode::MODE_3){
            auto diag_ids_xt = xt::arange<int64_t>((int64_t)ncols-1, -(int64_t)nrows, -1);
            diag_ids.assign(diag_ids_xt.begin(), diag_ids_xt.end());
        }

        int64_t i, j;
        int64_t i_offset, j_offset;
        int64_t nelems_in_diag;
        auto o = 0u;
        for (auto diag_id : diag_ids){
            if (diag_id >= 0) {
                nelems_in_diag = std::max((int64_t)nrows, (int64_t)ncols) - diag_id;
                i_offset = 0;
                j_offset = diag_id;
            } else {
                nelems_in_diag = std::max((int64_t)nrows, (int64_t)ncols) + diag_id;
                i_offset = -diag_id;
                j_offset = 0;
            }
            for (int64_t k_diag = 0; k_diag < nelems_in_diag; k_diag++){
                i = k_diag + i_offset;
                j = k_diag + j_offset;
                if (i >= (int64_t)nrows || j >= (int64_t)ncols) break;

                auto v = xt_mat(i, j);
                size_t new_i = o / ncols;
                size_t new_j = o % ncols;
                xt_trans_mat.at(new_i, new_j) = v;
                o++;
            }
        }
        
        mat.assign(nrows, std::vector<uint32_t>(ncols));
        for (size_t i_row = 0; i_row < nrows; ++i_row) {
            for (size_t j_col = 0; j_col < ncols; ++j_col) {
                mat[i_row][j_col] = xt_trans_mat(i_row, j_col);
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_row_bin(
    // Inputs
    const BinMatDtype& bin_mat,
    // Outputs
    UIntMatDtype& mat
){
    size_t bin_mat_nrows = bin_mat.size();
    if (bin_mat_nrows == 0) return;
    size_t bin_mat_ncols = bin_mat[0].size();

    UTILS_DIE_IF(bin_mat_nrows == 0, "Invalid mat_nrows!");
    UTILS_DIE_IF(bin_mat_ncols == 0, "Invalid mat_ncols!");

    size_t mat_ncols = bin_mat_ncols-1;

    size_t mat_nrows = 0;
    for (const auto& row : bin_mat) if (row[0]) mat_nrows++;

    UTILS_DIE_IF(mat_nrows == 0, "Invalid mat_nrows!");
    UTILS_DIE_IF(mat_ncols == 0, "Invalid mat_ncols!");

    mat.assign(mat_nrows, std::vector<uint32_t>(mat_ncols, 0));

    size_t target_i = 0;
    uint8_t bit_pos = 0;
    for (size_t i = 0u; i < bin_mat_nrows; i++){
        for (size_t j = 1u; j < bin_mat_ncols; j++) {
            if (bin_mat[i][j]) {
                mat[target_i][j-1] |= (1u << bit_pos);
            }
        }
        bool sentinel_flag = bin_mat[i][0];

        if (sentinel_flag){
            target_i++;
            bit_pos = 0;
        } else{
            bit_pos++;
        }
    }

    UTILS_DIE_IF(target_i != mat_nrows, "Not all of the mat rows are processed!");
}

// ---------------------------------------------------------------------------------------------------------------------

void transform_row_bin(
    // Inputs
    const UIntMatDtype& mat,
    // Outputs
    BinMatDtype& bin_mat
) {
    auto nrows = mat.size();
    if (nrows == 0) return;
    auto ncols = mat[0].size();

    std::vector<uint8_t> nbits_per_row(nrows);
    for (size_t i = 0; i < nrows; ++i) {
        uint32_t max_val = 0;
        for (uint32_t val : mat[i]) max_val = std::max(max_val, val);
        nbits_per_row[i] = (max_val == 0) ? 1 : static_cast<uint8_t>(std::ceil(std::log2(max_val + 1u)));
        if (nbits_per_row[i] == 0) nbits_per_row[i] = 1;
    }

    uint64_t bin_mat_nrows = 0;
    for (auto n : nbits_per_row) bin_mat_nrows += n;
    uint64_t bin_mat_ncols = ncols + 1;

    bin_mat.assign(bin_mat_nrows, std::vector<bool>(bin_mat_ncols, false));

    size_t target_i = 0;
    for (size_t i = 0; i < nrows; i++) {
        auto bitlength = nbits_per_row[i];
        for (size_t i_bit = 0; i_bit < bitlength; i_bit++) {
            for (size_t j = 0; j < ncols; ++j) {
                if (mat[i][j] & (1u << i_bit)) bin_mat[target_i][j+1] = true;
            }
            target_i++;
        }

        // Set the sentinel flag
        bin_mat[target_i - 1][0] = true;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void comp_start_end_ids(
    // Inputs
    size_t num_entries,
    size_t tile_size,
    size_t tile_idx,
    // Outputs
    size_t& start_idx,
    size_t& end_idx
){
    start_idx = tile_idx * tile_size;
    end_idx = std::min(start_idx + tile_size, num_entries);
}

// ---------------------------------------------------------------------------------------------------------------------

void bin_mat_to_bytes(
    // Inputs
    const BinMatDtype& bin_mat,
    // Outputs
    uint8_t** payload,
    size_t& payload_len
) {
    auto nrows = bin_mat.size();
    if (nrows == 0) {
        payload_len = 0;
        *payload = nullptr;
        return;
    }
    auto ncols = bin_mat[0].size();

    auto bpl = (ncols >> 3u) + ((ncols & 7u) > 0u);  // Ceil div operation
    payload_len = bpl * nrows;
    *payload = (unsigned char*) calloc (payload_len, sizeof(unsigned char));

    for (size_t i = 0u; i < nrows; i++) {
        size_t row_offset = i * bpl;
        for (size_t j = 0u; j < ncols; j++) {
            auto byte_offset = row_offset + (j >> 3u);
            uint8_t shift = (7u - (j & 7u));
            auto val = static_cast<uint8_t>(bin_mat[i][j]);
            val = static_cast<uint8_t>(val << shift);
            *(*payload + byte_offset) |= val;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void bin_mat_from_bytes(

    // Inputs

    const uint8_t* payload,

    size_t payload_len,

    size_t nrows,

    size_t ncols,

    // Outputs

    BinMatDtype& bin_mat

) {



    auto bpl = (ncols >> 3u) + ((ncols & 7u) > 0u);  // bytes per line with ceil operation

    UTILS_DIE_IF(payload_len != static_cast<size_t>(nrows * bpl), "Invalid payload_len / nrows / ncols!");



    bin_mat.assign(nrows, std::vector<bool>(ncols, false));



    for (size_t i = 0u; i < nrows; i++) {

        size_t row_offset = i * bpl;

        for (size_t j = 0u; j < ncols; j++) {

            auto byte_offset = row_offset + (j >> 3u);

            uint8_t shift = (7u - (j & 7u));

            bin_mat[i][j] = (*(payload + byte_offset) >> shift) & 1u;

        }

    }

}

// ---------------------------------------------------------------------------------------------------------------------

void decode_cm_tile(
    // Inputs
    const genie::contact::ContactMatrixTilePayload& tile_payload,
    core::AlgoID codec_ID,
    // Outputs
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

        bin_mat_from_bytes(
            raw_data,
            raw_data_len,
            static_cast<size_t>(tile_nrows),
            static_cast<size_t>(tile_ncols),
            bin_mat
        );

        free(raw_data);

    } else {

        tile_nrows = tile_payload.GetTileNRows();
        tile_ncols = tile_payload.GetTileNCols();

        UTILS_DIE("Not yet implemented");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_cm_tile(
    // Inputs
    const BinMatDtype& bin_mat,
    const core::AlgoID codec_ID,
    // Outputs
    genie::contact::ContactMatrixTilePayload& tile_payload
) {
    uint8_t* payload;
    size_t payload_len;
    uint8_t* compressed_payload;
    size_t compressed_payload_len;

    auto tile_nrows = static_cast<uint32_t>(bin_mat.size());
    auto tile_ncols = static_cast<uint32_t>(bin_mat.empty() ? 0 : bin_mat[0].size());

    if (codec_ID == genie::core::AlgoID::JBIG) {

        detail::xtensor::bin_mat_to_bytes(bin_mat, &payload, payload_len);

        mpegg_jbig_compress_default(
            &compressed_payload,
            &compressed_payload_len,
            payload,
            payload_len,
            tile_nrows,
            tile_ncols
        );

        free(payload);

    } else {
        UTILS_DIE("Not yet implemented for other codec!");
    }

    auto _tile_payload = ContactMatrixTilePayload(
        codec_ID,
        tile_nrows,
        tile_ncols,
        &compressed_payload,
        compressed_payload_len
    );

    tile_payload = std::move(_tile_payload);
}

// ---------------------------------------------------------------------------------------------------------------------

void conv_noop_on_sparse_mat(
    // Inputs and Outputs
    UInt64VecDtype& tile_row_ids,
    UInt64VecDtype& tile_col_ids,
    UIntVecDtype& tile_counts,
    uint32_t bin_size_mult,
    // Options
    bool sort_output
){

    size_t num_entries = tile_counts.size();

    std::map<std::pair<uint64_t, uint64_t>, uint32_t> lr_sparse_tile;
    for (auto i = 0u; i<num_entries; i++){
        auto lr_row_id = tile_row_ids[i] / bin_size_mult;
        auto lr_col_id = tile_col_ids[i] / bin_size_mult;
        auto count = tile_counts[i];

        auto row_col_id_pair = std::pair<uint64_t, uint64_t>(lr_row_id, lr_col_id);

        auto it = lr_sparse_tile.find(row_col_id_pair);
        if (it != lr_sparse_tile.end()){
            it->second += count;
        } else{
            lr_sparse_tile.emplace(row_col_id_pair, count);
        }
    }

    size_t lr_num_entries = lr_sparse_tile.size();

    UInt64VecDtype lr_tile_row_ids(lr_num_entries);
    UInt64VecDtype lr_tile_col_ids(lr_num_entries);
    UIntVecDtype lr_tile_counts(lr_num_entries);

    auto i_entry = 0u;
    for (const auto & it : lr_sparse_tile){
        lr_tile_row_ids[i_entry] = (it.first).first;
        lr_tile_col_ids[i_entry] = (it.first).second;
        lr_tile_counts[i_entry] = it.second;
        i_entry++;
    }

    lr_sparse_tile.clear();

    // Sort the tile_row_ids, tile_col_ids, and tile_counts
    //      according to tile_row_ids and tile_col_ids
    if (sort_output){
        detail::xtensor::sort_sparse_mat_inplace(
            lr_tile_row_ids,
            lr_tile_col_ids,
            lr_tile_counts
        );
    }

    tile_row_ids = std::move(lr_tile_row_ids);
    tile_col_ids = std::move(lr_tile_col_ids);
    tile_counts = std::move(lr_tile_counts);

}

// ---------------------------------------------------------------------------------------------------------------------

void sort_sparse_mat_inplace(
    // Inputs and Outputs
    UInt64VecDtype& tile_row_ids,
    UInt64VecDtype& tile_col_ids,
    UIntVecDtype& tile_counts
){
    size_t num_entries = tile_counts.size();

    // Assert that all input vectors have the same size
    assert(tile_row_ids.size() == num_entries);
    assert(tile_col_ids.size() == num_entries);

    std::vector<size_t> sort_ids(num_entries);
    std::iota(sort_ids.begin(), sort_ids.end(), 0);

    std::stable_sort(
        sort_ids.begin(),
        sort_ids.end(),
        [&tile_row_ids, &tile_col_ids](size_t i1, size_t i2) {
            return std::tie(tile_row_ids[i1], tile_col_ids[i1]) < std::tie(tile_row_ids[i2], tile_col_ids[i2]);
        }
    );

    if (std::is_sorted(sort_ids.begin(), sort_ids.end())) {
        return;
    }

    // Only reorder the ids and counts if the original order is not sorted
    UInt64VecDtype sorted_tile_row_ids(num_entries);
    UInt64VecDtype sorted_tile_col_ids(num_entries);
    UIntVecDtype sorted_tile_counts(num_entries);

    for (auto i_entry = 0u; i_entry < num_entries; i_entry++){
        sorted_tile_row_ids[i_entry] = tile_row_ids[sort_ids[i_entry]];
        sorted_tile_col_ids[i_entry] = tile_col_ids[sort_ids[i_entry]];
        sorted_tile_counts[i_entry] = tile_counts[sort_ids[i_entry]];
    }

    tile_row_ids = std::move(sorted_tile_row_ids);
    tile_col_ids = std::move(sorted_tile_col_ids);
    tile_counts = std::move(sorted_tile_counts);
}

// ---------------------------------------------------------------------------------------------------------------------

void set_rle_information_from_mask(
    RunLengthEncodingData& rleData,
    const BinVecDtype& scm_mask
)   {
    bool prevValue;
    uint32_t count = 1;
    size_t index = 0;
    prevValue = scm_mask[0];
    rleData.firstVal = scm_mask[0];

    // init counts from maximum possible size
    std::vector<uint32_t> rl_entries_vec(scm_mask.size());

    for(size_t i = 1; i < scm_mask.size(); i++) {
        if (scm_mask[i] == prevValue) {
            count++;
        } else {
            rl_entries_vec[index++] = count;
            prevValue = !prevValue;
            count = 1;
        }
    }
    rl_entries_vec[index++] = count; // add the last entry

    rl_entries_vec.resize(index);
    rleData.rl_entries = std::move(rl_entries_vec);

    rleData.maxCount = *std::max_element(rleData.rl_entries.begin(), rleData.rl_entries.end());
    if (rleData.maxCount <= UINT8_MAX) {
        rleData.transformID = TransformID::ID_1;
    } else if (rleData.maxCount <= UINT16_MAX) {
        rleData.transformID = TransformID::ID_2;
    } else if (rleData.maxCount <= UINT32_MAX) {
        rleData.transformID = TransformID::ID_3;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_scm(
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    SubcontactMatrixPayload& scm_payload,
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
    auto row_mask_exists = scm_param.GetRowMaskExistsFlag();
    auto col_mask_exists = scm_param.GetColMaskExistsFlag();

    auto chr1_len = cm_param.GetChromosomeLength(chr1_ID);
    auto chr1_num_bin_entries = cm_param.GetNumBinEntries(chr1_ID);
    auto chr2_len = cm_param.GetChromosomeLength(chr2_ID);
    auto chr2_num_bin_entries = cm_param.GetNumBinEntries(chr2_ID);
    auto ntiles_in_row = cm_param.GetNumTiles(chr1_ID);
    auto ntiles_in_col = cm_param.GetNumTiles(chr2_ID);

    if (row_mask_exists || col_mask_exists){
        detail::xtensor::decode_scm_masks(
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

            UIntMatDtype tile_mat;
            size_t start1_idx, end1_idx, start2_idx, end2_idx;
            UInt64VecDtype tile_row_ids;
            UInt64VecDtype tile_col_ids;
            UIntVecDtype tile_counts;

            auto& tile_param = scm_param.GetTileParameter(i_tile, j_tile);
            auto& tile_payload = scm_payload.GetTilePayload(i_tile, j_tile);
            auto binarization_mode = tile_param.binarization_mode;
            auto diag_transform_mode = tile_param.diag_tranform_mode;
            bool is_intra_tile = is_intra_scm && (i_tile == j_tile);

            if (tile_payload.GetPayloadSize() == 0){
                continue;
            }

            if (binarization_mode == BinarizationMode::ROW_BINARIZATION){
                BinMatDtype bin_mat;

                detail::xtensor::decode_cm_tile(
                    tile_payload,
                    codec_ID,
                    bin_mat
                );

                detail::xtensor::inverse_transform_row_bin(
                    bin_mat,
                    tile_mat
                );

            } else {
                UTILS_DIE("no binarization is not supported yet!");
            }

            detail::xtensor::inverse_diag_transform(
                tile_mat,
                diag_transform_mode
            );

            detail::xtensor::comp_start_end_ids(
                chr1_num_bin_entries,
                tile_size,
                i_tile,
                start1_idx,
                end1_idx
            );

            detail::xtensor::comp_start_end_ids(
                chr2_num_bin_entries,
                tile_size,
                j_tile,
                start2_idx,
                end2_idx
            );

            detail::xtensor::dense_to_sparse(
                tile_mat,
                tile_row_ids,
                tile_col_ids,
                tile_counts
            );

            if (row_mask_exists || col_mask_exists){
                BinVecDtype tile_row_mask(row_mask.begin() + start1_idx, row_mask.begin() + end1_idx);
                BinVecDtype tile_col_mask(col_mask.begin() + start2_idx, col_mask.begin() + end2_idx);

                detail::xtensor::insert_unaligned(
                    tile_row_ids,
                    tile_col_ids,
                    is_intra_tile,
                    tile_row_mask,
                    tile_col_mask
                );
            }

            for (auto& id : tile_row_ids) id += start1_idx;
            for (auto& id : tile_col_ids) id += start2_idx;

            if (bin_size_mult != 1){
                detail::xtensor::conv_noop_on_sparse_mat(
                    tile_row_ids,
                    tile_col_ids,
                    tile_counts,
                    bin_size_mult,
                    false
                );
            }

            start1_vec.insert(start1_vec.end(), tile_row_ids.begin(), tile_row_ids.end());
            start2_vec.insert(start2_vec.end(), tile_col_ids.begin(), tile_col_ids.end());
            counts_vec.insert(counts_vec.end(), tile_counts.begin(), tile_counts.end());
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

    for(size_t i=0; i<start1_vec.size(); ++i) {
        start1_vec[i] *= target_bin_size;
        end1_vec[i] = std::min(start1_vec[i] + target_bin_size, chr1_len);
        start2_vec[i] *= target_bin_size;
        end2_vec[i] = std::min(start2_vec[i] + target_bin_size, chr2_len);
    }
    
    rec.SetCMValues(
        std::move(start1_vec),
        std::move(end1_vec),
        std::move(start2_vec),
        std::move(end2_vec),
        std::move(counts_vec)
    );
}

void encode_scm(
    ContactMatrixParameters& cm_param,
    core::record::ContactRecord& rec,
    SubcontactMatrixParameters& scm_param,
    genie::contact::SubcontactMatrixPayload& scm_payload,
    bool remove_unaligned_region,
    bool transform_mask,
    bool ena_diag_transform,
    bool ena_binarization,
    bool norm_as_weight,
    bool multiplicative_norm,
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

  // Convert std::vector from record to xtensor for processing
  UInt64VecDtype row_ids = rec.GetStartPos1();
  for (auto& id : row_ids) id /= interval; 

  UInt64VecDtype col_ids = rec.GetStartPos2();
  for (auto& id : col_ids) id /= interval;

  UIntVecDtype counts = rec.GetCounts();

  if (remove_unaligned_region){
      detail::xtensor::compute_masks(
          row_ids,
          col_ids,
          chr1_num_bin_entries,
          chr2_num_bin_entries,
          is_intra_scm,
          row_mask,
          col_mask
      );

      if (transform_mask){
          RunLengthEncodingData rowRLEData;
          detail::xtensor::set_rle_information_from_mask(rowRLEData, row_mask);
          
          if(is_intra_scm) {
            scm_payload.SetRowMaskPayload(SubcontactMatrixMaskPayload(
                rowRLEData.transformID,
                rowRLEData.firstVal,
                rowRLEData.rl_entries
            ));
            scm_param.SetRowMaskExistsFlag(true);
            scm_param.SetColMaskExistsFlag(false);
          } else {
            RunLengthEncodingData colRLEData;
            detail::xtensor::set_rle_information_from_mask(colRLEData, col_mask);
            scm_payload.SetRowMaskPayload(SubcontactMatrixMaskPayload(
                rowRLEData.transformID,
                rowRLEData.firstVal,
                rowRLEData.rl_entries
            ));
            scm_param.SetRowMaskExistsFlag(true);
            scm_payload.SetColMaskPayload(SubcontactMatrixMaskPayload(
                colRLEData.transformID,
                colRLEData.firstVal,
                colRLEData.rl_entries
            ));
            scm_param.SetColMaskExistsFlag(true);
          }

      } else {
        // Direct mask storage, no RLE
        scm_payload.SetRowMaskPayload(SubcontactMatrixMaskPayload(std::move(row_mask)));
        scm_param.SetRowMaskExistsFlag(true);
        if (!is_intra_scm){
          scm_payload.SetColMaskPayload(SubcontactMatrixMaskPayload(std::move(col_mask)));
          scm_param.SetColMaskExistsFlag(true);
        }
      }

      detail::xtensor::remove_unaligned(
          row_ids,
          col_ids,
          is_intra_scm,
          row_mask,
          col_mask
      );
  } else {
      scm_param.SetRowMaskExistsFlag(false);
      scm_param.SetColMaskExistsFlag(false);
  }

  for (size_t i_tile = 0u; i_tile < ntiles_in_row; i_tile++) {
      for (size_t j_tile = 0u; j_tile < ntiles_in_col; j_tile++) {
          if (i_tile > j_tile && is_intra_scm) {
              continue;
          }

          size_t start1_idx, end1_idx, start2_idx, end2_idx;
          detail::xtensor::comp_start_end_ids(chr1_num_bin_entries, tile_size, i_tile, start1_idx, end1_idx);
          detail::xtensor::comp_start_end_ids(chr2_num_bin_entries, tile_size, j_tile, start2_idx, end2_idx);

          // Filtering
          std::vector<uint64_t> tile_row_ids_vec;
          std::vector<uint64_t> tile_col_ids_vec;
          std::vector<uint32_t> tile_counts_vec;
          for (size_t k = 0; k < row_ids.size(); ++k) {
              if (row_ids[k] >= start1_idx && row_ids[k] < end1_idx &&
                  col_ids[k] >= start2_idx && col_ids[k] < end2_idx) {
                  tile_row_ids_vec.push_back(row_ids[k] - start1_idx);
                  tile_col_ids_vec.push_back(col_ids[k] - start2_idx);
                  tile_counts_vec.push_back(counts[k]);
              }
          }
          
          if (tile_counts_vec.empty()) {
              scm_payload.SetTilePayload(
                  i_tile,
                  j_tile,
                  ContactMatrixTilePayload(codec_ID, 0, 0, nullptr, 0)
              );
              scm_param.SetTileParameter(
                  i_tile,
                  j_tile,
                  {DiagonalTransformMode::NONE, BinarizationMode::ROW_BINARIZATION}
              );
              continue;
          }
          
          UInt64VecDtype tile_row_ids = std::move(tile_row_ids_vec);
          UInt64VecDtype tile_col_ids = std::move(tile_col_ids_vec);
          UIntVecDtype tile_counts = std::move(tile_counts_vec);

          detail::xtensor::sort_sparse_mat_inplace(tile_row_ids, tile_col_ids, tile_counts);

          UIntMatDtype tile_mat;
          detail::xtensor::sparse_to_dense(
              tile_row_ids,
              tile_col_ids,
              tile_counts,
              tile_size,
              tile_size,
              tile_mat
          );

          if (ena_diag_transform && is_intra_scm && i_tile == j_tile) {
              detail::xtensor::diag_transform(tile_mat, DiagonalTransformMode::MODE_0);
          }
          
          BinMatDtype bin_mat;
          if (ena_binarization) {
              detail::xtensor::transform_row_bin(tile_mat, bin_mat);
          } else {
              UTILS_DIE("Non-binarized encoding not supported for xtensor!");
          }

          genie::contact::ContactMatrixTilePayload cm_tile_payload;
          detail::xtensor::encode_cm_tile(bin_mat, codec_ID, cm_tile_payload);

          scm_param.SetTileParameter(
              i_tile,
              j_tile,
              {
                  (ena_diag_transform && is_intra_scm && i_tile == j_tile) ? DiagonalTransformMode::MODE_0 : DiagonalTransformMode::NONE,
                  BinarizationMode::ROW_BINARIZATION
              }
          );
          scm_payload.SetTilePayload(i_tile, j_tile, std::move(cm_tile_payload));
      }
  }
}

} // namespace genie::contact::detail::xtensor
