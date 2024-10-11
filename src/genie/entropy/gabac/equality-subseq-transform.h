/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for the equality subsequence transformation functions in the gabac library.
 *
 * This file contains declarations for functions that perform equality subsequence transformations,
 * including forward and inverse operations. These transformations are used to compress and decompress
 * data sequences with equal values in the gabac library.
 *
 * @details The `equality_subseq_transform.h` header provides two main functions:
 * `transformEqualityCoding` and `inverseTransformEqualityCoding`. Both functions operate on a vector
 * of `DataBlock` objects, applying a transformation to identify and encode or decode runs of equal values.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_EQUALITY_SUBSEQ_TRANSFORM_H_
#define SRC_GENIE_ENTROPY_GABAC_EQUALITY_SUBSEQ_TRANSFORM_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>                 //!< Standard library include for std::vector.
#include "genie/util/data-block.h"  //!< Include for the DataBlock class used in transformations.

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Applies equality transformation to a set of data subsequences.
 *
 * This function performs equality coding on the input data blocks, compressing sequences
 * that have consecutive equal values. It modifies the provided vector of `DataBlock` objects
 * in place to reflect the compressed representation.
 *
 * @param transformedSubseqs Pointer to a vector of `DataBlock` objects to be transformed.
 */
void transformEqualityCoding(std::vector<util::DataBlock> *transformedSubseqs);

/**
 * @brief Applies the inverse equality transformation to a set of data subsequences.
 *
 * This function reverses the equality coding transformation, reconstructing the original
 * sequence from the compressed data. It modifies the provided vector of `DataBlock` objects
 * in place to restore the original representation.
 *
 * @param transformedSubseqs Pointer to a vector of `DataBlock` objects to be inverse transformed.
 */
void inverseTransformEqualityCoding(std::vector<util::DataBlock> *transformedSubseqs);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_EQUALITY_SUBSEQ_TRANSFORM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
