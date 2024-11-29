/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for the equality subsequence transformation functions in
 * the gabac library.
 *
 * This file contains declarations for functions that perform equality
 * subsequence transformations, including forward and inverse operations. These
 * transformations are used to Compress and Decompress data sequences with equal
 * values in the gabac library.
 *
 * @details The `equality_sub_seq_transform.h` header provides two main
 * functions: `TransformEqualityCoding` and `InverseTransformEqualityCoding`.
 * Both functions operate on a vector of `DataBlock` objects, applying a
 * transformation to identify and encode or Decode runs of equal values.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_EQUALITY_SUB_SEQ_TRANSFORM_H_
#define SRC_GENIE_ENTROPY_GABAC_EQUALITY_SUB_SEQ_TRANSFORM_H_

// -----------------------------------------------------------------------------

#include <vector>  //!< Standard library include for std::vector.

#include "genie/util/data_block.h"  //!< Include for the DataBlock class used
//!< in transformations.

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Applies equality transformation to a set of data subsequences.
 *
 * This function performs equality coding on the input data blocks, compressing
 * sequences that have consecutive equal values. It modifies the provided vector
 * of `DataBlock` objects in place to reflect the compressed representation.
 *
 * @param transformed_subseqs Pointer to a vector of `DataBlock` objects to be
 * transformed.
 */
void TransformEqualityCoding(std::vector<util::DataBlock>* transformed_subseqs);

/**
 * @brief Applies the inverse equality transformation to a set of data
 * subsequences.
 *
 * This function reverses the equality coding transformation, reconstructing the
 * original sequence from the compressed data. It modifies the provided vector
 * of `DataBlock` objects in place to restore the original representation.
 *
 * @param transformed_subseqs Pointer to a vector of `DataBlock` objects to be
 * inverse transformed.
 */
void InverseTransformEqualityCoding(
    std::vector<util::DataBlock>* transformed_subseqs);

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_EQUALITY_SUB_SEQ_TRANSFORM_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
