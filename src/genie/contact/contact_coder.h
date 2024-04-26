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
    const bool is_intra_scm,
    // Outputs:
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
);

// ---------------------------------------------------------------------------------------------------------------------

//TODO(yeremia): create the docstring
void decode_cm_masks(
    // Inputs
    ContactMatrixParameters& cm_param,
    SubcontactMatrixParameters& scm_param,
    const SubcontactMatrixPayload& scm_payload,
    // Outputs
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
);

// ---------------------------------------------------------------------------------------------------------------------

//TODO(yeremia): create the docstring
void decode_cm_mask_payload(
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
 * boolean flag is_intra, which specifies whether the operation is intra-chromosome or not.
 *
 * @param row_ids A reference to the vector of row IDs.
 * @param col_ids A reference to the vector of column IDs.
 * @param is_intra A boolean flag indicating whether the sub-contact matrix is intra-chromosome.
 * @param row_mask A reference to the binary vector representing the row mask.
 * @param col_mask A reference to the binary vector representing the column mask.
 */
void remove_unaligned(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    bool is_intra,
    const BinVecDtype& row_mask,
    const BinVecDtype& col_mask
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Appends unaligned regions to the given row and column ID vectors based on the provided masks.
 *
 * This function iterates over the row and column ID vectors and appends elements that are not aligned
 * according to the given row and column masks. The alignment is determined by the
 * boolean flag is_intra, which specifies whether the operation is intra-chromosome or not.
 *
 * @param row_ids A reference to the vector of row IDs.
 * @param col_ids A reference to the vector of column IDs.
 * @param is_intra A boolean flag indicating whether the sub-contact matrix is intra-chromosome.
 * @param row_mask A reference to the binary vector representing the row mask.
 * @param col_mask A reference to the binary vector representing the column mask.
 */
void insert_unaligned(
    UInt64VecDtype& row_ids,
    UInt64VecDtype& col_ids,
    bool is_intra,
    BinVecDtype& row_mask,
    BinVecDtype& col_mask
);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Converts a sparse matrix representation to a dense matrix representation.
 *
 * This function takes row and column IDs along with counts and constructs a dense matrix
 * representation. It allows specifying offsets for row and column IDs to adjust the matrix
 * accordingly.
 *
 * @param row_ids A reference to the vector of row IDs.
 * @param col_ids A reference to the vector of column IDs.
 * @param counts A reference to the vector of counts corresponding to each row and column pair.
 * @param nrows The number of rows in the dense matrix.
 * @param ncols The number of columns in the dense matrix.
 * @param mat A reference to the matrix data structure to be filled.
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
 * Converts a dense matrix representation to a sparse matrix representation.
 *
 * This function takes a dense matrix and converts it into a sparse representation using
 * row and column IDs and counts. It allows specifying offsets for row and column IDs to adjust the matrix
 * accordingly.
 *
 * @param mat A reference to the dense matrix data structure.
 * @param row_ids A reference to the vector of row IDs to be filled.
 * @param col_ids A reference to the vector of column IDs to be filled.
 * @param counts A reference to the vector of counts corresponding to each row and column pair.
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

// TODO(yeremia): docstring
void comp_start_end_idx(
    // Inputs
    size_t nentries,
    size_t tile_size,
    size_t tile_idx,
    // Outputs
    size_t& start_idx,
    size_t& end_idx
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

// TODO(yeremia): docstring
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

// TOOD(yeremia): docstring
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

// TODO(yeremia): docstring
void encode_cm_tile(
    // Inputs
    const BinMatDtype& bin_mat,
    core::AlgoID codec_ID,
    // Outputs
    genie::contact::ContactMatrixTilePayload& tile_payload
);

// ---------------------------------------------------------------------------------------------------------------------


// TODO(yeremia): docstring
void decode_cm_tile(
    // Inputs
    const genie::contact::ContactMatrixTilePayload& tile_payload,
    core::AlgoID codec_ID,
    // Outputs
    BinMatDtype& bin_mat
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
    SubcontactMatrixParameters scm_param,
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
void encode_cm(
    ContactRecords& recs,
    const EncodingOptions& opt,
    EncodingBlock& block
);

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
std::tuple<ContactMatrixParameters, EncodingBlock> encode_block(
    const EncodingOptions& opt,
    std::vector<core::record::ContactRecord>& recs
);

// ---------------------------------------------------------------------------------------------------------------------

}
}

#endif  // GENIE_CONTACT_CONTACT_CODER_H
