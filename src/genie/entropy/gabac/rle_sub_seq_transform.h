/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Provides functions for Run-Length Encoding (RLE) transformations and
 * their inverses.
 *
 * This file contains the declarations of functions that perform the Run-Length
 * Encoding (RLE) transformations and inverse transformations for subsequences
 * in the GENIE framework. The transformations are applied to a series of
 * symbols and are used for data compression within the entropy encoding
 * process. The main functions provided are `TransformRleCoding` and
 * `InverseTransformRleCoding`.
 *
 * @details RLE is a simple form of data compression in which runs of data
 * (consecutive repeated symbols) are stored as a single symbol and count,
 * rather than as the original run. This file implements both the forward and
 * reverse transformations to apply and revert this compression technique.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_RLE_SUB_SEQ_TRANSFORM_H_
#define SRC_GENIE_ENTROPY_GABAC_RLE_SUB_SEQ_TRANSFORM_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <vector>

#include "genie/entropy/paramcabac/subsequence.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Applies Run-Length Encoding (RLE) transformation to the given
 * subsequence.
 *
 * This function performs the RLE transformation on the input subsequence,
 * compressing consecutive repeated symbols into single symbols with associated
 * run lengths. The result is stored in the provided `transformed_subseqs`
 * vector.
 *
 * @param subseq_cfg Configuration of the subsequence to be transformed, which
 * includes the parameters needed to control the RLE transformation.
 * @param transformed_subseqs A pointer to a vector of DataBlocks where the
 * transformed subsequences will be stored.
 */
void TransformRleCoding(const paramcabac::Subsequence& subseq_cfg,
                        std::vector<util::DataBlock>* transformed_subseqs);

/**
 * @brief Reverts the Run-Length Encoding (RLE) transformation for the given
 * subsequence.
 *
 * This function performs the inverse of the RLE transformation, reconstructing
 * the original subsequence from the compressed form stored in
 * `transformed_subseqs`. It restores the data to its original uncompressed
 * form.
 *
 * @param subseq_cfg Configuration of the subsequence that was transformed,
 * which includes the parameters used in the RLE transformation.
 * @param transformed_subseqs A pointer to a vector of DataBlocks where the
 * transformed subsequences are stored.
 */
void InverseTransformRleCoding(
    const paramcabac::Subsequence& subseq_cfg,
    std::vector<util::DataBlock>* transformed_subseqs);

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_RLE_SUB_SEQ_TRANSFORM_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
