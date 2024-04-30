/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "contact_coder.h"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <xtensor/xadapt.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xio.hpp>
#include <codecs/include/mpegg-codecs.h>
#include <genie/core/record/contact/record.h>
#include <genie/util/runtime-exception.h>
#include "contact_matrix_parameters.h"
#include "contact_matrix_tile_payload.h"
#include "subcontact_matrix_parameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

void compute_mask(
    // input
    UInt64VecDtype& ids,
    size_t nelems,
    // Output
    BinVecDtype& mask
){
//    auto nelems = xt::amax(ids)(0)+1;
    mask = xt::zeros<bool>({nelems});

    UInt64VecDtype unique_ids = xt::unique(ids);
    for (auto id: unique_ids){
        mask(id) = true;
    }
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
    UTILS_DIE_IF(row_ids.shape(0) != col_ids.shape(0),
                 "The size of row_ids and col_ids must be same!");

    if (is_intra_scm){
        UTILS_DIE_IF(nrows != ncols,
            "Both nentries must be the same for intra SCM!"
        );

        BinVecDtype mask;

        // Handle the symmetry of intra SCM
        UInt64VecDtype ids = xt::concatenate(xt::xtuple(row_ids, col_ids));
        ids = xt::unique(ids);

        compute_mask(ids, nrows, mask);

        row_mask.resize(mask.shape());
        row_mask = BinVecDtype(mask);
        col_mask.resize(mask.shape());
        col_mask = BinVecDtype(std::move(mask));
    } else {
        compute_mask(row_ids, nrows, row_mask);
        compute_mask(col_ids, ncols, col_mask);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_cm_masks(
    // Inputs
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    const SubcontactMatrixPayload& scm_payload,
    // Outputs
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
){
    auto row_nentries = cm_param.getNumBinEntries(scm_param.getChr1ID());
    auto col_nentries = cm_param.getNumBinEntries(scm_param.getChr2ID());

    if (scm_param.getRowMaskExistsFlag()){
        decode_cm_mask_payload(
            scm_payload.getRowMaskPayload(),
            row_nentries,
            row_mask
        );
    } else {
        row_mask = xt::ones<bool>({row_nentries});
    }

    if (scm_param.isIntraSCM()){
        col_mask = row_mask;
    } else if (scm_param.getColMaskExistsFlag()){
        decode_cm_mask_payload(
            scm_payload.getColMaskPayload(),
            col_nentries,
            col_mask
        );
    } else {
        col_mask = xt::ones<bool>({col_nentries});
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_cm_mask_payload(
    // Inputs
    const SubcontactMatrixMaskPayload& mask_payload,
    size_t num_entries,
    // Outputs
    BinVecDtype& mask
) {
    auto transform_ID = mask_payload.getTransformID();
    if (transform_ID == TransformID::ID_0){
        auto& mask_array = mask_payload.getMaskArray();
        UTILS_DIE_IF(
            num_entries != mask_array.size(),
            "num_entries and the size of mask_array differ!"
        );
        mask = xt::adapt(mask_array, {mask_array.size()});
    } else {
        mask.resize({num_entries});

        bool first_val = mask_payload.getFirstVal();
        auto& rl_entries = mask_payload.getRLEntries();

        size_t start_idx = 0;
        size_t end_idx = 0;
        for (const auto& rl_entry: rl_entries){
            end_idx += rl_entry;
            // This is the for-loop for assigning the values based on run-lenght
            xt::view(mask, xt::range(start_idx, end_idx)) = first_val;
            start_idx = end_idx;
            first_val = !first_val;
        }
        UTILS_DIE_IF(
            start_idx >= num_entries,
            "start_idx value must be smaller than num_entries!"
        );
        // This is the for-loop for assigning the remaining values
        xt::view(mask, xt::range(start_idx, num_entries)) = first_val;
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
    UTILS_DIE_IF(row_ids.shape(0) != col_ids.shape(0),
                 "The size of row_ids and col_ids must be same!");


    if (is_intra_tile){
        //TODO(yeremia): to be deleted!
        UTILS_DIE_IF(row_mask != col_mask, "row_mask and col_mask are different!");

        auto num_entries = row_ids.shape(0);
        auto& mask = row_mask; // Note: Does not matter either row_mask or col_mask
        auto mapping_len = mask.shape(0);
        UInt64VecDtype mapping = xt::empty<uint64_t>({mapping_len});
        uint64_t new_id = 0u;
        for (auto i = 0u; i<mapping_len; i++){
            mapping(i) = new_id;
            auto v = mask(i);
            if (v){
                new_id++;
            }
        }

        for (auto i = 0u; i<num_entries; i++){
            auto old_id = row_ids(i);
            auto new_id = mapping(old_id);
            row_ids(i) = new_id;
        }
        for (auto i = 0u; i<num_entries; i++){
            auto old_id = col_ids(i);
            auto new_id = mapping(old_id);
            col_ids(i) = new_id;
        }
    } else {
        auto row_mapping_len = row_mask.shape(0);
        auto row_mapping = xt::empty<uint64_t>({row_mapping_len});
        {
            uint64_t new_id = 0u;
            for (auto i = 0u; i<row_mapping_len; i++){
                row_mapping(i) = new_id;
                auto v = row_mask(i);
                if (v){
                    new_id++;
                }
            }
        }
        auto num_entries = row_ids.shape(0);
        for (auto i = 0u; i<num_entries; i++){
            auto old_id = row_ids(i);
            auto new_id = row_mapping(old_id);
            row_ids(i) = new_id;
        }

        auto col_mapping_len = col_mask.shape(0);
        auto col_mapping = xt::empty<uint64_t>({col_mapping_len});
        {
            uint64_t new_id = 0u;
            for (auto i = 0u; i<col_mapping_len; i++){
                col_mapping(i) = new_id;
                auto v = col_mask(i);
                if (v){
                    new_id++;
                }
            }
        }
        num_entries = col_ids.shape(0);
        for (auto i = 0u; i<num_entries; i++){
            auto old_id = col_ids(i);
            auto new_id = col_mapping(old_id);
            col_ids(i) = new_id;
        }
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
    UTILS_DIE_IF(row_ids.shape(0) != col_ids.shape(0),
                 "The size of row_ids and col_ids must be same!");


    if (is_intra_tile){
        //TODO(yeremia): to be deleted!
        UTILS_DIE_IF(row_mask != col_mask, "row_mask and col_mask are different!");

        auto num_entries = row_ids.shape(0);
        auto& mask = row_mask; // Note: Does not matter either row_mask or col_mask

        auto argwhere_vec = xt::argwhere(mask);
        auto mapping_len = xt::sum(xt::cast<uint64_t>(mask))(0);
        auto mapping = xt::empty<uint64_t>({mapping_len});

        auto k = 0u;
        for (auto v : argwhere_vec){
            auto value = v[0];
            mapping(k++) = value;
        }

        for (auto i = 0u; i<num_entries; i++){
            row_ids(i) = mapping(row_ids(i));
            col_ids(i) = mapping(col_ids(i));
        }

    } else {
        // Handle row_mask
        {
            auto num_entries = row_ids.shape(0);
            auto argwhere_vec = xt::argwhere(row_mask);
            auto mapping_len = xt::sum(xt::cast<uint64_t>(row_mask))(0);
            auto mapping = xt::empty<uint64_t>({mapping_len});

            auto k = 0u;
            for (auto v : argwhere_vec){
                auto value = v[0];
                mapping(k++) = value;
            }

            for (auto i = 0u; i<num_entries; i++){
                row_ids(i) = mapping(row_ids(i));
            }
        }

        // Handle col_mask
        {
            auto num_entries = col_ids.shape(0);
            auto argwhere_vec = xt::argwhere(col_mask);
            auto mapping_len = xt::sum(xt::cast<uint64_t>(col_mask))(0);
            auto mapping = xt::empty<uint64_t>({mapping_len});

            auto k = 0u;
            for (auto v : argwhere_vec){
                auto value = v[0];
                mapping(k++) = value;
            }

            for (auto i = 0u; i<num_entries; i++){
                col_ids(i) = mapping(col_ids(i));
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
    // TODO(yeremia): Moves this check somewhere else
    {
        UTILS_DIE_IF(xt::amax(row_ids)(0) >= nrows, "Invalid nrows or row_ids!");
        UTILS_DIE_IF(xt::amax(col_ids)(0) >= ncols, "Invalid ncols or col_ids!");
        UTILS_DIE_IF(xt::any(xt::equal(counts, 0)), "Count with value 0 is found!");
    }
    mat = xt::zeros<uint32_t>({nrows, ncols});

    auto nentries = counts.shape(0);
    for (auto i = 0u; i<nentries; i++){
        auto count = counts(i);
        auto row_id = row_ids(i);
        auto col_id = col_ids(i);

        mat(row_id, col_id) = count;
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
    BinMatDtype mask = mat > 0u;

    auto ids = xt::argwhere(mask);
    auto num_entries = ids.size();

    row_ids.resize({num_entries});
    col_ids.resize({num_entries});
    counts.resize({num_entries});

    for (auto k = 0u; k<num_entries; k++){
        auto i = ids[k][0];
        auto j = ids[k][1];
        auto c = mat(i,j);

        row_ids[k] = i;
        col_ids[k] = j;
        counts[k] = c;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void sort_by_row_ids(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts
){
    auto num_entries = row_ids.size();

    UInt64VecDtype sort_ids = xt::argsort(row_ids);
    UInt64VecDtype tmp_row_ids = row_ids;
    UInt64VecDtype tmp_col_ids = col_ids;
    UIntVecDtype tmp_counts = counts;

    for (auto k = 0u; k< num_entries; k++){
        tmp_row_ids(k) = row_ids(sort_ids(k));
        tmp_col_ids(k) = col_ids(sort_ids(k));
        tmp_counts(k) = counts(sort_ids(k));
    }

    row_ids = tmp_row_ids;
    col_ids = tmp_col_ids;
    counts = tmp_counts;
}

//// ---------------------------------------------------------------------------------------------------------------------
//
//void comp_start_end_idx(
//    // Inputs
//    size_t nentries,
//    size_t tile_size,
//    size_t tile_idx,
//    // Outputs
//    size_t& start_idx,
//    size_t& end_idx
//){
//    start_idx = tile_idx * tile_size;
//    end_idx = std::min(nentries, start_idx + tile_size);
//}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_diag_transform(
    UIntMatDtype& mat,
    DiagonalTransformMode mode
){
    UIntMatDtype trans_mat;
    auto k = 0u;
    auto l = 0u;

    if (mode == DiagonalTransformMode::NONE) {
        return;  // Do nothing
    } else {
        if (mode == DiagonalTransformMode::MODE_0) {
            auto nrows = mat.shape(0);
            auto ncols = mat.shape(1);
            auto target_nrows = ncols;

            // Initailze trans_mat with zeros
            trans_mat = xt::zeros<uint32_t>({target_nrows, ncols});

            auto o = 0u;
            for (size_t k_diag = 0u; k_diag < ncols; k_diag++) {
                for (size_t target_i = 0u; target_i < (target_nrows - k_diag); target_i++) {
                    size_t target_j = target_i + k_diag;

                    size_t i = o / target_nrows;
                    size_t j = o % target_nrows;

                    auto v = mat(i, j);
                    trans_mat(target_i, target_j) = v;

                    o++;
                }
            }
            mat.resize(trans_mat.shape());
            mat = trans_mat;
        } else {
            auto nrows = static_cast<int64_t>(mat.shape(0));
            auto ncols = static_cast<int64_t>(mat.shape(1));
            trans_mat = xt::empty<uint32_t>({nrows, ncols});

            Int64VecDtype diag_ids = xt::empty<int64_t>({nrows+ncols-1});
            size_t k_elem;

            if (mode == DiagonalTransformMode::MODE_1){
                diag_ids(0) = 0;
                k_elem = 1u;
                auto ndiags = std::max(nrows, ncols);
                for (auto diag_id = 1; diag_id<ndiags; diag_id++){
                    if (diag_id < static_cast<int64_t>(ncols)){
                        diag_ids(k_elem++) = diag_id;
                    }
                    if (diag_id < static_cast<int64_t>(nrows)){
                        diag_ids(k_elem++) = -diag_id;
                    }
                }
            } else if (mode == DiagonalTransformMode::MODE_2){
                //            k_elem = 0u;
                //            for (int64_t diag_id = -nrows+1; diag_id < ncols; diag_id++){
                //                diag_ids(k_elem++) = diag_id;
                //            }
                diag_ids = xt::arange(-nrows+1, ncols, 1);
            } else if (mode == DiagonalTransformMode::MODE_3){
                //            k_elem = 0u;
                //            for (int64_t diag_id = ncols-1; diag_id > -nrows; diag_id--){
                //                diag_ids(k_elem++) = diag_id;
                //            }
                diag_ids = xt::arange<int64_t>(ncols-1, -nrows, -1);
            }

            int64_t target_i, target_j;
            int64_t i_offset, j_offset;
            int64_t nelems_in_diag;
            auto o = 0u;
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
                for (auto k_diag = 0; k_diag<nelems_in_diag; k_diag++){
                    target_i = k_diag + i_offset;
                    target_j = k_diag + j_offset;
                    if (target_i >= nrows)
                        break;
                    if (target_j >= ncols)
                        break;

                    size_t i = o / mat.shape(1);
                    size_t j = o % mat.shape(1);
                    trans_mat(target_i, target_j) = mat(i, j);

                    o++;
                }
            }
            mat = std::move(trans_mat);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void diag_transform(
    UIntMatDtype& mat,
    DiagonalTransformMode mode
){
    UIntMatDtype trans_mat;

    if (mode == DiagonalTransformMode::NONE){
        return ; // Do nothing
    } else if (mode == DiagonalTransformMode::MODE_0) {
        UTILS_DIE_IF(
            mat.shape(0) != mat.shape(1),
            "Matrix must be a square!"
        );

        auto nrows = mat.shape(0);
        auto new_nrows = nrows / 2 + 1;
        trans_mat = xt::zeros<uint32_t>({new_nrows, nrows});

        auto o = 0u;
        for (size_t k_diag = 0u; k_diag < nrows; k_diag++) {
            for (size_t i = 0u; i < (nrows - k_diag); i++) {
                size_t j = i + k_diag;

                auto v = mat(i, j);
                if (v != 0) {
                    size_t target_i = o / nrows;
                    size_t target_j = o % nrows;
                    trans_mat(target_i, target_j) = v;
                }
                o++;
            }
        }
        mat.resize(trans_mat.shape());
        mat = trans_mat;

    } else {
        auto nrows = static_cast<int64_t>(mat.shape(0));
        auto ncols = static_cast<int64_t>(mat.shape(1));
        trans_mat = xt::empty<uint32_t>({nrows, ncols});

        Int64VecDtype diag_ids = xt::empty<int64_t>({nrows+ncols-1});
        size_t k_elem;

        if (mode == DiagonalTransformMode::MODE_1){
            diag_ids(0) = 0;
            k_elem = 1u;
            auto ndiags = std::max(nrows, ncols);
            for (auto diag_id = 1; diag_id<ndiags; diag_id++){
                if (diag_id < static_cast<int64_t>(ncols))
                    diag_ids(k_elem++) = diag_id;
                if (diag_id < static_cast<int64_t>(nrows))
                    diag_ids(k_elem++) = -diag_id;
            }
        } else if (mode == DiagonalTransformMode::MODE_2){
//            k_elem = 0u;
//            for (int64_t diag_id = -nrows+1; diag_id < ncols; diag_id++){
//                diag_ids(k_elem++) = diag_id;
//            }
            diag_ids = xt::arange(-nrows+1, ncols, 1);
        } else if (mode == DiagonalTransformMode::MODE_3){
//            k_elem = 0u;
//            for (int64_t diag_id = ncols-1; diag_id > -nrows; diag_id--){
//                diag_ids(k_elem++) = diag_id;
//            }
            diag_ids = xt::arange<int64_t>(ncols-1, -nrows, -1);
        }

        int64_t i, j;
        int64_t i_offset, j_offset;
        int64_t nelems_in_diag;
        auto o = 0u;
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
            for (auto k_diag = 0; k_diag<nelems_in_diag; k_diag++){
                i = k_diag + i_offset;
                j = k_diag + j_offset;
                if (i >= nrows)
                    break;
                if (j >= ncols)
                    break;

                auto v = mat(i, j);
                size_t new_i = o / mat.shape(1);
                size_t new_j = o % mat.shape(1);
                trans_mat(new_i, new_j) = v;
//                if (v != 0) {
//                    size_t new_i = o / mat.shape(1);
//                    size_t new_j = o % mat.shape(1);
//                    trans_mat(new_i, new_j) = v;
//                }
                o++;
            }
        }
        mat = std::move(trans_mat);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void inverse_transform_row_bin(
    // Inputs
    const BinMatDtype& bin_mat,
    // Outputs
    UIntMatDtype& mat
){
    size_t bin_mat_nrows = bin_mat.shape(0);
    size_t bin_mat_ncols = bin_mat.shape(1);

    UTILS_DIE_IF(bin_mat_nrows == 0, "Invalid mat_nrows!");
    UTILS_DIE_IF(bin_mat_ncols == 0, "Invalid mat_ncols!");

    size_t mat_ncols = bin_mat_ncols-1;

    UIntVecDtype first_col = xt::cast<uint32_t>(xt::view(bin_mat, xt::all(), 0));
    size_t mat_nrows = xt::sum(first_col)(0);

    UTILS_DIE_IF(mat_nrows == 0, "Invalid mat_nrows!");
    UTILS_DIE_IF(mat_ncols == 0, "Invalid mat_ncols!");

    mat = xt::zeros<uint32_t>({mat_nrows, mat_ncols});

    size_t target_i = 0;
    uint8_t bit_pos = 0;
    auto target_js = xt::range(1u, bin_mat_ncols);
    for (auto i = 0u; i<bin_mat_nrows; i++){
        xt::view(mat, target_i, xt::all()) |= xt::cast<uint32_t>(xt::view(bin_mat, i, target_js)) << bit_pos;
        bool sentinel_flag = bin_mat(i, 0);

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
    auto nrows = mat.shape(0);
    auto ncols = mat.shape(1);

    UInt8VecDtype nbits_per_row = xt::cast<uint8_t>(xt::ceil(
        xt::log2(xt::amax(mat, {1}) + 1)
    ));
    // Handle the case where maximum value is 0 -> log2(1) = 0 bits
    xt::filter(nbits_per_row, xt::equal(nbits_per_row, 0u)) = 1;

    uint64_t bin_mat_nrows = static_cast<uint64_t>(xt::sum(nbits_per_row)(0));
    uint64_t bin_mat_ncols = ncols + 1;

    bin_mat = xt::zeros<bool>({bin_mat_nrows, bin_mat_ncols});

    size_t target_i = 0;
    auto target_js = xt::range(1u, bin_mat_ncols);
    for (size_t i = 0; i < nrows; i++) {
        auto bitlength = nbits_per_row[i];
        for (size_t i_bit = 0; i_bit < bitlength; i_bit++) {
            xt::view(bin_mat, target_i++, target_js) = xt::cast<bool>(xt::view(mat, i, xt::all()) & (1u << i_bit));
        }

        // Set the sentinel flag
        // Add offset due to "++" operation in the for-loop
        bin_mat(target_i - 1, 0) = true;
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

// TODO(yeremia): This should be part of LCC?
void bin_mat_to_bytes(
    // Inputs
    const BinMatDtype& bin_mat,
    // Outputs
    uint8_t** payload,
    size_t& payload_len
) {
    auto nrows = static_cast<size_t>(bin_mat.shape(0));
    auto ncols = static_cast<size_t>(bin_mat.shape(1));

    auto bpl = (ncols >> 3u) + ((ncols & 7u) > 0u);  // Ceil div operation
    payload_len = bpl * nrows;
    *payload = (unsigned char*) calloc (payload_len, sizeof(unsigned char));

    for (auto i = 0u; i < nrows; i++) {
        size_t row_offset = i * bpl;
        for (auto j = 0u; j < ncols; j++) {
            auto byte_offset = row_offset + (j >> 3u);
            uint8_t shift = (7u - (j & 7u));
            auto val = static_cast<uint8_t>(bin_mat(i, j));
            val = static_cast<uint8_t>(val << shift);
            *(*payload + byte_offset) |= val;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

// TODO(yeremia): This should be part of LCC?
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

    MatShapeDtype bin_mat_shape = {nrows, ncols};
    bin_mat.resize(bin_mat_shape);
    xt::view(bin_mat, xt::all(), xt::all()) = false;  // Initialize value with 0

    for (auto i = 0u; i < nrows; i++) {
        size_t row_offset = i * bpl;
        for (auto j = 0u; j < ncols; j++) {
            auto byte_offset = row_offset + (j >> 3u);
            uint8_t shift = (7u - (j & 7u));
            bin_mat(i, j) = (*(payload + byte_offset) >> shift) & 1u;
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
        compressed_data_len = tile_payload.getPayloadSize();
        auto& payload = tile_payload.getPayload();

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

        bin_mat_from_bytes(
            raw_data,
            raw_data_len,
            tile_nrows,
            tile_ncols,
            bin_mat
        );
        free(compressed_data);

    } else {

        tile_nrows = tile_payload.getTileNRows();
        tile_ncols = tile_payload.getTileNCols();

        UTILS_DIE("Not yet implemented");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_cm_tile(
    // Inputs
    const BinMatDtype& bin_mat,
    core::AlgoID codec_ID,
    // Outputs
    genie::contact::ContactMatrixTilePayload& tile_payload
) {
    uint8_t* payload;
    size_t payload_len;
    uint8_t* compressed_payload;
    size_t compressed_payload_len;

    auto tile_nrows = static_cast<uint32_t>(bin_mat.shape(0));
    auto tile_ncols = static_cast<uint32_t>(bin_mat.shape(1));

    if (codec_ID == genie::core::AlgoID::JBIG) {

        bin_mat_to_bytes(bin_mat, &payload, payload_len);

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

void decode_scm(
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    genie::contact::SubcontactMatrixPayload& scm_payload,
    core::record::ContactRecord& rec,
    uint32_t bin_size_mult
){
    // (not part of specification) Initialize variables
    BinVecDtype row_mask;
    BinVecDtype col_mask;
    UInt64VecDtype row_ids, col_ids;
    UIntVecDtype _counts;
    std::vector<uint64_t> start1;
    std::vector<uint64_t> end1;
    std::vector<uint64_t> start2;
    std::vector<uint64_t> end2;
    std::vector<uint32_t> counts;

    auto bin_size = cm_param.getBinSize();
    auto target_bin_size = bin_size * bin_size_mult;
    auto tile_size = cm_param.getTileSize();

    UTILS_DIE_IF(
        !cm_param.isBinSizeMultiplierValid(bin_size_mult),
        "Bin size multiplier is not supported!"
    );

    // Input parameters retrieved from parameter set
    auto chr1_ID = scm_param.getChr1ID();
    auto chr2_ID = scm_param.getChr2ID();
    auto is_intra_scm = scm_param.isIntraSCM();

    auto codec_ID = scm_param.getCodecID();
    auto row_mask_exists = scm_param.getRowMaskExistsFlag();
    auto col_mask_exists = scm_param.getColMaskExistsFlag();

    auto chr1_len = cm_param.getChromosomeLength(chr1_ID);
    auto chr1_num_bin_entries = cm_param.getNumBinEntries(chr1_ID);
    auto chr2_len = cm_param.getChromosomeLength(chr2_ID);
    auto chr2_num_bin_entries = cm_param.getNumBinEntries(chr2_ID);
    auto ntiles_in_row = cm_param.getNumTiles(chr1_ID);
    auto ntiles_in_col = cm_param.getNumTiles(chr2_ID);

    UTILS_DIE_IF(
        !cm_param.isBinSizeMultiplierValid(bin_size_mult),
        "Bin size multiplier is not supported!"
    );

    if (row_mask_exists || col_mask_exists){
        decode_cm_masks(
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

            // Not part of the specification
            UIntMatDtype tile_mat;
            size_t start1_idx, end1_idx, start2_idx, end2_idx;
            UInt64VecDtype tile_row_ids, tile_col_ids;
            UIntVecDtype tile_counts;

            // Assign
            auto& tile_param = scm_param.getTileParameter(i_tile, j_tile);
            auto& tile_payload = scm_payload.getTilePayload(i_tile, j_tile);
            auto binarization_mode = tile_param.binarization_mode;
            auto diag_transform_mode = tile_param.diag_tranform_mode;
            bool is_intra_tile = is_intra_scm && (i_tile == j_tile);

            if (binarization_mode == BinarizationMode::ROW_BINARIZATION){
                BinMatDtype bin_mat;

                decode_cm_tile(
                    tile_payload,
                    codec_ID,
                    bin_mat
                );

                inverse_transform_row_bin(
                    bin_mat,
                    tile_mat
                );

            } else {
                UTILS_DIE("no binarization is not supported yet!");
            }

            inverse_diag_transform(
                tile_mat,
                diag_transform_mode
            );

            comp_start_end_ids(
                chr1_num_bin_entries,
                tile_size,
                i_tile,
                start1_idx,
                end1_idx
            );

            comp_start_end_ids(
                chr2_num_bin_entries,
                tile_size,
                j_tile,
                start2_idx,
                end2_idx
            );

            if (bin_size_mult != 1){
                // TODO(yeremia): implement the on-the-fly normalization
                UTILS_DIE("Not yet implemented!");
            }

            dense_to_sparse(
                tile_mat,
                tile_row_ids,
                tile_col_ids,
                tile_counts
            );

            if (row_mask_exists || col_mask_exists){
                // This is slice function
                BinVecDtype tile_row_mask = xt::view(row_mask, xt::range(start1_idx, end1_idx));
                BinVecDtype tile_col_mask = xt::view(col_mask, xt::range(start2_idx, end2_idx));

                insert_unaligned(
                    tile_row_ids,
                    tile_col_ids,
                    is_intra_tile,
                    tile_row_mask,
                    tile_col_mask
                );
            }

            if (i_tile != 0){
                tile_row_ids += start1_idx;
            }
            if (j_tile != 0){
                tile_col_ids += start2_idx;
            }

            // Not in the specification
            // This is just for the concatenation
            if (i_tile == 0 && j_tile == 0){
                row_ids = tile_row_ids;
                col_ids = tile_col_ids;
                _counts = tile_counts;
            } else {
                row_ids = xt::concatenate(xt::xtuple(row_ids, tile_row_ids));
                col_ids = xt::concatenate(xt::xtuple(col_ids, tile_col_ids));
                _counts = xt::concatenate(xt::xtuple(_counts, tile_counts));
            }

        }
    }

    // Not in the specification
    auto num_entries = _counts.shape(0);
    start1.resize(num_entries);
    end1.resize(num_entries);
    start2.resize(num_entries);
    end2.resize(num_entries);
    counts.resize(num_entries);

    for (auto i=0u; i<num_entries; i++){
        start1[i] = row_ids(i) * target_bin_size;
        end1[i] = std::min(start1[i] + target_bin_size, chr1_len);
        start2[i] = col_ids(i) * target_bin_size;
        end2[i] = std::min(start2[i] + target_bin_size, chr2_len);
        counts[i] = _counts(i);
    }

    auto sample_ID = scm_payload.getSampleID();
    rec.setSampleID(sample_ID);
    //TODO(yeremia): fix this
//    auto sample_name = std::string(cm_param.getSampleName(sample_ID));
//    rec.setSampleName(std::move(sample_name));
    rec.setChr1ID(chr1_ID);
    rec.setChr2ID(chr2_ID);
    rec.setBinSize(bin_size);
    rec.SetCMValues(
        std::move(start1),
        std::move(end1),
        std::move(start2),
        std::move(end2),
        std::move(counts)
    );
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_scm(
    // Inputs
    ContactMatrixParameters& cm_param,
    core::record::ContactRecord& rec,
    // Outputs
    SubcontactMatrixParameters& scm_param,
    genie::contact::SubcontactMatrixPayload& scm_payload,
    // Options
    bool remove_unaligned_region,
    bool transform_mask,
    bool ena_diag_transform,
    bool ena_binarization,
    core::AlgoID codec_ID
) {

    // Initialize variables
    BinVecDtype row_mask;
    BinVecDtype col_mask;

    auto interval = cm_param.getBinSize();
    auto tile_size = cm_param.getTileSize();
    auto num_entries = rec.getNumEntries();
    auto chr1_ID = rec.getChr1ID();
    auto chr1_num_bin_entries = cm_param.getNumBinEntries(chr1_ID);
    auto ntiles_in_row = cm_param.getNumTiles(chr1_ID);
    auto chr2_ID = rec.getChr2ID();
    auto chr2_num_bin_entries = cm_param.getNumBinEntries(chr2_ID);
    auto ntiles_in_col = cm_param.getNumTiles(chr2_ID);

    scm_payload.setSampleID(rec.getSampleID());

    scm_param.setChr1ID(chr1_ID);
    scm_payload.setChr1ID(chr1_ID);

    scm_param.setChr2ID(chr2_ID);
    scm_payload.setChr2ID(chr2_ID);

    auto is_intra_scm = scm_param.isIntraSCM();

    scm_param.setCodecID(codec_ID);

    scm_param.setNumTiles(ntiles_in_row, ntiles_in_col);
    scm_payload.setNumTiles(ntiles_in_row, ntiles_in_col);

    UInt64VecDtype row_ids = xt::adapt(rec.getStartPos1(), {num_entries});
    row_ids /= interval;

    UInt64VecDtype col_ids = xt::adapt(rec.getStartPos2(), {num_entries});
    col_ids /= interval;

    UIntVecDtype counts = xt::adapt(rec.getCounts(), {num_entries});

    if (remove_unaligned_region){
        // Compute mask for
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
            //TODO(yeremia): implement this
            UTILS_DIE("Not yet implemented!");
        } else {
            auto row_mask_payload = SubcontactMatrixMaskPayload(
                std::move(row_mask)
            );

            scm_payload.setRowMaskPayload(
                std::move(row_mask)
            );
            scm_param.setRowMaskExistsFlag(true);

            auto col_mask_payload = SubcontactMatrixMaskPayload(
                std::move(col_mask)
            );

            scm_payload.setColMaskPayload(
                std::move(col_mask)
            );
            scm_param.setColMaskExistsFlag(true);
        }
    }

    for (size_t i_tile = 0u; i_tile < ntiles_in_row; i_tile++){

        auto min_row_id = i_tile*tile_size;
        auto max_row_id = std::min(min_row_id+tile_size, chr1_num_bin_entries);

        for (size_t j_tile = 0u; j_tile< ntiles_in_col; j_tile++){

            if (i_tile > j_tile && is_intra_scm){
                continue;
            }

            uint32_t tile_nrows, tile_ncols;

            // Assign
            auto& tile_param = scm_param.getTileParameter(i_tile, j_tile);
            auto& diag_transform_mode = tile_param.diag_tranform_mode;
            auto& binarization_mode = tile_param.binarization_mode;
            bool is_intra_tile = is_intra_scm && (i_tile == j_tile);

            // Mode selection for encoding
            if (ena_diag_transform){
                if (i_tile == j_tile){
                    if (is_intra_scm){
                        diag_transform_mode = DiagonalTransformMode::MODE_0;
                    } else
                        diag_transform_mode = DiagonalTransformMode::MODE_1;
                } else if (i_tile < j_tile){
                    diag_transform_mode = DiagonalTransformMode::MODE_2;
                } else if (j_tile > i_tile) {
                    diag_transform_mode = DiagonalTransformMode::MODE_3;
                } else {
                    UTILS_DIE("This should never be reached!");
                }

            } else {
                diag_transform_mode = DiagonalTransformMode::NONE;
            }

            if (ena_binarization){
                binarization_mode = BinarizationMode::ROW_BINARIZATION;
            } else {
                binarization_mode = BinarizationMode::NONE;
            }

            auto min_col_id = j_tile*tile_size;
            auto max_col_id = std::min(min_col_id+tile_size, chr2_num_bin_entries);

            // Filter entries that belongs to the current tile
            BinVecDtype mask1 = (row_ids >= min_row_id) && (row_ids < max_row_id);
            BinVecDtype mask2 = (col_ids >= min_col_id) && (col_ids < max_col_id);
            BinVecDtype mask = mask1 && mask2;

            //TODO(yeremia): create a pipeline where the whole tile is unaligned
            UTILS_DIE_IF(!xt::any(mask), "There is no entry in tile_mat at all?!");

            // Filter the values only for the corresponding tile
            UInt64VecDtype tile_row_ids = xt::filter(row_ids, mask);
            UInt64VecDtype tile_col_ids = xt::filter(col_ids, mask);
            UIntVecDtype tile_counts = xt::filter(counts, mask);

            if (min_row_id > 0){
                auto min_id = xt::amin(tile_row_ids)(0);
                tile_row_ids -= min_row_id;
            }
            if (min_col_id > 0){
                auto min_id = xt::amin(tile_col_ids)(0);
                tile_col_ids -= min_col_id;
            }

            tile_nrows = static_cast<uint32_t>(std::min(max_row_id, chr1_num_bin_entries) - min_row_id);
            tile_ncols = static_cast<uint32_t>(std::min(max_col_id, chr2_num_bin_entries) - min_col_id);

            if (remove_unaligned_region){
                BinVecDtype tile_row_mask = xt::view(row_mask, xt::range(min_row_id, max_row_id));
                BinVecDtype tile_col_mask = xt::view(col_mask, xt::range(min_col_id, max_col_id));

                remove_unaligned(
                    tile_row_ids,
                    tile_col_ids,
                    is_intra_tile,
                    tile_row_mask,
                    tile_col_mask
                );

            }

            UIntMatDtype tile_mat;

            //TODO(yeremia): Create a specification where sparse2dense transformation is disabled
            //               better for no transformation compression
            sparse_to_dense(
                tile_row_ids,
                tile_col_ids,
                tile_counts,
                tile_nrows,
                tile_ncols,
                tile_mat
            );
            genie::contact::diag_transform(tile_mat, diag_transform_mode);

            if (binarization_mode == BinarizationMode::ROW_BINARIZATION){
                genie::contact::BinMatDtype bin_mat;
                genie::contact::transform_row_bin(tile_mat, bin_mat);

                ContactMatrixTilePayload tile_payload;
                encode_cm_tile(
                    bin_mat,
                    codec_ID,
                    tile_payload
                );

                scm_payload.setTilePayload(
                    i_tile,
                    j_tile,
                    std::move(tile_payload)
                );

            // BinarizationMode::NONE
            } else {
                UTILS_DIE("Not yet implemented!");
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void encode_cm(
    std::list<genie::core::record::ContactRecord>& recs,
    const EncodingOptions& opt,
    EncodingBlock& block
) {

    auto params = ContactMatrixParameters();
    params.setBinSize(opt.bin_size);
    params.setTileSize(opt.tile_size);
    std::map<uint8_t, SampleInformation> samples;
    std::map<uint8_t, ChromosomeInformation> chrs;

    for (auto& rec: recs){
        auto rec_bin_size = rec.getBinSize();
        UTILS_DIE_IF(rec_bin_size != opt.bin_size, "Found record with different bin_size!");

        if (rec.getChr1ID() > rec.getChr2ID())
            rec.transposeCM();

        uint8_t chr1_ID = rec.getChr1ID();
        uint8_t chr2_ID = rec.getChr2ID();

        auto sample_name = std::string(rec.getSampleName());
        params.addSample(rec.getSampleID(), std::move(sample_name));
        auto chr1_name = std::string(rec.getChr1Name());
        params.upsertChromosome(
            chr1_ID,
            std::move(chr1_name),
            rec.getChr1Length()
        );
        auto chr2_name = std::string(rec.getChr2Name());
        params.upsertChromosome(
            chr2_ID,
            std::move(chr2_name),
            rec.getChr2Length()
        );

        auto scm_payload = genie::contact::SubcontactMatrixPayload(
            0, //TODO(yeremia): change the default parameter_set_ID
            rec.getSampleID(),
            chr1_ID,
            chr2_ID
        );

//        encode_scm(
//            codec_ID,
//            params,
//            rec,
//            scm_payload
//        );
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<ContactMatrixParameters, EncodingBlock> encode_block(
    const EncodingOptions& opt,
    std::vector<core::record::ContactRecord>& recs){

}

// ---------------------------------------------------------------------------------------------------------------------

}
}

// ---------------------------------------------------------------------------------------------------------------------