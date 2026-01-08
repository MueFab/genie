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

#if defined(GENIE_CONTACT_BACKEND_XTENSOR)
#include "contact_coder_xtensor.h"
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
#include "contact_coder_eigen.h"
#else
#include "contact_coder_std.h"
#endif

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::contact {

#if defined(GENIE_CONTACT_BACKEND_XTENSOR)
namespace backend = detail::xtensor;
#elif defined(GENIE_CONTACT_BACKEND_EIGEN)
namespace backend = detail::eigen;
#else
namespace backend = detail::std_lib;
#endif

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
inline void set_rle_information_from_mask(
    RunLengthEncodingData& rleData,
    const BinVecDtype& scm_mask
) {
    backend::set_rle_information_from_mask(rleData, scm_mask);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Compute a mask.
 */
inline void compute_mask(
    UInt64VecDtype& ids,
    size_t nelems,
    BinVecDtype& mask
) {
    backend::compute_mask(ids, nelems, mask);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Compute masks.
 */
inline void compute_masks(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    size_t nrows,
    size_t ncols,
    bool is_intra_scm,
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
) {
    backend::compute_masks(row_ids, col_ids, nrows, ncols, is_intra_scm, row_mask, col_mask);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
* @brief Decodes the row and column masks from a SubcontactMatrixPayload.
*/
inline void decode_scm_masks(
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    const SubcontactMatrixPayload& scm_payload,
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
) {
    backend::decode_scm_masks(cm_param, scm_param, scm_payload, row_mask, col_mask);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Decodes a mask payload_ into a binary vector.
 */
inline void decode_scm_mask_payload(
    const SubcontactMatrixMaskPayload& mask_payload,
    size_t num_entries,
    BinVecDtype& mask
) {
    backend::decode_scm_mask_payload(mask_payload, num_entries, mask);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Removes unaligned regions from the given row and column ID vectors based on the provided masks.
 */
inline void remove_unaligned(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    bool is_intra_tile,
    const BinVecDtype& row_mask,
    const BinVecDtype& col_mask
) {
    backend::remove_unaligned(row_ids, col_ids, is_intra_tile, row_mask, col_mask);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Appends unaligned regions to the given row and column ID vectors based on the provided masks.
 */
inline void insert_unaligned(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    bool is_intra_tile,
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
) {
    backend::insert_unaligned(row_ids, col_ids, is_intra_tile, row_mask, col_mask);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a sparse matrix to a dense matrix.
 */
inline void sparse_to_dense(
    const UInt64VecDtype& row_ids,
    const UInt64VecDtype& col_ids,
    const UIntVecDtype& counts,
    size_t nrows,
    size_t ncols,
    UIntMatDtype& mat
) {
    backend::sparse_to_dense(row_ids, col_ids, counts, nrows, ncols, mat);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a dense matrix to a sparse matrix.
 */
inline void dense_to_sparse(
    const UIntMatDtype& mat,
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts
) {
    backend::dense_to_sparse(mat, row_ids, col_ids, counts);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Sorts the given row and column ID vectors by row IDs.
 */
[[maybe_unused]] inline void sort_by_row_ids(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts
) {
    backend::sort_by_row_ids(row_ids, col_ids, counts);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Applies an inverse diagonal transform to a matrix.
 */
inline void inverse_diag_transform(
    UIntMatDtype& mat,
    DiagonalTransformMode mode
) {
    backend::inverse_diag_transform(mat, mode);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Applies a diagonal transformation to the given matrix.
 */
inline void diag_transform(
    UIntMatDtype& mat,
    DiagonalTransformMode mode
) {
    backend::diag_transform(mat, mode);
}


// ---------------------------------------------------------------------------------------------------------------------

/**
 * Inverse transform of the row binarization of a binary matrix row to a matrix of unsigned integers.
 */
inline void inverse_transform_row_bin(
    const BinMatDtype& bin_mat,
    UIntMatDtype& mat
) {
    backend::inverse_transform_row_bin(bin_mat, mat);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Transform the given matrix using row binarization.
 */
inline void transform_row_bin(
    const UIntMatDtype& mat,
    BinMatDtype& bin_mat
) {
    backend::transform_row_bin(mat, bin_mat);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Computes the start and end indices for a given tile.
 */
inline void comp_start_end_ids(
    size_t num_entries,
    size_t tile_size,
    size_t tile_idx,
    size_t& start_idx,
    size_t& end_idx
) {
    backend::comp_start_end_ids(num_entries, tile_size, tile_idx, start_idx, end_idx);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a binary matrix to a byte array.
 */
inline void bin_mat_to_bytes(
    const BinMatDtype& bin_mat,
    uint8_t** payload,
    size_t& payload_len
) {
    backend::bin_mat_to_bytes(bin_mat, payload, payload_len);
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
    backend::bin_mat_from_bytes(payload, payload_len, nrows, ncols, bin_mat);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Encodes a binary matrix into a ContactMatrixTilePayload.
 */
inline void encode_cm_tile(
    const BinMatDtype& bin_mat,
    core::AlgoID codec_ID,
    genie::contact::ContactMatrixTilePayload& tile_payload
) {
    backend::encode_cm_tile(bin_mat, codec_ID, tile_payload);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Decodes a ContactMatrixTilePayload into a binary matrix.
 */
inline void decode_cm_tile(
    const genie::contact::ContactMatrixTilePayload& tile_payload,
    core::AlgoID codec_ID,
    BinMatDtype& bin_mat
) {
    backend::decode_cm_tile(tile_payload, codec_ID, bin_mat);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a sparse matrix to a lower resolution version by pooling values.
 */
inline void conv_noop_on_sparse_mat(
    UInt64VecDtype& tile_row_ids,
    UInt64VecDtype& tile_col_ids,
    UIntVecDtype& tile_counts,
    uint32_t bin_size_mult,
    bool sort_output=false
) {
    backend::conv_noop_on_sparse_mat(tile_row_ids, tile_col_ids, tile_counts, bin_size_mult, sort_output);
}

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Sorts a sparse matrix in-place based on row and column IDs.
 */
inline void sort_sparse_mat_inplace(
    UInt64VecDtype& tile_row_ids,
    UInt64VecDtype& tile_col_ids,
    UIntVecDtype& tile_counts
) {
    backend::sort_sparse_mat_inplace(tile_row_ids, tile_col_ids, tile_counts);
}

// ---------------------------------------------------------------------------------------------------------------------

inline void decode_scm(
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    genie::contact::SubcontactMatrixPayload& scm_payload,
    core::record::ContactRecord& rec,
    uint32_t bin_size_mult=1
) {
    backend::decode_scm(cm_param, scm_param, scm_payload, rec, bin_size_mult);
}

// ---------------------------------------------------------------------------------------------------------------------

inline void encode_scm(
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
) {
    backend::encode_scm(
        cm_param,
        rec,
        scm_param,
        scm_payload,
        remove_unaligned_region,
        transform_mask,
        ena_diag_transform,
        ena_binarization,
        norm_as_weight,
        multiplicative_norm,
        codec_ID
    );
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::contact

#endif  // GENIE_CONTACT_CONTACT_CODER_H

// ---------------------------------------------------------------------------------------------------------------------