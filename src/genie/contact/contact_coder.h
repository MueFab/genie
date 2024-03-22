/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CONTACT_CONTACT_CODER_H
#define GENIE_CONTACT_CONTACT_CODER_H

#include <cstdint>
#include <list>
#include <tuple>
#include <optional>
#include <xtensor/xarray.hpp>
#include <xtensor/xtensor.hpp>
#include "genie/core/constants.h"
#include "genie/core/record/contact/record.h"
#include "contact_parameters.h"

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

using CountsDtype = uint32_t;
using BinVecDtype = xt::xtensor<bool, 1, xt::layout_type::row_major>;
using BinMatDtype = xt::xtensor<bool, 2, xt::layout_type::row_major>;
using UInt8VecDtype = xt::xtensor<uint8_t , 1, xt::layout_type::row_major>;
//using UInt8MatDtype = xt::xtensor<uint8_t , 2, xt::layout_type::row_major>;
//using Int8MatDtype = xt::xtensor<int8_t , 2, xt::layout_type::row_major>;
using UIntVecDtype = xt::xtensor<uint32_t, 1, xt::layout_type::row_major>;
using UIntMatDtype = xt::xtensor<uint32_t , 2, xt::layout_type::row_major>;
using UInt64VecDtype = xt::xtensor<uint64_t, 1, xt::layout_type::row_major>;
using Int64VecDtype = xt::xtensor<int64_t, 1, xt::layout_type::row_major>;

using VecShapeDtype = xt::xtensor<size_t, 1>::shape_type;
using MatShapeDtype = xt::xtensor<size_t, 2>::shape_type;

// ---------------------------------------------------------------------------------------------------------------------

//using ChrIDPair = std::pair<uint8_t, uint8_t>;
using ContactRecords = std::list<genie::core::record::ContactRecord>;
using SCMRecDtype = std::unordered_map<ChrIDPair, core::record::ContactRecord, chr_pair_hash>;
using IntervSCMRecDtype = std::unordered_map<uint32_t, SCMRecDtype>;

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
    ContactParameters params;
//    IntervSCMRecDtype interv_scm_recs;
};

// ---------------------------------------------------------------------------------------------------------------------

void decompose(const EncodingOptions& opt, EncodingBlock& block, std::vector<core::record::ContactRecord>& recs);

// ---------------------------------------------------------------------------------------------------------------------

void compute_mask(UInt64VecDtype& ids,BinVecDtype& mask);

// ---------------------------------------------------------------------------------------------------------------------

void compute_masks(UInt64VecDtype& row_ids,UInt64VecDtype& col_ids,bool is_intra,
                   BinVecDtype& row_mask,BinVecDtype& col_mask);

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
void remove_unaligned(UInt64VecDtype& row_ids, UInt64VecDtype& col_ids, bool is_intra,
                      BinVecDtype& row_mask, BinVecDtype& col_mask);

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
void append_unaligned(UInt64VecDtype& row_ids, UInt64VecDtype& col_ids, bool is_intra,
                      BinVecDtype& row_mask, BinVecDtype& col_mask);

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
 * @param mat A reference to the matrix data structure to be filled.
 * @param nrows The number of rows in the dense matrix.
 * @param ncols The number of columns in the dense matrix.
 * @param row_id_offset An optional offset for row IDs, defaulting to  0.
 * @param col_id_offset An optional offset for column IDs, defaulting to  0.
 */
void sparse_to_dense(UInt64VecDtype& row_ids, UInt64VecDtype& col_ids, UIntVecDtype& counts,
                     UIntMatDtype& mat, size_t nrows, size_t ncols,
                     uint64_t row_id_offset=0, uint64_t col_id_offset=0);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Converts a dense matrix representation to a sparse matrix representation.
 *
 * This function takes a dense matrix and converts it into a sparse representation using
 * row and column IDs and counts. It allows specifying offsets for row and column IDs to adjust the matrix
 * accordingly.
 *
 * @param mat A reference to the dense matrix data structure.
 * @param row_id_offset An offset for row IDs.
 * @param col_id_offset An offset for column IDs.
 * @param row_ids A reference to the vector of row IDs to be filled.
 * @param col_ids A reference to the vector of column IDs to be filled.
 * @param counts A reference to the vector of counts corresponding to each row and column pair.
 */
void dense_to_sparse(UIntMatDtype& mat, uint64_t row_id_offset, uint64_t col_id_offset,
                     UInt64VecDtype& row_ids, UInt64VecDtype& col_ids,UIntVecDtype& counts);

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
void sort_by_row_ids(UInt64VecDtype& row_ids, UInt64VecDtype& col_ids, UIntVecDtype& counts);

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
void diag_transform(UIntMatDtype& mat, DiagonalTransformMode mode);

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
void binarize_rows(UIntMatDtype& mat, BinMatDtype& bin_mat);

// ---------------------------------------------------------------------------------------------------------------------

/**
 * Inverse transform of the row binarization of a binary matrix row to a matrix of unsigned integers.
 *
 * This function takes a binary matrix row and inverse transform it into a matrix of unsigned integers.
 *
 * @param bin_mat A reference to the binary matrix row to be converted.
 * @param mat A reference to the matrix of unsigned integers where the converted row will be stored.
 */
void debinarize_row_bin(BinMatDtype& bin_mat, UIntMatDtype& mat);

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
    BinMatDtype& bin_mat,
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
 * @param bin_mat The binary matrix to store the converted byte array.
 * @param payload The byte array to be converted.
 * @param payload_len The length of the byte array.
 * @param nrows The number of rows in the binary matrix.
 * @param ncols The number of columns in the binary matrix.
 */
void bin_mat_from_bytes(
    BinMatDtype& bin_mat,
    const uint8_t* payload,
    size_t payload_len,
    size_t nrows,
    size_t ncols
);


// ---------------------------------------------------------------------------------------------------------------------

/**
 * Encodes contact parameters into a contact record.
 *
 * This function encodes the given contact parameters into a contact record. The encoding
 * process is tailored to the specific requirements of the application.
 *
 * @param params A reference to the contact parameters to be encoded.
 * @param rec A reference to the contact record where the encoded parameters will be stored.
 */
void encode_scm(
    const EncodingOptions& opt,
    ContactParameters& params,
    core::record::ContactRecord& rec
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
std::tuple<ContactParameters, EncodingBlock> encode_block(
    const EncodingOptions& opt,
    std::vector<core::record::ContactRecord>& recs
);

// ---------------------------------------------------------------------------------------------------------------------

}
}

#endif  // GENIE_CONTACT_CONTACT_CODER_H
