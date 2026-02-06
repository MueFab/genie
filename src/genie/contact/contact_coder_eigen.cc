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

using EigenBinVec = Eigen::Matrix<bool, Eigen::Dynamic, 1>;
using EigenBinMat = Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using EigenUIntMat = Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using EigenUInt64Vec = Eigen::Matrix<uint64_t, Eigen::Dynamic, 1>;
using EigenUIntVec = Eigen::Matrix<uint32_t, Eigen::Dynamic, 1>;

// ---------------------------------------------------------------------------------------------------------------------

void set_rle_information_from_mask(
    RunLengthEncodingData& rleData,
    const BinVecDtype& scm_mask
) {
    if (scm_mask.empty()) {
        rleData.maxCount = 0;
        rleData.transformID = TransformID::ID_0;
        return;
    }

    rleData.firstVal = scm_mask[0];
    std::vector<uint32_t> rl_entries_vec;

    uint32_t current_count = 0;
    bool current_val = scm_mask[0];

    for (size_t i = 0; i < scm_mask.size(); ++i) {
        if (scm_mask[i] == current_val) {
            current_count++;
        } else {
            rl_entries_vec.push_back(current_count);
            current_val = scm_mask[i];
            current_count = 1;
        }
    }
    rl_entries_vec.push_back(current_count);

    rleData.rl_entries = std::move(rl_entries_vec);

    if (rleData.rl_entries.size() > 0) {
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

void compute_mask(
    UInt64VecDtype& ids,
    size_t nelems,
    BinVecDtype& mask
){
    mask.assign(nelems, false);
    if (ids.empty()) {
        return;
    }
    std::vector<uint64_t> unique_ids = ids;
    std::sort(unique_ids.begin(), unique_ids.end());
    unique_ids.erase(std::unique(unique_ids.begin(), unique_ids.end()), unique_ids.end());
    for (auto id : unique_ids){
        if (id < nelems) {
            mask[id] = true;
        }
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
    UTILS_DIE_IF(row_ids.size() != col_ids.size(),
                 "The size of row_ids and col_ids must be same!");

    if (is_intra_scm){
        UTILS_DIE_IF(nrows != ncols,
            "Both nentries must be the same for intra SCM!"
        );

        BinVecDtype mask;
        std::vector<uint64_t> combined_ids;
        combined_ids.reserve(row_ids.size() + col_ids.size());
        combined_ids.insert(combined_ids.end(), row_ids.begin(), row_ids.end());
        combined_ids.insert(combined_ids.end(), col_ids.begin(), col_ids.end());

        detail::eigen::compute_mask(combined_ids, nrows, mask);

        row_mask = mask;
        col_mask = mask;
    } else {
        detail::eigen::compute_mask(row_ids, nrows, row_mask);
        detail::eigen::compute_mask(col_ids, ncols, col_mask);
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
        detail::eigen::decode_scm_mask_payload(scm_payload.GetRowMaskPayload(), row_nentries, row_mask);
    } else {
        row_mask.assign(row_nentries, true);
    }

    if (scm_param.IsIntraSCM()){
        col_mask = row_mask;
    } else if (scm_param.GetColMaskExistsFlag()){
        detail::eigen::decode_scm_mask_payload(scm_payload.GetColMaskPayload(), col_nentries, col_mask);
    } else {
        col_mask.assign(col_nentries, true);
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
        mask = mask_payload.GetMaskArray();
        UTILS_DIE_IF(
            num_entries != mask.size(),
            "num_entries and the size of mask_array_ differ!"
        );
    } else {
        mask.assign(num_entries, false);

        bool first_val = mask_payload.GetFirstVal();
        const auto& rl_entries = mask_payload.GetRlEntries();

        size_t start_idx = 0;
        for (const auto& rl_entry : rl_entries){
            size_t end_idx = start_idx + rl_entry;
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
        UTILS_DIE_IF(row_mask != col_mask, "row_mask and col_mask are different!");

        auto num_entries = row_ids.size();
        auto mapping_len = row_mask.size();
        std::vector<uint64_t> mapping(mapping_len);
        uint64_t new_id = 0u;
        for (size_t i = 0; i < mapping_len; ++i){
            mapping[i] = new_id;
            if (row_mask[i]){
                new_id++;
            }
        }

        for (size_t i = 0; i < num_entries; i++){
            row_ids[i] = mapping[row_ids[i]];
            col_ids[i] = mapping[col_ids[i]];
        }
    } else {
        auto row_mapping_len = row_mask.size();
        std::vector<uint64_t> row_mapping(row_mapping_len);
        {
            uint64_t new_id = 0u;
            for (size_t i = 0; i < row_mapping_len; ++i){
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
            for (size_t i = 0; i < col_mapping_len; ++i){
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
    const BinVecDtype& row_mask,
    const BinVecDtype& col_mask
){
    UTILS_DIE_IF(row_ids.size() != col_ids.size(),
                 "The size of row_ids and col_ids must be same!");

    if (is_intra_tile){
        UTILS_DIE_IF(row_mask != col_mask, "row_mask and col_mask are different!");

        std::vector<uint64_t> mapping;
        for (size_t i = 0; i < row_mask.size(); ++i) if (row_mask[i]) mapping.push_back(i);

        for (size_t i = 0; i < row_ids.size(); i++){
            row_ids[i] = mapping[row_ids[i]];
            col_ids[i] = mapping[col_ids[i]];
        }

    } else {
        {
            std::vector<uint64_t> mapping;
            for (size_t i = 0; i < row_mask.size(); ++i) if (row_mask[i]) mapping.push_back(i);
            for (size_t i = 0; i < row_ids.size(); i++){
                row_ids[i] = mapping[row_ids[i]];
            }
        }

        {
            std::vector<uint64_t> mapping;
            for (size_t i = 0; i < col_mask.size(); ++i) if (col_mask[i]) mapping.push_back(i);
            for (size_t i = 0; i < col_ids.size(); i++){
                col_ids[i] = mapping[col_ids[i]];
            }
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
    mat.assign(nrows, std::vector<uint32_t>(ncols, 0));
    for (size_t i = 0; i < counts.size(); i++){
        mat[row_ids[i]][col_ids[i]] = counts[i];
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void dense_to_sparse(
    const UIntMatDtype& mat,
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
    size_t num_entries = counts.size();
    std::vector<size_t> p(num_entries);
    std::iota(p.begin(), p.end(), 0);
    std::stable_sort(p.begin(), p.end(),
                     [&](size_t i, size_t j) {
                         if (row_ids[i] != row_ids[j]) return row_ids[i] < row_ids[j];
                         return col_ids[i] < col_ids[j];
                     });

    UInt64VecDtype s_row_ids(num_entries);
    UInt64VecDtype s_col_ids(num_entries);
    UIntVecDtype s_counts(num_entries);
    for (size_t i = 0; i < num_entries; ++i) {
        s_row_ids[i] = row_ids[p[i]];
        s_col_ids[i] = col_ids[p[i]];
        s_counts[i] = counts[p[i]];
    }
    row_ids = std::move(s_row_ids);
    col_ids = std::move(s_col_ids);
    counts = std::move(s_counts);
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_diag_transform(
    UIntMatDtype& mat,
    DiagonalTransformMode mode
){
    if (mode == DiagonalTransformMode::NONE) return;

    auto nrows = mat.size();
    if (nrows == 0) return;
    auto ncols = mat[0].size();

    EigenUIntMat xt_mat(nrows, ncols);
    for (size_t i=0; i<nrows; ++i) for (size_t j=0; j<ncols; ++j) xt_mat(i, j) = mat[i][j];

    if (mode == DiagonalTransformMode::MODE_0) {
        auto source_ncols = ncols;
        auto target_nrows = source_ncols;
        EigenUIntMat trans_mat = EigenUIntMat::Zero(target_nrows, target_nrows);

        size_t o = 0u;
        for (int64_t k_diag = 0; k_diag < (int64_t)source_ncols; ++k_diag) {
            for (int64_t i_src = 0; i_src < ((int64_t)source_ncols - k_diag); ++i_src) {
                int64_t j_src = i_src + k_diag;
                if (o >= (size_t)nrows * ncols) goto end_mode_0_inverse_transform;
                int64_t i_mat = static_cast<int64_t>(o / ncols);
                int64_t j_mat = static_cast<int64_t>(o % ncols);
                if (i_mat < (int64_t)nrows && j_mat < (int64_t)ncols) trans_mat(i_src, j_src) = xt_mat(i_mat, j_mat);
                o++;
            }
        }
        end_mode_0_inverse_transform:;
        mat.assign(trans_mat.rows(), std::vector<uint32_t>(trans_mat.cols()));
        for(int i=0; i<trans_mat.rows(); ++i) for(int j=0; j<trans_mat.cols(); ++j) mat[i][j] = trans_mat(i, j);
    } else {
        EigenUIntMat trans_mat = EigenUIntMat::Zero(nrows, ncols);
        std::vector<int64_t> diag_ids;
        if (mode == DiagonalTransformMode::MODE_1){
            diag_ids.push_back(0);
            auto ndiags = std::max(nrows, ncols);
            for (int64_t diag_id = 1; diag_id<(int64_t)ndiags; diag_id++){
                if (diag_id < (int64_t)ncols) diag_ids.push_back(diag_id);
                if (diag_id < (int64_t)nrows) diag_ids.push_back(-diag_id);
            }
        } else if (mode == DiagonalTransformMode::MODE_2){
            for (int64_t diag_id = -(int64_t)nrows+1; diag_id < (int64_t)ncols; diag_id++) diag_ids.push_back(diag_id);
        } else if (mode == DiagonalTransformMode::MODE_3){
            for (int64_t diag_id = (int64_t)ncols-1; diag_id > -(int64_t)nrows; diag_id--) diag_ids.push_back(diag_id);
        }

        int64_t i_orig, j_orig, i_offset, j_offset, nelems_in_diag;
        size_t o = 0u;
        for (auto diag_id : diag_ids){
            if (diag_id >= 0) {
                nelems_in_diag = std::max(nrows, ncols) - diag_id;
                i_offset = 0; j_offset = diag_id;
            } else {
                nelems_in_diag = std::max(nrows, ncols) + diag_id;
                i_offset = -diag_id; j_offset = 0;
            }
            for (int64_t k_diag = 0; k_diag<nelems_in_diag; k_diag++){
                i_orig = k_diag + i_offset; j_orig = k_diag + j_offset;
                if (i_orig >= (int64_t)nrows || j_orig >= (int64_t)ncols) break;
                size_t i_packed = o / ncols; size_t j_packed = o % ncols;
                if (i_packed < nrows) trans_mat(i_orig, j_orig) = xt_mat((int)i_packed, (int)j_packed);
                o++;
            }
        }
        mat.assign(trans_mat.rows(), std::vector<uint32_t>(trans_mat.cols()));
        for(int i=0; i<trans_mat.rows(); ++i) for(int j=0; j<trans_mat.cols(); ++j) mat[i][j] = trans_mat(i, j);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void diag_transform(
    UIntMatDtype& mat,
    DiagonalTransformMode mode
){
    if (mode == DiagonalTransformMode::NONE) return;

    auto nrows = mat.size();
    if (nrows == 0) return;
    auto ncols = mat[0].size();

    EigenUIntMat xt_mat(nrows, ncols);
    for (size_t i=0; i<nrows; ++i) for (size_t j=0; j<ncols; ++j) xt_mat(i, j) = mat[i][j];

    if (mode == DiagonalTransformMode::MODE_0) {
        UTILS_DIE_IF(nrows != ncols, "Matrix must be a square!");
        auto new_nrows = nrows / 2 + 1;
        EigenUIntMat trans_mat = EigenUIntMat::Zero(new_nrows, ncols);

        size_t o = 0u;
        for (int64_t k_diag = 0; k_diag < (int64_t)nrows; ++k_diag) {
            for (int64_t i = 0; i < ((int64_t)nrows - k_diag); ++i) {
                int64_t j = i + k_diag;
                auto v = xt_mat(i, j);
                int64_t target_i = static_cast<int64_t>(o / ncols);
                int64_t target_j = static_cast<int64_t>(o % ncols);
                if (target_i < (int64_t)new_nrows && target_j < (int64_t)ncols) trans_mat(target_i, target_j) = v;
                o++;
            }
        }
        mat.assign(trans_mat.rows(), std::vector<uint32_t>(trans_mat.cols()));
        for(int i=0; i<trans_mat.rows(); ++i) for(int j=0; j<trans_mat.cols(); ++j) mat[i][j] = trans_mat(i, j);
    } else {
        EigenUIntMat trans_mat = EigenUIntMat::Zero(nrows, ncols);
        std::vector<int64_t> diag_ids;
        if (mode == DiagonalTransformMode::MODE_1){
            diag_ids.push_back(0);
            auto ndiags = std::max(nrows, ncols);
            for (int64_t diag_id = 1; diag_id<(int64_t)ndiags; diag_id++){
                if (diag_id < (int64_t)ncols) diag_ids.push_back(diag_id);
                if (diag_id < (int64_t)nrows) diag_ids.push_back(-diag_id);
            }
        } else if (mode == DiagonalTransformMode::MODE_2){
            for (int64_t diag_id = -(int64_t)nrows+1; diag_id < (int64_t)ncols; diag_id++) diag_ids.push_back(diag_id);
        } else if (mode == DiagonalTransformMode::MODE_3){
            for (int64_t diag_id = (int64_t)ncols-1; diag_id > -(int64_t)nrows; diag_id--) diag_ids.push_back(diag_id);
        }

        int64_t i_orig, j_orig, i_offset, j_offset, nelems_in_diag;
        size_t o = 0u;
        for (auto diag_id : diag_ids){
            if (diag_id >= 0) {
                nelems_in_diag = std::max(nrows, ncols) - diag_id;
                i_offset = 0; j_offset = diag_id;
            } else {
                nelems_in_diag = std::max(nrows, ncols) + diag_id;
                i_offset = -diag_id; j_offset = 0;
            }
            for (int64_t k_diag = 0; k_diag<nelems_in_diag; k_diag++){
                i_orig = k_diag + i_offset; j_orig = k_diag + j_offset;
                if (i_orig >= (int64_t)nrows || j_orig >= (int64_t)ncols) break;
                auto v = xt_mat(i_orig, j_orig);
                size_t i_packed = o / ncols; size_t j_packed = o % ncols;
                if (i_packed < nrows) trans_mat((int)i_packed, (int)j_packed) = v;
                o++;
            }
        }
        mat.assign(trans_mat.rows(), std::vector<uint32_t>(trans_mat.cols()));
        for(int i=0; i<trans_mat.rows(); ++i) for(int j=0; j<trans_mat.cols(); ++j) mat[i][j] = trans_mat(i, j);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_row_bin(
    const BinMatDtype& bin_mat,
    UIntMatDtype& mat
){
    size_t bin_mat_nrows = bin_mat.size();
    if (bin_mat_nrows == 0) return;
    size_t bin_mat_ncols = bin_mat[0].size();

    size_t mat_ncols = bin_mat_ncols - 1;
    size_t mat_nrows = 0;
    for(size_t i = 0; i < bin_mat_nrows; ++i) if(bin_mat[i][0]) mat_nrows++;

    mat.assign(mat_nrows, std::vector<uint32_t>(mat_ncols, 0));
    size_t target_i = 0;
    uint8_t bit_pos = 0;

    for (size_t i = 0; i < bin_mat_nrows; ++i){
        for (size_t j = 1; j < bin_mat_ncols; ++j) {
            if (bin_mat[i][j]) mat[target_i][j-1] |= (1u << bit_pos);
        }
        if (bin_mat[i][0]){
            target_i++; bit_pos = 0;
        } else bit_pos++;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void transform_row_bin(
    const UIntMatDtype& mat,
    BinMatDtype& bin_mat
) {
    auto nrows = mat.size();
    if (nrows == 0) return;
    auto ncols = mat[0].size();

    std::vector<uint8_t> nbits_per_row(nrows);
    for(size_t i=0; i<nrows; ++i) {
        uint32_t max_val = 0;
        for (uint32_t v : mat[i]) if (v > max_val) max_val = v;
        nbits_per_row[i] = (max_val == 0) ? 1 : static_cast<uint8_t>(std::ceil(std::log2(static_cast<double>(max_val) + 1.0)));
        if (nbits_per_row[i] == 0) nbits_per_row[i] = 1;
    }

    uint64_t bin_mat_nrows = 0;
    for (auto n : nbits_per_row) bin_mat_nrows += n;
    uint64_t bin_mat_ncols = ncols + 1;

    bin_mat.assign(bin_mat_nrows, std::vector<bool>(bin_mat_ncols, false));

    size_t current_bin_mat_row = 0;
    for (size_t i = 0; i < nrows; ++i) {
        auto bitlength = nbits_per_row[i];
        for (size_t i_bit = 0; i_bit < bitlength; ++i_bit) {
            for(size_t j=0; j<ncols; ++j) {
                if(mat[i][j] & (1u << i_bit)) bin_mat[current_bin_mat_row][j+1] = true;
            }
            current_bin_mat_row++;
        }
        if (current_bin_mat_row > 0) bin_mat[current_bin_mat_row - 1][0] = true;
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
    const BinMatDtype& bin_mat,
    uint8_t** payload,
    size_t& payload_len
) {
    if (bin_mat.empty()) {
        payload_len = 0; *payload = nullptr; return;
    }
    auto nrows = bin_mat.size();
    auto ncols = bin_mat[0].size();
    auto bpl = (ncols >> 3u) + ((ncols & 7u) > 0u);
    payload_len = bpl * nrows;
    *payload = (unsigned char*) calloc (payload_len, sizeof(unsigned char));
    for (size_t i = 0u; i < nrows; i++) {
        size_t row_offset = i * bpl;
        for (size_t j = 0u; j < ncols; j++) {
            if (bin_mat[i][j]) {
                auto byte_offset = row_offset + (j >> 3u);
                uint8_t shift = static_cast<uint8_t>(7u - (j & 7u));
                *(*payload + byte_offset) |= (1u << shift);
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void bin_mat_from_bytes(
    const uint8_t* payload,
    size_t payload_len,
    size_t nrows,
    size_t ncols,
    BinMatDtype& bin_mat
) {
    auto bpl = (ncols >> 3u) + ((ncols & 7u) > 0u);
    bin_mat.assign(nrows, std::vector<bool>(ncols, false));
    for (size_t i = 0u; i < nrows; i++) {
        size_t row_offset = i * bpl;
        for (size_t j = 0u; j < ncols; j++) {
            auto byte_offset = row_offset + (j >> 3u);
            uint8_t shift = static_cast<uint8_t>(7u - (j & 7u));
            bin_mat[i][j] = (*(payload + byte_offset) >> shift) & 1u;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_cm_tile(
    const genie::contact::ContactMatrixTilePayload& tile_payload,
    core::AlgoID codec_ID,
    BinMatDtype& bin_mat
){
    uint8_t* raw_data = nullptr; size_t raw_data_len = 0;
    unsigned long tile_nrows = 0; unsigned long tile_ncols = 0;
    if (codec_ID == core::AlgoID::JBIG){
        mpegg_jbig_decompress_default(&raw_data, &raw_data_len, tile_payload.GetPayload().data(), tile_payload.GetPayloadSize(), &tile_nrows, &tile_ncols);
        detail::eigen::bin_mat_from_bytes(raw_data, raw_data_len, (size_t)tile_nrows, (size_t)tile_ncols, bin_mat);
        if (raw_data) free(raw_data);
    } else UTILS_DIE("Not yet implemented");
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_cm_tile(
    const BinMatDtype& bin_mat,
    const core::AlgoID codec_ID,
    genie::contact::ContactMatrixTilePayload& tile_payload
) {
    uint8_t* payload = nullptr; size_t payload_len = 0;
    uint8_t* compressed_payload = nullptr; size_t compressed_payload_len = 0;
    auto tile_nrows = static_cast<uint32_t>(bin_mat.size());
    auto tile_ncols = static_cast<uint32_t>(bin_mat.empty() ? 0 : bin_mat[0].size());
    if (codec_ID == genie::core::AlgoID::JBIG) {
        detail::eigen::bin_mat_to_bytes(bin_mat, &payload, payload_len);
        mpegg_jbig_compress_default(&compressed_payload, &compressed_payload_len, payload, payload_len, tile_nrows, tile_ncols);
        if (payload) free(payload);
    } else UTILS_DIE("Not yet implemented");
    auto _tile_payload = ContactMatrixTilePayload(codec_ID, tile_nrows, tile_ncols, &compressed_payload, compressed_payload_len);
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
    if (tile_counts.empty()) return;
    std::map<std::pair<uint64_t, uint64_t>, uint32_t> lr_sparse_tile_map;
    for (size_t i = 0; i < tile_counts.size(); i++){
        lr_sparse_tile_map[{tile_row_ids[i] / bin_size_mult, tile_col_ids[i] / bin_size_mult}] += tile_counts[i];
    }
    size_t lr_num_entries = lr_sparse_tile_map.size();
    tile_row_ids.assign(lr_num_entries, 0); tile_col_ids.assign(lr_num_entries, 0); tile_counts.assign(lr_num_entries, 0);
    size_t i_entry = 0u;
    for (const auto & pair_entry : lr_sparse_tile_map){
        tile_row_ids[i_entry] = pair_entry.first.first; tile_col_ids[i_entry] = pair_entry.first.second; tile_counts[i_entry] = pair_entry.second;
        i_entry++;
    }
    if (sort_output) detail::eigen::sort_sparse_mat_inplace(tile_row_ids, tile_col_ids, tile_counts);
}

// ---------------------------------------------------------------------------------------------------------------------

void sort_sparse_mat_inplace(
    UInt64VecDtype& tile_row_ids,
    UInt64VecDtype& tile_col_ids,
    UIntVecDtype& tile_counts
){
    size_t num_entries = tile_counts.size();
    std::vector<size_t> p(num_entries); std::iota(p.begin(), p.end(), 0);
    std::stable_sort(p.begin(), p.end(), [&](size_t i1, size_t i2) {
        if (tile_row_ids[i1] != tile_row_ids[i2]) return tile_row_ids[i1] < tile_row_ids[i2];
        return tile_col_ids[i1] < tile_col_ids[i2];
    });
    UInt64VecDtype s_row_ids(num_entries); UInt64VecDtype s_col_ids(num_entries); UIntVecDtype s_counts(num_entries);
    for (size_t k = 0u; k < num_entries; k++){
        s_row_ids[k] = tile_row_ids[p[k]]; s_col_ids[k] = tile_col_ids[p[k]]; s_counts[k] = tile_counts[p[k]];
    }
    tile_row_ids = std::move(s_row_ids); tile_col_ids = std::move(s_col_ids); tile_counts = std::move(s_counts);
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
  BinVecDtype row_mask, col_mask;
  auto interval = cm_param.GetBinSize(); auto tile_size = cm_param.GetTileSize();
  auto chr1_ID = rec.GetChr1ID(); auto chr1_num_bin_entries = cm_param.GetNumBinEntries(chr1_ID); auto ntiles_in_row = cm_param.GetNumTiles(chr1_ID);
  auto chr2_ID = rec.GetChr2ID(); auto chr2_num_bin_entries = cm_param.GetNumBinEntries(chr2_ID); auto ntiles_in_col = cm_param.GetNumTiles(chr2_ID);
  cm_param.UpsertSample(rec.GetSampleID(), std::string(rec.GetSampleName())); scm_payload.SetSampleID(rec.GetSampleID());
  scm_param.SetChr1ID(chr1_ID); scm_payload.SetChr1ID(chr1_ID); scm_param.SetChr2ID(chr2_ID); scm_payload.SetChr2ID(chr2_ID);
  auto is_intra_scm = scm_param.IsIntraSCM(); scm_param.SetCodecID(codec_ID);
  scm_param.SetNumTiles(ntiles_in_row, ntiles_in_col); scm_payload.SetNumTiles(ntiles_in_row, ntiles_in_col);
  
  UInt64VecDtype row_ids = rec.GetStartPos1(); for(auto& id : row_ids) id /= interval;
  UInt64VecDtype col_ids = rec.GetStartPos2(); for(auto& id : col_ids) id /= interval;
  UIntVecDtype counts = rec.GetCounts();

  if (remove_unaligned_region){
      detail::eigen::compute_masks(row_ids, col_ids, chr1_num_bin_entries, chr2_num_bin_entries, is_intra_scm, row_mask, col_mask);
      if (transform_mask){
          RunLengthEncodingData rowRLEData; detail::eigen::set_rle_information_from_mask(rowRLEData, row_mask);
          if(is_intra_scm) {
            scm_payload.SetRowMaskPayload(SubcontactMatrixMaskPayload(rowRLEData.transformID, rowRLEData.firstVal, rowRLEData.rl_entries));
            scm_param.SetRowMaskExistsFlag(true); scm_param.SetColMaskExistsFlag(false);
          } else {
            RunLengthEncodingData colRLEData; detail::eigen::set_rle_information_from_mask(colRLEData, col_mask);
            scm_payload.SetRowMaskPayload(SubcontactMatrixMaskPayload(rowRLEData.transformID, rowRLEData.firstVal, rowRLEData.rl_entries));
            scm_param.SetRowMaskExistsFlag(true);
            scm_payload.SetColMaskPayload(SubcontactMatrixMaskPayload(colRLEData.transformID, colRLEData.firstVal, colRLEData.rl_entries));
            scm_param.SetColMaskExistsFlag(true);
          }
      } else {
        scm_payload.SetRowMaskPayload(SubcontactMatrixMaskPayload(row_mask)); scm_param.SetRowMaskExistsFlag(true);
        if (!is_intra_scm){ scm_payload.SetColMaskPayload(SubcontactMatrixMaskPayload(col_mask)); scm_param.SetColMaskExistsFlag(true); }
      }
      detail::eigen::remove_unaligned(row_ids, col_ids, is_intra_scm, row_mask, col_mask);
  } else {
      scm_param.SetRowMaskExistsFlag(false); scm_param.SetColMaskExistsFlag(false);
  }

  for (size_t i_tile = 0u; i_tile < ntiles_in_row; i_tile++) {
      for (size_t j_tile = 0u; j_tile < ntiles_in_col; j_tile++) {
          if (i_tile > j_tile && is_intra_scm) continue;
          size_t s1, e1, s2, e2; detail::eigen::comp_start_end_ids(chr1_num_bin_entries, tile_size, i_tile, s1, e1); detail::eigen::comp_start_end_ids(chr2_num_bin_entries, tile_size, j_tile, s2, e2);
          UInt64VecDtype tr, tc; UIntVecDtype tct;
          for (size_t k=0; k<row_ids.size(); ++k) if (row_ids[k] >= s1 && row_ids[k] < e1 && col_ids[k] >= s2 && col_ids[k] < e2) { tr.push_back(row_ids[k]-s1); tc.push_back(col_ids[k]-s2); tct.push_back(counts[k]); }
          if (tct.empty()) {
              scm_payload.SetTilePayload(i_tile, j_tile, ContactMatrixTilePayload(codec_ID, 0, 0, nullptr, 0));
              scm_param.SetTileParameter(i_tile, j_tile, {DiagonalTransformMode::NONE, BinarizationMode::ROW_BINARIZATION}); continue;
          }
          detail::eigen::sort_sparse_mat_inplace(tr, tc, tct);
          UIntMatDtype tile_mat; detail::eigen::sparse_to_dense(tr, tc, tct, tile_size, tile_size, tile_mat);
          if (ena_diag_transform && is_intra_scm && i_tile == j_tile) detail::eigen::diag_transform(tile_mat, DiagonalTransformMode::MODE_0);
          BinMatDtype bin_mat; detail::eigen::transform_row_bin(tile_mat, bin_mat);
          genie::contact::ContactMatrixTilePayload cm_tile_payload; detail::eigen::encode_cm_tile(bin_mat, codec_ID, cm_tile_payload);
          scm_param.SetTileParameter(i_tile, j_tile, {(ena_diag_transform && is_intra_scm && i_tile == j_tile) ? DiagonalTransformMode::MODE_0 : DiagonalTransformMode::NONE, BinarizationMode::ROW_BINARIZATION});
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
    BinVecDtype row_mask, col_mask; std::vector<uint64_t> s1_v, e1_v, s2_v, e2_v; std::vector<uint32_t> c_v;
    auto bin_size = cm_param.GetBinSize(); auto target_bin_size = bin_size * bin_size_mult; auto tile_size = cm_param.GetTileSize();
    auto chr1_ID = scm_param.GetChr1ID(); auto chr2_ID = scm_param.GetChr2ID(); auto is_intra_scm = scm_param.IsIntraSCM();
    auto codec_ID = scm_param.GetCodecID();
    auto chr1_len = cm_param.GetChromosomeLength(chr1_ID); auto chr1_num_bin_entries = cm_param.GetNumBinEntries(chr1_ID);
    auto chr2_len = cm_param.GetChromosomeLength(chr2_ID); auto chr2_num_bin_entries = cm_param.GetNumBinEntries(chr2_ID);
    auto ntiles_in_row = cm_param.GetNumTiles(chr1_ID); auto ntiles_in_col = cm_param.GetNumTiles(chr2_ID);

    if (scm_param.GetRowMaskExistsFlag() || scm_param.GetColMaskExistsFlag()) detail::eigen::decode_scm_masks(cm_param, scm_param, scm_payload, row_mask, col_mask);

    for (size_t i_tile = 0u; i_tile < ntiles_in_row; i_tile++) {
        for (size_t j_tile = 0u; j_tile < ntiles_in_col; j_tile++) {
            if (i_tile > j_tile && is_intra_scm) continue;
            auto& tile_payload = scm_payload.GetTilePayload(i_tile, j_tile); if (tile_payload.GetPayloadSize() == 0) continue;
            BinMatDtype bin_mat; detail::eigen::decode_cm_tile(tile_payload, codec_ID, bin_mat);
            UIntMatDtype tile_mat; detail::eigen::inverse_transform_row_bin(bin_mat, tile_mat);
            auto& tile_param = scm_param.GetTileParameter(i_tile, j_tile); detail::eigen::inverse_diag_transform(tile_mat, tile_param.diag_tranform_mode);
            size_t s1, e1, s2, e2; detail::eigen::comp_start_end_ids(chr1_num_bin_entries, tile_size, i_tile, s1, e1); detail::eigen::comp_start_end_ids(chr2_num_bin_entries, tile_size, j_tile, s2, e2);
            UInt64VecDtype tr, tc; UIntVecDtype tct; detail::eigen::dense_to_sparse(tile_mat, tr, tc, tct);
            if (scm_param.GetRowMaskExistsFlag() || scm_param.GetColMaskExistsFlag()){
                BinVecDtype trm(row_mask.begin()+s1, row_mask.begin()+e1); BinVecDtype tcm(col_mask.begin()+s2, col_mask.begin()+e2);
                detail::eigen::insert_unaligned(tr, tc, is_intra_scm && i_tile == j_tile, trm, tcm);
            }
            for (auto& id : tr) id += s1; for (auto& id : tc) id += s2;
            if (bin_size_mult != 1) detail::eigen::conv_noop_on_sparse_mat(tr, tc, tct, bin_size_mult, false);
            s1_v.insert(s1_v.end(), tr.begin(), tr.end()); s2_v.insert(s2_v.end(), tc.begin(), tc.end()); c_v.insert(c_v.end(), tct.begin(), tct.end());
        }
    }
    rec.SetSampleId(scm_payload.GetSampleID()); rec.SetSampleName(std::string(cm_param.GetSampleName(scm_payload.GetSampleID())));
    rec.SetChr1ID(chr1_ID); rec.SetChr2ID(chr2_ID); rec.SetBinSize(bin_size);
    e1_v.resize(s1_v.size()); e2_v.resize(s2_v.size());
    for(size_t i=0; i<s1_v.size(); ++i) { s1_v[i] *= target_bin_size; e1_v[i] = std::min(s1_v[i] + target_bin_size, chr1_len); s2_v[i] *= target_bin_size; e2_v[i] = std::min(s2_v[i] + target_bin_size, chr2_len); }
    rec.SetCMValues(std::move(s1_v), std::move(e1_v), std::move(s2_v), std::move(e2_v), std::move(c_v));
}

} // namespace genie::contact::detail::eigen