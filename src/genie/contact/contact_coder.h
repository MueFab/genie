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
#include <xtensor/xarray.hpp>
#include <xtensor/xtensor.hpp>
#include "consts.h"
#include "contact_matrix_parameters.h"
#include "genie/core/constants.h"
#include "genie/core/record/contact/record.h"
#include "subcontact_matrix_parameters.h"
#include "subcontact_matrix_payload.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingOptions {
    uint32_t bin_size = 0;
    uint32_t tile_size = 0;
    bool multi_intervals = false;
    bool diag_transform = true;
    bool binarize = true;
    genie::core::AlgoID codec = genie::core::AlgoID::JBIG;
};

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingBlock {
    ContactMatrixParameters params;
//    IntervSCMRecDtype interv_scm_recs;
};

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Compute a mask.
 *
 * This function computes a mask based on the given IDs.
 *
 * @param ids The IDs to compute the mask from.
 * @param mask The computed mask.
 */
void compute_mask(
    UInt64VecDtype& ids,
    size_t nelems,
    BinVecDtype& mask
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Compute masks.
 *
 * This function computes masks based on the given row and column IDs.
 *
 * @param row_ids The row IDs to compute the masks from.
 * @param col_ids The column IDs to compute the masks from.
 * @param is_intra_scm A boolean indicating whether the computation is intra or not.
 * @param row_mask The computed row mask.
 * @param col_mask The computed column mask.
 */
void compute_masks(
    // Inputs
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    size_t nrows,
    size_t ncols,
    bool is_intra_scm,
    // Outputs:
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
);

// ---------------------------------------------------------------------------------------------------------------------

/**
* @brief Decodes the row and column masks from a SubcontactMatrixPayload.
*
* Decodes the row and column masks from the given SubcontactMatrixPayload and stores them in the output vectors.
* If a mask does not exist, it is set to a vector of ones.
*
* @param cm_param The contact matrix parameters.
* @param scm_param The subcontact matrix parameters.
* @param scm_payload The subcontact matrix payload to decode masks from.
* @param row_mask The output vector for the row mask.
* @param col_mask The output vector for the column mask.
*/
void decode_scm_masks(
    // Inputs
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    const SubcontactMatrixPayload& scm_payload,
    // Outputs
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Decodes a mask payload into a binary vector.
 *
 * Decodes the mask payload into a binary vector based on the transform ID.
 * If the transform ID is 0, it directly copies the mask array.
 * Otherwise, it decodes the run-length encoded entries and assigns the values accordingly.
 *
 * @param mask_payload The mask payload to decode.
 * @param num_entries The number of entries in the output mask.
 * @param mask The output binary vector for the decoded mask.
 * @throws std::runtime_error If the number of entries and the size of the mask array differ, or if the start index exceeds the number of entries.
 */
void decode_scm_mask_payload(
    // Inputs
    const SubcontactMatrixMaskPayload& mask_payload,
    size_t num_entries,
    // Outputs
    BinVecDtype& mask
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Removes unaligned regions from the given row and column ID vectors based on the provided masks.
 *
 * This function iterates over the row and column ID vectors and removes elements that contains no observation
 * according to the given row and column masks. The alignment is determined by the
 * boolean flag is_intra_tile, which specifies whether the operation is intra-chromosome or not.
 *
 * @param row_ids A reference to the vector of row IDs.
 * @param col_ids A reference to the vector of column IDs.
 * @param is_intra_tile A boolean flag indicating whether the sub-contact matrix is intra-chromosome.
 * @param row_mask A reference to the binary vector representing the row mask.
 * @param col_mask A reference to the binary vector representing the column mask.
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
 *
 * This function iterates over the row and column ID vectors and appends elements that are not aligned
 * according to the given row and column masks. The alignment is determined by the
 * boolean flag is_intra_tile, which specifies whether the operation is intra-chromosome or not.
 *
 * @param row_ids A reference to the vector of row IDs.
 * @param col_ids A reference to the vector of column IDs.
 * @param is_intra_tile A boolean flag indicating whether the sub-contact matrix is intra-chromosome.
 * @param row_mask A reference to the binary vector representing the row mask.
 * @param col_mask A reference to the binary vector representing the column mask.
 */
void insert_unaligned(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    bool is_intra_tile,
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a sparse matrix to a dense matrix.
 *
 * This function converts a sparse matrix represented by three vectors (row_ids, col_ids, counts)
 * into a dense matrix. The function first checks if the row_ids and col_ids are valid (i.e., they are
 * within the range of nrows and ncols). It also checks if there are any counts with value 0. If any
 * of these checks fail, the function throws an exception.
 *
 * @param row_ids A vector of row indices.
 * @param col_ids A vector of column indices.
 * @param counts A vector of counts.
 * @param nrows The number of rows in the dense matrix.
 * @param ncols The number of columns in the dense matrix.
 * @param mat The output dense matrix.
 *
 * @throws std::runtime_error If the row_ids or col_ids are invalid or if there are counts with value 0.
 */
void sparse_to_dense(
    // Inputs
    const UInt64VecDtype& row_ids,
    const UInt64VecDtype& col_ids,
    const UIntVecDtype& counts,
    size_t nrows,
    size_t ncols,
    // Outputs
    UIntMatDtype& mat
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a dense matrix to a sparse matrix.
 *
 * This function converts a dense matrix into a sparse matrix represented by three vectors (row_ids, col_ids, counts).
 * The function first creates a mask of the matrix where the elements are greater than 0. It then finds the indices of the mask
 * where the elements are true. The function then resizes the output vectors to the number of entries and fills them with the row indices,
 * column indices, and counts from the dense matrix.
 *
 *  @param mat The input dense matrix.
 *  @param row_ids The output vector of row indices.
 *  @param col_ids The output vector of column indices.
 *  @param counts The output vector of counts.
 */
void dense_to_sparse(
    // Inputs
    const UIntMatDtype& mat,
    // Outputs
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Sorts the given row and column ID vectors by row IDs.
 *
 * This function sorts the row and column ID vectors based on the row IDs, ensuring that
 * the elements are ordered according to the row IDs.
 *
 * @param row_ids A reference to the vector of row IDs to be sorted.
 * @param col_ids A reference to the vector of column IDs to be sorted.
 * @param counts A reference to the vector of counts to be sorted accordingly.
 */
void sort_by_row_ids(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    UIntVecDtype& counts
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Applies an inverse diagonal transform to a matrix.
 *
 * This function applies an inverse diagonal transform to a matrix. The transform mode determines the type of transform
 * applied. If the mode is NONE, the function does nothing, otherwise applies diagonal transformation according to the mode
 *
 *   @param mat The input matrix.
 *   @param mode The transform mode.
 */
void inverse_diag_transform(
    UIntMatDtype& mat,
    DiagonalTransformMode mode
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Applies a diagonal transformation to the given matrix.
 *
 * This function applies a specified diagonal transformation mode to the matrix,
 * modifying it in place.
 *
 * @param mat A reference to the matrix data structure to be transformed.
 * @param mode The diagonal transformation mode to apply.
 */
void diag_transform(
    UIntMatDtype& mat,
    DiagonalTransformMode mode
);


// ---------------------------------------------------------------------------------------------------------------------

/**
 * Inverse transform of the row binarization of a binary matrix row to a matrix of unsigned integers.
 *
 * This function takes a binary matrix row and inverse transform it into a matrix of unsigned integers.
 *
 * @param bin_mat A reference to the binary matrix row to be converted.
 * @param mat A reference to the matrix of unsigned integers where the converted row will be stored.
 */
void inverse_transform_row_bin(
    // Inputs
    const BinMatDtype& bin_mat,
    // Outputs
    UIntMatDtype& mat
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Transform the given matrix using row binarization.
 *
 * This function binarizes the rows of the matrix, setting each element to  1 if it is non-zero,
 * and  0 otherwise. The result is stored in a binary matrix data structure.
 *
 * @param mat A reference to the matrix data structure to be binarized.
 * @param bin_mat A reference to the binary matrix data structure to store the result.
 */
void transform_row_bin(
    // Inputs
    const UIntMatDtype& mat,
    // Outputs
    BinMatDtype& bin_mat
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Computes the start and end indices for a given tile.
 *
 * This function computes the start and end indices for a given tile. The tile is defined by its index and size. The
 * function takes into account the total number of entries and ensures that the end index does not exceed the total number
 * of entries.
 *
 *   @param num_entries The total number of entries.
 *   @param tile_size The size of the tile.
 *   @param tile_idx The index of the tile.
 *   @param start_idx The output start index.
 *   @param end_idx The output end index.
 */
void comp_start_end_ids(
    // Inputs
    size_t num_entries,
    size_t tile_size,
    size_t tile_idx,
    // Outputs
    size_t& start_idx,
    size_t& end_idx
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a binary matrix to a byte array.
 *
 * This function takes a binary matrix (bin_mat) and converts it into a byte array (payload).
 * The length of the byte array is stored in payload_len.
 *
 * @param bin_mat The binary matrix to be converted.
 * @param payload The byte array to store the converted binary matrix.
 * @param payload_len The length of the byte array.
 */
void bin_mat_to_bytes(
    // Inputs
    const BinMatDtype& bin_mat,
    // Outputs
    uint8_t** payload,
    size_t& payload_len
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a byte array to a binary matrix.
 *
 * This function takes a byte array (payload) and converts it into a binary matrix (bin_mat).
 * The byte array is assumed to be of length (payload_len) and represents a binary matrix with
 * (nrows) rows and (ncols) columns.
 *
 * @param payload The byte array to be converted.
 * @param payload_len The length of the byte array.
 * @param nrows The number of rows in the binary matrix.
 * @param ncols The number of columns in the binary matrix.
 * @param bin_mat The binary matrix to store the converted byte array.
 */
void bin_mat_from_bytes(
    // Inputs
    const uint8_t* payload,
    size_t payload_len,
    size_t nrows,
    size_t ncols,
    // Outputs
    BinMatDtype& bin_mat
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Encodes a binary matrix into a ContactMatrixTilePayload.
 *
 * Encodes the binary matrix using the specified codec ID and stores the result in the output tile payload.
 * Currently, only JBIG codec is supported.
 *
 * @param bin_mat The binary matrix to encode.
 * @param codec_ID The codec ID to use for encoding.
 * @param tile_payload The output ContactMatrixTilePayload object.
 * @throws std::runtime_error If the codec ID is not JBIG.
 */
void encode_cm_tile(
    // Inputs
    const BinMatDtype& bin_mat,
    core::AlgoID codec_ID,
    // Outputs
    genie::contact::ContactMatrixTilePayload& tile_payload
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Decodes a ContactMatrixTilePayload into a binary matrix.
 *
 * Decodes the tile payload using the specified codec ID and stores the result in the output binary matrix.
 * Currently, only JBIG codec is supported.
 *
 * @param tile_payload The ContactMatrixTilePayload object to decode.
 * @param codec_ID The codec ID to use for decoding.
 * @param bin_mat The output binary matrix.
 * @throws std::runtime_error If the codec ID is not JBIG.
 */
void decode_cm_tile(
    // Inputs
    const genie::contact::ContactMatrixTilePayload& tile_payload,
    core::AlgoID codec_ID,
    // Outputs
    BinMatDtype& bin_mat
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Converts a sparse matrix to a lower resolution version by pooling values.
 *
 * This function takes a sparse matrix represented by three vectors: tile_row_ids,
 * tile_col_ids, and tile_counts. It then converts this sparse matrix to a lower
 * resolution version by pooling values based on the bin_size_mult parameter. The
 * output can be sorted based on the sort_output parameter.
 *
 * @param tile_row_ids Vector containing the row IDs of the sparse matrix.
 * @param tile_col_ids Vector containing the column IDs of the sparse matrix.
 * @param tile_counts Vector containing the counts of the sparse matrix.
 * @param bin_size_mult The multiplier used to determine the bin size for pooling.
 * @param sort_output Flag indicating whether to sort the output sparse matrix.
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
 *
 * This function sorts the input sparse matrix in-place based on the row and column IDs.
 * If the original order is not sorted, it reorders the IDs and counts accordingly.
 *
 * @param tile_row_ids Vector containing the row IDs of the sparse matrix.
 * @param tile_col_ids Vector containing the column IDs of the sparse matrix.
 * @param tile_counts Vector containing the counts of the sparse matrix.
 */
void sort_sparse_mat_inplace(
    UInt64VecDtype& tile_row_ids,
    UInt64VecDtype& tile_col_ids,
    UIntVecDtype& tile_counts
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Decode a Subcontact Matrix Payload.
 *
 * This function decodes a Subcontact Matrix Payload into a Contact Matrix Parameters object and a Contact Record object.
 *
 * @param scm_payload The Subcontact Matrix Payload to decode.
 * @param params The Contact Matrix Parameters object to store the decoded parameters.
 * @param rec The Contact Record object to store the decoded record.
 */
void decode_scm(
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    genie::contact::SubcontactMatrixPayload& scm_payload,
    core::record::ContactRecord& rec,
    uint32_t bin_size_mult=1
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Encodes a subcontact matrix.
 *
 * This function encodes a subcontact matrix from a contact matrix. The function takes a contact matrix parameters object,
 * a contact record, and outputs a subcontact matrix parameters object and a subcontact matrix payload. The function also
 * takes several options to remove unaligned regions, transform masks, enable diagonal transform, enable binarization,
 * and specify the codec ID. The default values for these options are true, true, true, true, and core::AlgoID::JBIG,
 * respectively.
 *
 *     @param cm_param The contact matrix parameters.
 *     @param rec The contact record.
 *     @param scm_param The subcontact matrix parameters.
 *     @param scm_payload The subcontact matrix payload.
 *     @param remove_unaligned_region A flag indicating whether to remove unaligned regions. Default is true.
 *     @param transform_mask A flag indicating whether to transform the mask. Default is true.
 *     @param ena_diag_transform A flag indicating whether to enable diagonal transform. Default is true.
 *     @param ena_binarization A flag indicating whether to enable binarization. Default is true.
 *     @param codec_ID The codec ID. Default is core::AlgoID::JBIG.
 */
void encode_scm(
    // Inputs
    ContactMatrixParameters& cm_param,
    core::record::ContactRecord& rec,
    // Outputs
    SubcontactMatrixParameters& scm_param,
    genie::contact::SubcontactMatrixPayload& scm_payload,
    // Options
    bool remove_unaligned_region=true,
    bool transform_mask=true,
    bool ena_diag_transform=true,
    bool ena_binarization=true,
    bool norm_as_weight=true,
    bool multiplicative_norm=true,
    core::AlgoID codec_ID=core::AlgoID::JBIG
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Encodes a block of data according to the specified encoding options.
 *
 * This function takes a block of data and encodes it according to the provided encoding options.
 * The encoding process is crucial for the data's integrity and security.
 *
 * @param opt A reference to the encoding options that dictate how the block should be encoded.
 * @param block A reference to the encoding block that contains the data to be encoded.
 */
//void encode_cm(
//    ContactRecords& recs,
//    const EncodingOptions& opt,
//    EncodingBlock& block
//);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Encodes a set of contact records according to the specified encoding options.
 *
 * This function takes a set of contact records and encodes them according to the provided encoding options.
 * The encoded parameters and the encoding block are returned as a tuple.
 *
 * @param opt A reference to the encoding options that dictate how the records should be encoded.
 * @param recs A reference to the vector of contact records to be encoded.
 * @return A tuple containing the encoded contact parameters and the encoding block.
 */
//std::tuple<ContactMatrixParameters, EncodingBlock> encode_block(
//    const EncodingOptions& opt,
//    std::vector<core::record::ContactRecord>& recs
//);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_CONTACT_CODER_H

// ---------------------------------------------------------------------------------------------------------------------