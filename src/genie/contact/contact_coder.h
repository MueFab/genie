/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CONTACT_CONTACT_CODER_H
#define GENIE_CONTACT_CONTACT_CODER_H

#include <cstdint>
#include <list>
#include <optional>
#include <tuple>
#include "consts.h"
#include "contact_matrix_parameters.h"
#include "genie/core/constants.h"
#include "genie/core/contact_record/record.h"
#include "subcontact_matrix_parameters.h"
#include "subcontact_matrix_payload.h"
#include "contact_types.h"
#include <genie/backend/backend.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::contact {

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingOptions {
    uint32_t tile_size = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingBlock {
    ContactMatrixParameters params;
};

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Extract run length encoding relevant information from mask.
 */
void set_rle_information_from_mask(
    RunLengthEncodingData& rleData,
    const BinVecDtype& scm_mask
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Compute a mask.
 */
inline void compute_mask(
    UInt64VecDtype& ids,
    size_t nelems,
    BinVecDtype& mask
) {
    ::genie::backend::compute_mask(ids, nelems, mask);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Compute masks.
 */
void compute_masks(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    size_t nrows,
    size_t ncols,
    bool is_intra_scm,
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
);

// ---------------------------------------------------------------------------------------------------------------------

/**
* @brief Decodes the row and column masks from a SubcontactMatrixPayload.
*/
void decode_scm_masks(
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    const SubcontactMatrixPayload& scm_payload,
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Decodes a mask payload_ into a binary vector.
 */
void decode_scm_mask_payload(
    const SubcontactMatrixMaskPayload& mask_payload,
    size_t num_entries,
    BinVecDtype& mask
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Removes unaligned regions from the given row and column ID vectors based on the provided masks.
 */
void remove_unaligned(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    bool is_intra_tile,
    const BinVecDtype& row_mask,
    const BinVecDtype& col_mask
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Appends unaligned regions to the given row and column ID vectors based on the provided masks.
 */
void insert_unaligned(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    bool is_intra_tile,
    const BinVecDtype& row_mask,
    const BinVecDtype& col_mask
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a sparse matrix to a dense matrix.
 */
void sparse_to_dense(
    const UInt64VecDtype& row_ids,
    const UInt64VecDtype& col_ids,
    const UIntVecDtype& counts,
    size_t nrows,
    size_t ncols,
    UIntMatDtype& mat
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a dense matrix to a sparse matrix.
 */
void dense_to_sparse(
    const UIntMatDtype& mat,
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Sorts the given row and column ID vectors by row IDs.
 */
[[maybe_unused]] inline void sort_by_row_ids(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts
) {
    ::genie::backend::sort_sparse_mat_inplace(row_ids, col_ids, counts);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Applies an inverse diagonal transform to a matrix.
 */
void inverse_diag_transform(
    UIntMatDtype& mat,
    DiagonalTransformMode mode
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Applies a diagonal transformation to the given matrix.
 */
void diag_transform(
    UIntMatDtype& mat,
    DiagonalTransformMode mode
);


// ---------------------------------------------------------------------------------------------------------------------

/**
 * Inverse transform of the row binarization of a binary matrix row to a matrix of unsigned integers.
 */
void inverse_transform_row_bin(
    const BinMatDtype& bin_mat,
    UIntMatDtype& mat
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Transform the given matrix using row binarization.
 */
void transform_row_bin(
    const UIntMatDtype& mat,
    BinMatDtype& bin_mat
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Computes the start and end indices for a given tile.
 */
void comp_start_end_ids(
    size_t num_entries,
    size_t tile_size,
    size_t tile_idx,
    size_t& start_idx,
    size_t& end_idx
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a binary matrix to a byte array.
 */
inline void bin_mat_to_bytes(
    const BinMatDtype& bin_mat,
    uint8_t** payload,
    size_t& payload_len
) {
    ::genie::backend::bin_mat_to_bytes(bin_mat, payload, payload_len);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a byte array to a binary matrix.
 */
inline void bin_mat_from_bytes(
    const uint8_t* payload,
    size_t payload_len,
    size_t nrows,
    size_t ncols,
    BinMatDtype& bin_mat
) {
    ::genie::backend::bin_mat_from_bytes(payload, payload_len, nrows, ncols, bin_mat);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Encodes a binary matrix into a ContactMatrixTilePayload.
 */
void encode_cm_tile(
    const BinMatDtype& bin_mat,
    core::AlgoID codec_ID,
    genie::contact::ContactMatrixTilePayload& tile_payload
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Decodes a ContactMatrixTilePayload into a binary matrix.
 */
void decode_cm_tile(
    const genie::contact::ContactMatrixTilePayload& tile_payload,
    core::AlgoID codec_ID,
    BinMatDtype& bin_mat
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a sparse matrix to a lower resolution version by pooling values.
 */
void conv_noop_on_sparse_mat(
    UInt64VecDtype& tile_row_ids,
    UInt64VecDtype& tile_col_ids,
    UIntVecDtype& tile_counts,
    uint32_t bin_size_mult,
    bool sort_output=false
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Sorts a sparse matrix in-place based on row and column IDs.
 */
inline void sort_sparse_mat_inplace(
    UInt64VecDtype& tile_row_ids,
    UInt64VecDtype& tile_col_ids,
    UIntVecDtype& tile_counts
) {
    ::genie::backend::sort_sparse_mat_inplace(tile_row_ids, tile_col_ids, tile_counts);
}

// ---------------------------------------------------------------------------------------------------------------------

void decode_scm(
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    genie::contact::SubcontactMatrixPayload& scm_payload,
    core::record::ContactRecord& rec,
    uint32_t bin_size_mult=1
);

// ---------------------------------------------------------------------------------------------------------------------

void encode_scm(
    ContactMatrixParameters& cm_param,
    core::record::ContactRecord& rec,
    SubcontactMatrixParameters& scm_param,
    genie::contact::SubcontactMatrixPayload& scm_payload,
    bool remove_unaligned_region=true,
    bool transform_mask=true,
    bool ena_diag_transform=true,
    bool ena_binarization=true,
    bool norm_as_weight=true,
    bool multiplicative_norm=true,
    core::AlgoID codec_ID=core::AlgoID::JBIG
);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::contact

#endif  // GENIE_CONTACT_CONTACT_CODER_H