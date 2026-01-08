/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "contact_coder_eigen.h"
#include <codecs/include/mpegg-codecs.h>
#include <genie/core/contact_record/record.h>
#include <genie/util/runtime_exception.h>
#include <cstdint>
#include <cstring>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <map>
#include "contact_matrix_parameters.h"
#include "contact_matrix_tile_payload.h"
#include "subcontact_matrix_parameters.h"
#include "subcontact_matrix_mask_payload.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::contact::detail::eigen {

// ---------------------------------------------------------------------------------------------------------------------

void set_rle_information_from_mask(
    RunLengthEncodingData& rleData,
    const Eigen::Matrix<bool, -1, 1>& scm_mask
) {
    if (scm_mask.size() == 0) {
        rleData.maxCount = 0;
        rleData.transformID = TransformID::ID_0;
        return;
    }

    rleData.firstVal = scm_mask(0);
    std::vector<uint32_t> rl_entries_vec;

    uint32_t current_count = 0;
    bool current_val = scm_mask(0);

    for (int i = 0; i < scm_mask.size(); ++i) {
        if (scm_mask(i) == current_val) {
            current_count++;
        } else {
            rl_entries_vec.push_back(current_count);
            current_val = scm_mask(i);
            current_count = 1;
        }
    }
    rl_entries_vec.push_back(current_count);

    rleData.rl_entries = Eigen::Map<Eigen::Matrix<uint32_t, -1, 1>>(rl_entries_vec.data(), static_cast<Eigen::Index>(rl_entries_vec.size()));

    if (rleData.rl_entries.size() > 0) {
        rleData.maxCount = rleData.rl_entries.maxCoeff();
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

void compute_mask(
    const Eigen::Matrix<uint64_t, -1, 1>& ids,
    size_t nelems,
    Eigen::Matrix<bool, -1, 1>& mask
){
    mask.setZero(static_cast<Eigen::Index>(nelems));
    if (ids.size() == 0) {
        return;
    }
    Eigen::Matrix<uint64_t, -1, 1> unique_ids = ids;
    std::sort(unique_ids.data(), unique_ids.data() + unique_ids.size());
    auto new_end = std::unique(unique_ids.data(), unique_ids.data() + unique_ids.size());
    unique_ids.conservativeResize(static_cast<Eigen::Index>(new_end - unique_ids.data()));
    for (int i = 0; i < unique_ids.size(); ++i){
        if (unique_ids(i) < nelems) {
            mask(static_cast<Eigen::Index>(unique_ids(i))) = true;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void compute_masks(
    Eigen::Matrix<uint64_t, -1, 1>& row_ids,
    Eigen::Matrix<uint64_t, -1, 1>& col_ids,
    size_t nrows,
    size_t ncols,
    const bool is_intra_scm,
    Eigen::Matrix<bool, -1, 1>& row_mask,
    Eigen::Matrix<bool, -1, 1>& col_mask
){
    UTILS_DIE_IF(row_ids.size() != col_ids.size(),
                 "The size of row_ids and col_ids must be same!");

    if (is_intra_scm){
        UTILS_DIE_IF(nrows != ncols,
            "Both nentries must be the same for intra SCM!"
        );

        Eigen::Matrix<bool, -1, 1> mask;
        Eigen::Matrix<uint64_t, -1, 1> combined_ids(row_ids.size() + col_ids.size());
        combined_ids << row_ids, col_ids;

        compute_mask(combined_ids, nrows, mask);

        row_mask = mask;
        col_mask = mask;
    } else {
        compute_mask(row_ids, nrows, row_mask);
        compute_mask(col_ids, ncols, col_mask);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_scm_masks(
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    const SubcontactMatrixPayload& scm_payload,
    Eigen::Matrix<bool, -1, 1>& row_mask,
    Eigen::Matrix<bool, -1, 1>& col_mask
){
    auto row_nentries = cm_param.GetNumBinEntries(scm_param.GetChr1ID());
    auto col_nentries = cm_param.GetNumBinEntries(scm_param.GetChr2ID());

    if (scm_param.GetRowMaskExistsFlag()){
        decode_scm_mask_payload(scm_payload.GetRowMaskPayload(), row_nentries, row_mask);
    } else {
        row_mask.setOnes(static_cast<Eigen::Index>(row_nentries));
    }

    if (scm_param.IsIntraSCM()){
        col_mask = row_mask;
    } else if (scm_param.GetColMaskExistsFlag()){
        decode_scm_mask_payload(scm_payload.GetColMaskPayload(), col_nentries, col_mask);
    } else {
        col_mask.setOnes(static_cast<Eigen::Index>(col_nentries));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_scm_mask_payload(
    const SubcontactMatrixMaskPayload& mask_payload,
    size_t num_entries,
    Eigen::Matrix<bool, -1, 1>& mask
) {
    auto transform_ID = mask_payload.GetTransformID();
    if (transform_ID == TransformID::ID_0){
        const auto& mask_array = mask_payload.GetMaskArray();
        mask.resize(static_cast<Eigen::Index>(num_entries));
        for(size_t i=0; i<num_entries; ++i) mask(static_cast<Eigen::Index>(i)) = mask_array[i];
        UTILS_DIE_IF(
            num_entries != static_cast<size_t>(mask.size()),
            "num_entries and the size of mask_array_ differ!"
        );
    } else {
        mask.setZero(static_cast<Eigen::Index>(num_entries));

        bool first_val = mask_payload.GetFirstVal();
        const auto& rl_entries = mask_payload.GetRlEntries();

        size_t start_idx = 0;
        for (const auto& rl_entry : rl_entries){
            size_t end_idx = start_idx + rl_entry;
            size_t len = std::min(end_idx, num_entries) - start_idx;
            mask.segment(static_cast<Eigen::Index>(start_idx), static_cast<Eigen::Index>(len)).setConstant(first_val);
            start_idx = end_idx;
            first_val = !first_val;
        }
        UTILS_DIE_IF(
            start_idx > num_entries,
            "start_idx value must be smaller than num_entries!"
        );
        if (start_idx < num_entries) {
            mask.segment(static_cast<Eigen::Index>(start_idx), static_cast<Eigen::Index>(num_entries - start_idx)).setConstant(first_val);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void remove_unaligned(
    Eigen::Matrix<uint64_t, -1, 1>& row_ids,
    Eigen::Matrix<uint64_t, -1, 1>& col_ids,
    bool is_intra_tile,
    const Eigen::Matrix<bool, -1, 1>& row_mask,
    const Eigen::Matrix<bool, -1, 1>& col_mask
){
    UTILS_DIE_IF(row_ids.size() != col_ids.size(),
                 "The size of row_ids and col_ids must be same!");

    if (is_intra_tile){
        UTILS_DIE_IF(row_mask.size() != col_mask.size() || (row_mask.array() != col_mask.array()).any(), "row_mask and col_mask are different!");

        auto num_entries = row_ids.size();
        auto& mask = row_mask;
        Eigen::Matrix<uint64_t, -1, 1> mapping(mask.size());
        uint64_t new_id = 0u;
        for (int i = 0; i < mask.size(); ++i){
            mapping(i) = new_id;
            if (mask(i)){
                new_id++;
            }
        }

        for (int i = 0; i < num_entries; i++){
            row_ids(i) = mapping(static_cast<Eigen::Index>(row_ids(i)));
            col_ids(i) = mapping(static_cast<Eigen::Index>(col_ids(i)));
        }
    } else {
        auto row_mapping_len = row_mask.size();
        Eigen::Matrix<uint64_t, -1, 1> row_mapping(row_mapping_len);
        {
            uint64_t new_id = 0u;
            for (int i = 0; i < row_mapping_len; ++i){
                row_mapping(i) = new_id;
                if (row_mask(i)){
                    new_id++;
                }
            }
        }
        auto num_entries = row_ids.size();
        for (int i = 0; i < num_entries; i++){
            row_ids(i) = row_mapping(static_cast<Eigen::Index>(row_ids(i)));
        }

        auto col_mapping_len = col_mask.size();
        Eigen::Matrix<uint64_t, -1, 1> col_mapping(col_mapping_len);
        {
            uint64_t new_id = 0u;
            for (int i = 0; i < col_mapping_len; ++i){
                col_mapping(i) = new_id;
                if (col_mask(i)){
                    new_id++;
                }
            }
        }
        num_entries = col_ids.size();
        for (int i = 0; i < num_entries; i++){
            col_ids(i) = col_mapping(static_cast<Eigen::Index>(col_ids(i)));
        }
    }
}
// ---------------------------------------------------------------------------------------------------------------------

void insert_unaligned(
    Eigen::Matrix<uint64_t, -1, 1>& row_ids,
    Eigen::Matrix<uint64_t, -1, 1>& col_ids,
    bool is_intra_tile,
    const Eigen::Matrix<bool, -1, 1>& row_mask,
    const Eigen::Matrix<bool, -1, 1>& col_mask
){
    UTILS_DIE_IF(row_ids.rows() != col_ids.rows(),
                 "The size of row_ids and col_ids must be same!");

    if (is_intra_tile){
        UTILS_DIE_IF(row_mask.rows() != col_mask.rows() || (row_mask.array() != col_mask.array()).any(), "row_mask and col_mask are different!");

        auto num_entries = row_ids.rows();
        auto& mask = row_mask;
        Eigen::Matrix<uint64_t, -1, 1> mapping(mask.size());
        uint64_t new_id = 0u;
        for (int i = 0; i < mask.size(); ++i) {
            if (mask(i)) {
                mapping(static_cast<Eigen::Index>(new_id++)) = static_cast<uint64_t>(i);
            }
        }
        for (int i = 0; i < num_entries; i++){
            UTILS_DIE_IF(row_ids(i) >= static_cast<uint64_t>(mapping.size()), "row_id out of mapping bounds");
            UTILS_DIE_IF(col_ids(i) >= static_cast<uint64_t>(mapping.size()), "col_id out of mapping bounds");
            row_ids(i) = mapping(static_cast<Eigen::Index>(row_ids(i)));
            col_ids(i) = mapping(static_cast<Eigen::Index>(col_ids(i)));
        }

    } else {
        {
            auto num_entries = row_ids.rows();
            Eigen::Matrix<uint64_t, -1, 1> mapping(row_mask.size());
            uint64_t new_id = 0u;
            for (int i = 0; i < row_mask.size(); ++i) {
                if (row_mask(i)) {
                    mapping(static_cast<Eigen::Index>(new_id++)) = static_cast<uint64_t>(i);
                }
            }
            mapping.conservativeResize(static_cast<Eigen::Index>(new_id));

            for (int i = 0; i < num_entries; i++){
                UTILS_DIE_IF(row_ids(i) >= static_cast<uint64_t>(mapping.size()), "row_id out of mapping bounds");
                row_ids(i) = mapping(static_cast<Eigen::Index>(row_ids(i)));
            }
        }

        {
            auto num_entries = col_ids.rows();
            Eigen::Matrix<uint64_t, -1, 1> mapping(col_mask.size());
            uint64_t new_id = 0u;
            for (int i = 0; i < col_mask.size(); ++i) {
                if (col_mask(i)) {
                    mapping(static_cast<Eigen::Index>(new_id++)) = static_cast<uint64_t>(i);
                }
            }
            mapping.conservativeResize(static_cast<Eigen::Index>(new_id));
            for (int i = 0; i < num_entries; i++){
                UTILS_DIE_IF(col_ids(i) >= static_cast<uint64_t>(mapping.size()), "col_id out of mapping bounds");
                col_ids(i) = mapping(static_cast<Eigen::Index>(col_ids(i)));
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void sparse_to_dense(
    const Eigen::Matrix<uint64_t, -1, 1>& row_ids,
    const Eigen::Matrix<uint64_t, -1, 1>& col_ids,
    const Eigen::Matrix<uint32_t, -1, 1>& counts,
    size_t nrows,
    size_t ncols,
    Eigen::Matrix<uint32_t, -1, -1, Eigen::RowMajor>& mat
){
    if (row_ids.size() > 0) {
        UTILS_DIE_IF(row_ids.maxCoeff() >= nrows, "Invalid nrows or row_ids!");
    }
    if (col_ids.size() > 0) {
        UTILS_DIE_IF(col_ids.maxCoeff() >= ncols, "Invalid ncols or col_ids!");
    }
    if (counts.size() > 0) {
        UTILS_DIE_IF((counts.array() == 0).any(), "Count with value 0 is found!");
    }
    mat.setZero(static_cast<Eigen::Index>(nrows), static_cast<Eigen::Index>(ncols));

    for (int i = 0; i < counts.size(); i++){
        mat(static_cast<Eigen::Index>(row_ids(i)), static_cast<Eigen::Index>(col_ids(i))) = counts(i);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void dense_to_sparse(
    const Eigen::Matrix<uint32_t, -1, -1, Eigen::RowMajor>& mat,
    Eigen::Matrix<uint64_t, -1, 1>& row_ids,
    Eigen::Matrix<uint64_t, -1, 1>& col_ids,
    Eigen::Matrix<uint32_t, -1, 1>& counts
){
    std::vector<uint64_t> row_ids_vec;
    std::vector<uint64_t> col_ids_vec;
    std::vector<uint32_t> counts_vec;
    for (int i = 0; i < mat.rows(); ++i) {
        for (int j = 0; j < mat.cols(); ++j) {
            if (mat(i, j) > 0) {
                row_ids_vec.push_back(static_cast<uint64_t>(i));
                col_ids_vec.push_back(static_cast<uint64_t>(j));
                counts_vec.push_back(mat(i, j));
            }
        }
    }
    row_ids = Eigen::Map<Eigen::Matrix<uint64_t, -1, 1>>(row_ids_vec.data(), static_cast<Eigen::Index>(row_ids_vec.size()));
    col_ids = Eigen::Map<Eigen::Matrix<uint64_t, -1, 1>>(col_ids_vec.data(), static_cast<Eigen::Index>(col_ids_vec.size()));
    counts = Eigen::Map<Eigen::Matrix<uint32_t, -1, 1>>(counts_vec.data(), static_cast<Eigen::Index>(counts_vec.size()));
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void sort_by_row_ids(
    Eigen::Matrix<uint64_t, -1, 1>& row_ids,
    Eigen::Matrix<uint64_t, -1, 1>& col_ids,
    Eigen::Matrix<uint32_t, -1, 1>& counts
){
    size_t num_entries = static_cast<size_t>(counts.size());
    assert(static_cast<size_t>(row_ids.size()) == num_entries);
    assert(static_cast<size_t>(col_ids.size()) == num_entries);

    std::vector<size_t> sort_indices(num_entries);
    std::iota(sort_indices.begin(), sort_indices.end(), 0);

    std::stable_sort(
        sort_indices.begin(),
        sort_indices.end(),
        [&](size_t i1, size_t i2) {
            if (row_ids(static_cast<Eigen::Index>(i1)) != row_ids(static_cast<Eigen::Index>(i2))) {
                return row_ids(static_cast<Eigen::Index>(i1)) < row_ids(static_cast<Eigen::Index>(i2));
            }
            return col_ids(static_cast<Eigen::Index>(i1)) < col_ids(static_cast<Eigen::Index>(i2));
        }
    );

    bool already_sorted = true;
    for(size_t i = 0; i < num_entries; ++i) {
        if(sort_indices[i] != i) {
            already_sorted = false;
            break;
        }
    }
    if (already_sorted) {
        return;
    }

    Eigen::Matrix<uint64_t, -1, 1> sorted_row_ids(num_entries);
    Eigen::Matrix<uint64_t, -1, 1> sorted_col_ids(num_entries);
    Eigen::Matrix<uint32_t, -1, 1> sorted_counts(num_entries);

    for (size_t k = 0u; k < num_entries; k++){
        sorted_row_ids(static_cast<Eigen::Index>(k)) = row_ids(static_cast<Eigen::Index>(sort_indices[k]));
        sorted_col_ids(static_cast<Eigen::Index>(k)) = col_ids(static_cast<Eigen::Index>(sort_indices[k]));
        sorted_counts(static_cast<Eigen::Index>(k)) = counts(static_cast<Eigen::Index>(sort_indices[k]));
    }

    row_ids = sorted_row_ids;
    col_ids = sorted_col_ids;
    counts = sorted_counts;
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_diag_transform(
    Eigen::Matrix<uint32_t, -1, -1, Eigen::RowMajor>& mat,
    DiagonalTransformMode mode
){
    if (mode == DiagonalTransformMode::NONE) {
        return;
    }

    auto nrows = mat.rows();
    if (nrows == 0) return;
    auto ncols = mat.cols();

    Eigen::Matrix<uint32_t, -1, -1, Eigen::RowMajor> trans_mat;

    if (mode == DiagonalTransformMode::MODE_0) {
        auto source_ncols = ncols;
        auto target_nrows = source_ncols;

        trans_mat.setZero(target_nrows, target_nrows);

        size_t o = 0u;
        for (int64_t k_diag = 0; k_diag < source_ncols; ++k_diag) {
            for (int64_t i_src = 0; i_src < (source_ncols - k_diag); ++i_src) {
                int64_t j_src = i_src + k_diag;

                if (o >= (size_t)nrows * ncols) {
                    goto end_mode_0_inverse_transform;
                }

                int64_t i_mat = static_cast<int64_t>(o / static_cast<size_t>(ncols));
                int64_t j_mat = static_cast<int64_t>(o % static_cast<size_t>(ncols));

                if (i_mat < nrows && j_mat < ncols) {
                    trans_mat(i_src, j_src) = mat(i_mat, j_mat);
                }
                o++;
            }
        }
        end_mode_0_inverse_transform:;
        mat = trans_mat;
    } else {
        trans_mat.setZero(nrows, ncols);

        std::vector<int64_t> diag_ids;
        if (mode == DiagonalTransformMode::MODE_1){
            diag_ids.push_back(0);
            auto ndiags = std::max(nrows, ncols);
            for (int64_t diag_id = 1; diag_id<ndiags; diag_id++){
                if (diag_id < ncols) diag_ids.push_back(diag_id);
                if (diag_id < nrows) diag_ids.push_back(-diag_id);
            }
        } else if (mode == DiagonalTransformMode::MODE_2){
            for (int64_t diag_id = -nrows+1; diag_id < ncols; diag_id++) diag_ids.push_back(diag_id);
        } else if (mode == DiagonalTransformMode::MODE_3){
            for (int64_t diag_id = ncols-1; diag_id > -nrows; diag_id--) diag_ids.push_back(diag_id);
        }

        int64_t i_orig, j_orig;
        int64_t i_offset, j_offset;
        int64_t nelems_in_diag;
        size_t o = 0u;
        for (auto diag_id : diag_ids){
            if (diag_id >= 0) {
                nelems_in_diag = std::max(nrows, ncols) - diag_id;
                i_offset = 0;
                j_offset = diag_id;
            } else {
                nelems_in_diag = std::max(nrows, ncols) + diag_id;
                i_offset = -diag_id;
                j_offset = 0;
            }
            for (int64_t k_diag = 0; k_diag<nelems_in_diag; k_diag++){
                i_orig = k_diag + i_offset;
                j_orig = k_diag + j_offset;
                if (i_orig >= nrows || j_orig >= ncols) break;

                size_t i_packed = o / static_cast<size_t>(ncols);
                size_t j_packed = o % static_cast<size_t>(ncols);
                if (i_packed < static_cast<size_t>(nrows)) {
                    trans_mat(i_orig, j_orig) = mat(static_cast<Eigen::Index>(i_packed), static_cast<Eigen::Index>(j_packed));
                }
                o++;
            }
        }
        mat = trans_mat;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void diag_transform(
    Eigen::Matrix<uint32_t, -1, -1, Eigen::RowMajor>& mat,
    DiagonalTransformMode mode
){
    if (mode == DiagonalTransformMode::NONE){
        return;
    }

    auto nrows = mat.rows();
    if (nrows == 0) return;
    auto ncols = mat.cols();

    Eigen::Matrix<uint32_t, -1, -1, Eigen::RowMajor> trans_mat;

    if (mode == DiagonalTransformMode::MODE_0) {
        UTILS_DIE_IF(
            nrows != ncols,
            "Matrix must be a square!"
        );

        auto new_nrows = nrows / 2 + 1;
        trans_mat.setZero(new_nrows, ncols);

        size_t o = 0u;
        for (int64_t k_diag = 0; k_diag < nrows; ++k_diag) {
            for (int64_t i = 0; i < (nrows - k_diag); ++i) {
                int64_t j = i + k_diag;

                auto v = mat(i, j);
                int64_t target_i = static_cast<int64_t>(o / static_cast<size_t>(ncols));
                int64_t target_j = static_cast<int64_t>(o % static_cast<size_t>(ncols));

                if (target_i < new_nrows && target_j < ncols) {
                    trans_mat(target_i, target_j) = v;
                }
                o++;
            }
        }
        mat = trans_mat;
    } else {
        trans_mat.setZero(nrows, ncols);

        std::vector<int64_t> diag_ids;
        if (mode == DiagonalTransformMode::MODE_1){
            diag_ids.push_back(0);
            auto ndiags = std::max(nrows, ncols);
            for (int64_t diag_id = 1; diag_id<ndiags; diag_id++){
                if (diag_id < ncols) diag_ids.push_back(diag_id);
                if (diag_id < nrows) diag_ids.push_back(-diag_id);
            }
        } else if (mode == DiagonalTransformMode::MODE_2){
            for (int64_t diag_id = -nrows+1; diag_id < ncols; diag_id++) diag_ids.push_back(diag_id);
        } else if (mode == DiagonalTransformMode::MODE_3){
            for (int64_t diag_id = ncols-1; diag_id > -nrows; diag_id--) diag_ids.push_back(diag_id);
        }

        int64_t i_orig, j_orig;
        int64_t i_offset, j_offset;
        int64_t nelems_in_diag;
        size_t o = 0u;
        for (auto diag_id : diag_ids){
            if (diag_id >= 0) {
                nelems_in_diag = std::max(nrows, ncols) - diag_id;
                i_offset = 0;
                j_offset = diag_id;
            } else {
                nelems_in_diag = std::max(nrows, ncols) + diag_id;
                i_offset = -diag_id;
                j_offset = 0;
            }
            for (int64_t k_diag = 0; k_diag<nelems_in_diag; k_diag++){
                i_orig = k_diag + i_offset;
                j_orig = k_diag + j_offset;
                if (i_orig >= nrows || j_orig >= ncols) break;

                auto v = mat(i_orig, j_orig);
                size_t i_packed = o / static_cast<size_t>(ncols);
                size_t j_packed = o % static_cast<size_t>(ncols);
                if (i_packed < static_cast<size_t>(nrows)) {
                    trans_mat(static_cast<Eigen::Index>(i_packed), static_cast<Eigen::Index>(j_packed)) = v;
                }
                o++;
            }
        }
        mat = trans_mat;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_row_bin(
    const Eigen::Matrix<bool, -1, -1, Eigen::RowMajor>& bin_mat,
    Eigen::Matrix<uint32_t, -1, -1, Eigen::RowMajor>& mat
){
    size_t bin_mat_nrows = static_cast<size_t>(bin_mat.rows());
    if (bin_mat_nrows == 0) return;
    size_t bin_mat_ncols = static_cast<size_t>(bin_mat.cols());

    UTILS_DIE_IF(bin_mat_nrows == 0, "Invalid bin_mat_nrows!");
    UTILS_DIE_IF(bin_mat_ncols == 0, "Invalid bin_mat_ncols!");

    size_t mat_ncols = bin_mat_ncols - 1;

    size_t mat_nrows = 0;
    for(int i = 0; i < bin_mat.rows(); ++i) {
        if(bin_mat(i, 0)) {
            mat_nrows++;
        }
    }

    UTILS_DIE_IF(mat_nrows == 0, "Invalid mat_nrows after transformation!");
    UTILS_DIE_IF(mat_ncols == 0, "Invalid mat_ncols after transformation!");

    mat.setZero(static_cast<Eigen::Index>(mat_nrows), static_cast<Eigen::Index>(mat_ncols));

    size_t target_i = 0;
    uint8_t bit_pos = 0;

    for (size_t i = 0; i < bin_mat_nrows; ++i){
        for (size_t j = 1; j < bin_mat_ncols; ++j) {
            if (bin_mat(static_cast<Eigen::Index>(i), static_cast<Eigen::Index>(j))) {
                mat(static_cast<Eigen::Index>(target_i), static_cast<Eigen::Index>(j-1)) |= (1u << bit_pos);
            }
        }

        if (bin_mat(static_cast<Eigen::Index>(i), 0)){
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
    const Eigen::Matrix<uint32_t, -1, -1, Eigen::RowMajor>& mat,
    Eigen::Matrix<bool, -1, -1, Eigen::RowMajor>& bin_mat
) {
    auto nrows = mat.rows();
    if (nrows == 0) return;
    auto ncols = mat.cols();

    Eigen::Matrix<uint8_t, -1, 1> nbits_per_row(nrows);
    for(int i=0; i<nrows; ++i) {
        uint32_t max_val = 0;
        if (mat.row(i).size() > 0) {
            max_val = mat.row(i).maxCoeff();
        }
        nbits_per_row(i) = (max_val == 0) ? 1 : static_cast<uint8_t>(std::ceil(std::log2(static_cast<double>(max_val) + 1.0)));
        if (nbits_per_row(i) == 0) nbits_per_row(i) = 1;
    }


    uint64_t bin_mat_nrows = static_cast<uint64_t>(nbits_per_row.cast<uint64_t>().sum());
    uint64_t bin_mat_ncols = static_cast<uint64_t>(ncols + 1);

    bin_mat.setZero(static_cast<Eigen::Index>(bin_mat_nrows), static_cast<Eigen::Index>(bin_mat_ncols));

    size_t current_bin_mat_row = 0;
    for (int i = 0; i < nrows; ++i) {
        auto bitlength = nbits_per_row(i);
        for (size_t i_bit = 0; i_bit < bitlength; ++i_bit) {
            for(int j=0; j<ncols; ++j) {
                if(mat(i,j) & (1u << i_bit)) {
                    bin_mat(static_cast<Eigen::Index>(current_bin_mat_row), static_cast<Eigen::Index>(j+1)) = true;
                }
            }
            current_bin_mat_row++;
        }
        if (current_bin_mat_row > 0) {
            bin_mat(static_cast<Eigen::Index>(current_bin_mat_row - 1), 0) = true;
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

void bin_mat_to_bytes(
    const Eigen::Matrix<bool, -1, -1, Eigen::RowMajor>& bin_mat,
    uint8_t** payload,
    size_t& payload_len
) {
    auto nrows = static_cast<size_t>(bin_mat.rows());
    auto ncols = static_cast<size_t>(bin_mat.cols());

    auto bpl = (ncols >> 3u) + ((ncols & 7u) > 0u);  // Ceil div operation
    payload_len = bpl * nrows;
    
    *payload = (unsigned char*) calloc (payload_len, sizeof(unsigned char));
    if (*payload == nullptr && payload_len > 0) {
        UTILS_DIE("calloc failed in bin_mat_to_bytes");
    }

    for (size_t i = 0u; i < nrows; i++) {
        size_t row_offset = i * bpl;
        for (size_t j = 0u; j < ncols; j++) {
            auto byte_offset = row_offset + (j >> 3u);
            uint8_t shift = static_cast<uint8_t>(7u - (j & 7u));
            
            auto val = static_cast<uint8_t>(bin_mat(static_cast<Eigen::Index>(i), static_cast<Eigen::Index>(j)));
            val = static_cast<uint8_t>(val << shift);
            *(*payload + byte_offset) |= val;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void bin_mat_from_bytes(
    const uint8_t* payload,
    size_t payload_len,
    size_t nrows,
    size_t ncols,
    Eigen::Matrix<bool, -1, -1, Eigen::RowMajor>& bin_mat
) {
    auto bpl = (ncols >> 3u) + ((ncols & 7u) > 0u);  // bytes per line with ceil operation
    UTILS_DIE_IF(payload_len != static_cast<size_t>(nrows * bpl), "Invalid payload_len / nrows / ncols!");

    bin_mat.setZero(static_cast<Eigen::Index>(nrows), static_cast<Eigen::Index>(ncols));

    for (size_t i = 0u; i < nrows; i++) {
        size_t row_offset = i * bpl;
        for (size_t j = 0u; j < ncols; j++) {
            auto byte_offset = row_offset + (j >> 3u);
            uint8_t shift = static_cast<uint8_t>(7u - (j & 7u));
            bin_mat(static_cast<Eigen::Index>(i), static_cast<Eigen::Index>(j)) = (*(payload + byte_offset) >> shift) & 1u;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_cm_tile(
    const genie::contact::ContactMatrixTilePayload& tile_payload,
    core::AlgoID codec_ID,
    Eigen::Matrix<bool, -1, -1, Eigen::RowMajor>& bin_mat
){
    uint8_t* raw_data = nullptr;
    size_t raw_data_len = 0;
    uint8_t* compressed_data = nullptr;
    size_t compressed_data_len = 0;

    unsigned long tile_nrows = 0;
    unsigned long tile_ncols = 0;

    if (codec_ID == core::AlgoID::JBIG){
        compressed_data_len = tile_payload.GetPayloadSize();
        auto& payload = tile_payload.GetPayload(); // This is std::vector<uint8_t>

        compressed_data = (uint8_t*)malloc(compressed_data_len * sizeof(uint8_t));
        if (compressed_data == nullptr) {
            UTILS_DIE("Failed to allocate memory for compressed_data");
        }
        memcpy(compressed_data, payload.data(), compressed_data_len);

        int status = mpegg_jbig_decompress_default(
            &raw_data,
            &raw_data_len,
            compressed_data,
            compressed_data_len,
            &tile_nrows,
            &tile_ncols
        );

        free(compressed_data);

        if (status != 0 || raw_data == nullptr) {
            if (raw_data) free(raw_data);
            UTILS_DIE("JBIG decompression failed");
        }

        bin_mat_from_bytes(
            raw_data,
            raw_data_len,
            static_cast<size_t>(tile_nrows),
            static_cast<size_t>(tile_ncols),
            bin_mat
        );

        free(raw_data);

    } else {
        UTILS_DIE("Not yet implemented");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_cm_tile(
    const Eigen::Matrix<bool, -1, -1, Eigen::RowMajor>& bin_mat,
    const core::AlgoID codec_ID,
    genie::contact::ContactMatrixTilePayload& tile_payload
) {
    uint8_t* payload = nullptr;
    size_t payload_len = 0;
    uint8_t* compressed_payload = nullptr;
    size_t compressed_payload_len = 0;

    auto tile_nrows = static_cast<uint32_t>(bin_mat.rows());
    auto tile_ncols = static_cast<uint32_t>(bin_mat.cols());

    if (codec_ID == genie::core::AlgoID::JBIG) {

        bin_mat_to_bytes(bin_mat, &payload, payload_len);

        int status = mpegg_jbig_compress_default(
            &compressed_payload,
            &compressed_payload_len,
            payload,
            payload_len,
            tile_nrows,
            tile_ncols
        );

        free(payload);

        if (status != 0 || compressed_payload == nullptr) {
            if (compressed_payload) free(compressed_payload);
            UTILS_DIE("JBIG compression failed");
        }

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
    UInt64VecDtype& tile_row_ids,
    UInt64VecDtype& tile_col_ids,
    UIntVecDtype& tile_counts,
    uint32_t bin_size_mult,
    bool sort_output
){
    size_t num_entries = static_cast<size_t>(tile_counts.size());
    if (num_entries == 0) return;

    std::map<std::pair<uint64_t, uint64_t>, uint32_t> lr_sparse_tile_map;
    for (int i = 0; i < tile_counts.size(); i++){
        auto lr_row_id = tile_row_ids(i) / bin_size_mult;
        auto lr_col_id = tile_col_ids(i) / bin_size_mult;
        auto count = tile_counts(i);

        lr_sparse_tile_map[{lr_row_id, lr_col_id}] += count;
    }

    size_t lr_num_entries = lr_sparse_tile_map.size();
    tile_row_ids.resize(static_cast<Eigen::Index>(lr_num_entries));
    tile_col_ids.resize(static_cast<Eigen::Index>(lr_num_entries));
    tile_counts.resize(static_cast<Eigen::Index>(lr_num_entries));

    size_t i_entry = 0u;
    for (const auto & pair_entry : lr_sparse_tile_map){
        tile_row_ids(static_cast<Eigen::Index>(i_entry)) = (pair_entry.first).first;
        tile_col_ids(static_cast<Eigen::Index>(i_entry)) = (pair_entry.first).second;
        tile_counts(static_cast<Eigen::Index>(i_entry)) = pair_entry.second;
        i_entry++;
    }

    if (sort_output){
        sort_sparse_mat_inplace(tile_row_ids, tile_col_ids, tile_counts);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void sort_sparse_mat_inplace(
    Eigen::Matrix<uint64_t, -1, 1>& tile_row_ids,
    Eigen::Matrix<uint64_t, -1, 1>& tile_col_ids,
    Eigen::Matrix<uint32_t, -1, 1>& tile_counts
){
    size_t num_entries = static_cast<size_t>(tile_counts.size());

    assert(static_cast<size_t>(tile_row_ids.size()) == num_entries);
    assert(static_cast<size_t>(tile_col_ids.size()) == num_entries);

    std::vector<size_t> sort_indices(num_entries);
    std::iota(sort_indices.begin(), sort_indices.end(), 0);

    std::stable_sort(
        sort_indices.begin(),
        sort_indices.end(),
        [&](size_t i1, size_t i2) {
            if (tile_row_ids(static_cast<Eigen::Index>(i1)) != tile_row_ids(static_cast<Eigen::Index>(i2))) {
                return tile_row_ids(static_cast<Eigen::Index>(i1)) < tile_row_ids(static_cast<Eigen::Index>(i2));
            }
            return tile_col_ids(static_cast<Eigen::Index>(i1)) < tile_col_ids(static_cast<Eigen::Index>(i2));
        }
    );

    bool already_sorted = true;
    for(size_t i = 0; i < num_entries; ++i) {
        if(sort_indices[i] != i) {
            already_sorted = false;
            break;
        }
    }
    if (already_sorted) {
        return;
    }

    Eigen::Matrix<uint64_t, -1, 1> sorted_row_ids(num_entries);
    Eigen::Matrix<uint64_t, -1, 1> sorted_col_ids(num_entries);
    Eigen::Matrix<uint32_t, -1, 1> sorted_counts(num_entries);

    for (size_t k = 0u; k < num_entries; k++){
        sorted_row_ids(static_cast<Eigen::Index>(k)) = tile_row_ids(static_cast<Eigen::Index>(sort_indices[k]));
        sorted_col_ids(static_cast<Eigen::Index>(k)) = tile_col_ids(static_cast<Eigen::Index>(sort_indices[k]));
        sorted_counts(static_cast<Eigen::Index>(k)) = tile_counts(static_cast<Eigen::Index>(sort_indices[k]));
    }

    tile_row_ids = sorted_row_ids;
    tile_col_ids = sorted_col_ids;
    tile_counts = sorted_counts;
}

// ---------------------------------------------------------------------------------------------------------------------

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

  Eigen::Matrix<bool, -1, 1> row_mask;
  Eigen::Matrix<bool, -1, 1> col_mask;

  auto interval = cm_param.GetBinSize();
  auto tile_size = cm_param.GetTileSize();
  auto chr1_ID = rec.GetChr1ID();
  auto chr1_num_bin_entries = cm_param.GetNumBinEntries(chr1_ID);
  auto ntiles_in_row = cm_param.GetNumTiles(chr1_ID);
  auto chr2_ID = rec.GetChr2ID();
  auto chr2_num_bin_entries = cm_param.GetNumBinEntries(chr2_ID);
  auto ntiles_in_col = cm_param.GetNumTiles(chr2_ID);

  cm_param.UpsertSample(rec.GetSampleID(), std::string(rec.GetSampleName()));
  scm_payload.SetSampleID(rec.GetSampleID());

  scm_param.SetChr1ID(chr1_ID);
  scm_payload.SetChr1ID(chr1_ID);

  scm_param.SetChr2ID(chr2_ID);
  scm_payload.SetChr2ID(chr2_ID);

  auto is_intra_scm = scm_param.IsIntraSCM();

  scm_param.SetCodecID(codec_ID);

  scm_param.SetNumTiles(ntiles_in_row, ntiles_in_col);
  scm_payload.SetNumTiles(ntiles_in_row, ntiles_in_col);

  // Convert std::vector from record to Eigen::Matrix
  const auto& rec_row_ids = rec.GetStartPos1();
  const auto& rec_col_ids = rec.GetStartPos2();
  
  Eigen::Matrix<uint64_t, -1, 1> row_ids = Eigen::Map<const Eigen::Matrix<uint64_t, -1, 1>>(rec_row_ids.data(), static_cast<Eigen::Index>(rec_row_ids.size()));
  for(int i = 0; i < row_ids.size(); ++i) row_ids(i) /= interval;

  Eigen::Matrix<uint64_t, -1, 1> col_ids = Eigen::Map<const Eigen::Matrix<uint64_t, -1, 1>>(rec_col_ids.data(), static_cast<Eigen::Index>(rec_col_ids.size()));
  for(int i = 0; i < col_ids.size(); ++i) col_ids(i) /= interval;

  const auto& rec_counts = rec.GetCounts();
  Eigen::Matrix<uint32_t, -1, 1> counts = Eigen::Map<const Eigen::Matrix<uint32_t, -1, 1>>(rec_counts.data(), static_cast<Eigen::Index>(rec_counts.size()));

  if (remove_unaligned_region){
      compute_masks(
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
          set_rle_information_from_mask(rowRLEData, row_mask);
          
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
            set_rle_information_from_mask(colRLEData, col_mask);
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

      remove_unaligned(
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
          comp_start_end_ids(chr1_num_bin_entries, tile_size, i_tile, start1_idx, end1_idx);
          comp_start_end_ids(chr2_num_bin_entries, tile_size, j_tile, start2_idx, end2_idx);

          bool is_intra_tile = is_intra_scm && (i_tile == j_tile);

          // Filtering operation
          std::vector<int> filtered_indices_vec;
          for(int i = 0; i < row_ids.size(); ++i) {
              if (row_ids(i) >= start1_idx && row_ids(i) < end1_idx &&
                  col_ids(i) >= start2_idx && col_ids(i) < end2_idx) {
                  filtered_indices_vec.push_back(i);
              }
          }

          if (filtered_indices_vec.empty()) { // No entries in this tile
              // Create an empty tile payload if no entries, will be handled by decoder
              scm_payload.SetTilePayload(
                  i_tile,
                  j_tile,
                  ContactMatrixTilePayload(codec_ID, 0, 0, std::vector<uint8_t>{}) // Empty payload
              );
              scm_param.SetTileParameter(
                  i_tile,
                  j_tile,
                  TileParameter{DiagonalTransformMode::NONE, BinarizationMode::ROW_BINARIZATION}
              );
              continue; // Move to next tile
          }
          
          size_t num_filtered_entries = filtered_indices_vec.size();
          
          Eigen::Matrix<uint64_t, -1, 1> tile_row_ids(num_filtered_entries);
          Eigen::Matrix<uint64_t, -1, 1> tile_col_ids(num_filtered_entries);
          Eigen::Matrix<uint32_t, -1, 1> tile_counts(num_filtered_entries);

          for (size_t k = 0; k < num_filtered_entries; ++k) {
              int original_idx = filtered_indices_vec[k];
              tile_row_ids(static_cast<Eigen::Index>(k)) = row_ids(original_idx) - start1_idx;
              tile_col_ids(static_cast<Eigen::Index>(k)) = col_ids(original_idx) - start2_idx;
              tile_counts(static_cast<Eigen::Index>(k)) = counts(original_idx);
          }

          // Sort the filtered entries if needed (important for some downstream processing)
          sort_sparse_mat_inplace(tile_row_ids, tile_col_ids, tile_counts); // Re-sort locally within tile if needed

          Eigen::Matrix<uint32_t, -1, -1, Eigen::RowMajor> tile_mat;
          sparse_to_dense(
              tile_row_ids,
              tile_col_ids,
              tile_counts,
              tile_size, // NROWS for sparse_to_dense is tile_size
              tile_size, // NCOLS for sparse_to_dense is tile_size
              tile_mat
          );

          if (ena_diag_transform && is_intra_tile) {
              diag_transform(tile_mat, DiagonalTransformMode::MODE_0); // Using MODE_0 for simplicity as in original
          }
          
          Eigen::Matrix<bool, -1, -1, Eigen::RowMajor> bin_mat;
          if (ena_binarization) { // Always true due to assert, but good practice
              transform_row_bin(tile_mat, bin_mat);
          } else {
              UTILS_DIE("Non-binarized encoding not supported for Eigen!");
          }

          genie::contact::ContactMatrixTilePayload cm_tile_payload;
          encode_cm_tile(bin_mat, codec_ID, cm_tile_payload);

          // Store tile parameters
          scm_param.SetTileParameter(
              i_tile,
              j_tile,
              TileParameter{
                  (ena_diag_transform && is_intra_tile) ? DiagonalTransformMode::MODE_0 : DiagonalTransformMode::NONE,
                  BinarizationMode::ROW_BINARIZATION // Assuming this is always used for now
              }
          );
          // Store tile payload
          scm_payload.SetTilePayload(i_tile, j_tile, std::move(cm_tile_payload));
      }
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
    auto codec_ID = scm_param.GetCodecID();
    auto interval = cm_param.GetBinSize();
    auto tile_size = cm_param.GetTileSize();
    auto chr1_ID = scm_param.GetChr1ID();
    auto chr1_num_bin_entries = cm_param.GetNumBinEntries(chr1_ID);
    auto ntiles_in_row = cm_param.GetNumTiles(chr1_ID);
    auto chr2_ID = scm_param.GetChr2ID();
    auto chr2_num_bin_entries = cm_param.GetNumBinEntries(chr2_ID);
    auto ntiles_in_col = cm_param.GetNumTiles(chr2_ID);

    rec.SetChr1ID(chr1_ID);
    const auto& chr1_info = cm_param.GetChromosomes().at(chr1_ID);
    rec.SetChr1Name(std::string(chr1_info.name));
    rec.SetChr1Length(chr1_info.length);

    rec.SetChr2ID(chr2_ID);
    const auto& chr2_info = cm_param.GetChromosomes().at(chr2_ID);
    rec.SetChr2Name(std::string(chr2_info.name));
    rec.SetChr2Length(chr2_info.length);

    rec.SetSampleId(scm_payload.GetSampleID());
    rec.SetSampleName(std::string(cm_param.GetSampleName(scm_payload.GetSampleID())));

    rec.SetBinSize(interval);

    auto is_intra_scm = scm_param.IsIntraSCM();

    Eigen::Matrix<bool, -1, 1> row_mask;
    Eigen::Matrix<bool, -1, 1> col_mask;

    decode_scm_masks(cm_param, scm_param, scm_payload, row_mask, col_mask);

    std::vector<uint64_t> aligned_row_ids;
    std::vector<uint64_t> aligned_col_ids;
    std::vector<uint32_t> aligned_counts;

    for (size_t i_tile = 0u; i_tile < ntiles_in_row; i_tile++){
        for (size_t j_tile = 0u; j_tile < ntiles_in_col; j_tile++){
            if (i_tile > j_tile && is_intra_scm){
                continue;
            }

            size_t start1_idx, end1_idx, start2_idx, end2_idx;
            comp_start_end_ids(chr1_num_bin_entries, tile_size, i_tile, start1_idx, end1_idx);
            comp_start_end_ids(chr2_num_bin_entries, tile_size, j_tile, start2_idx, end2_idx);

            bool is_intra_tile = is_intra_scm && (i_tile == j_tile);

            auto& tile_payload = scm_payload.GetTilePayload(i_tile, j_tile);
            if (tile_payload.GetPayloadSize() == 0) {
                continue;
            }

            Eigen::Matrix<bool, -1, -1, Eigen::RowMajor> bin_mat;
            decode_cm_tile(tile_payload, codec_ID, bin_mat);

            Eigen::Matrix<uint32_t, -1, -1, Eigen::RowMajor> tile_mat;
            inverse_transform_row_bin(bin_mat, tile_mat);

            auto tile_param = scm_param.GetTileParameter(i_tile, j_tile);
            inverse_diag_transform(tile_mat, tile_param.diag_tranform_mode);

            Eigen::Matrix<uint64_t, -1, 1> tile_row_ids;
            Eigen::Matrix<uint64_t, -1, 1> tile_col_ids;
            Eigen::Matrix<uint32_t, -1, 1> tile_counts;

            dense_to_sparse(tile_mat, tile_row_ids, tile_col_ids, tile_counts);

            for(int i=0; i<tile_row_ids.size(); ++i) {
                aligned_row_ids.push_back(tile_row_ids(i) + start1_idx);
                aligned_col_ids.push_back(tile_col_ids(i) + start2_idx);
                aligned_counts.push_back(tile_counts(i));
            }
        }
    }

    Eigen::Matrix<uint64_t, -1, 1> row_ids = Eigen::Map<Eigen::Matrix<uint64_t, -1, 1>>(aligned_row_ids.data(), static_cast<Eigen::Index>(aligned_row_ids.size()));
    Eigen::Matrix<uint64_t, -1, 1> col_ids = Eigen::Map<Eigen::Matrix<uint64_t, -1, 1>>(aligned_col_ids.data(), static_cast<Eigen::Index>(aligned_col_ids.size()));
    Eigen::Matrix<uint32_t, -1, 1> counts = Eigen::Map<Eigen::Matrix<uint32_t, -1, 1>>(aligned_counts.data(), static_cast<Eigen::Index>(aligned_counts.size()));

    if (scm_param.GetRowMaskExistsFlag()){
        insert_unaligned(row_ids, col_ids, is_intra_scm, row_mask, col_mask);
    }

    if (bin_size_mult != 1){
        conv_noop_on_sparse_mat(row_ids, col_ids, counts, bin_size_mult, true);
    }

    auto target_bin_size = interval * bin_size_mult;
    std::vector<uint64_t> start1(row_ids.size());
    std::vector<uint64_t> end1(row_ids.size());
    std::vector<uint64_t> start2(col_ids.size());
    std::vector<uint64_t> end2(col_ids.size());
    std::vector<uint32_t> final_counts(counts.size());

    for(int i=0; i<row_ids.size(); ++i) {
        start1[i] = row_ids(i) * target_bin_size;
        end1[i] = std::min(start1[i] + target_bin_size, chr1_info.length);
        start2[i] = col_ids(i) * target_bin_size;
        end2[i] = std::min(start2[i] + target_bin_size, chr2_info.length);
        final_counts[i] = counts(i);
    }

    rec.SetCMValues(std::move(start1), std::move(end1), std::move(start2), std::move(end2), std::move(final_counts));
}

} // namespace genie::contact::detail::eigen
