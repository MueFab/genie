/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of functions for applying and reversing the match
 * subsequence transform.
 *
 * This file contains the declarations of the TransformMatchCoding and
 * InverseTransformMatchCoding functions, which implement the transformation and
 * inverse transformation of match coding on subsequences using the GABAC
 * (Genomic Adaptive Binary Arithmetic Coding) engine.
 *
 * @details Match coding is a transformation applied to specific genomic data
 * sequences to reduce redundancy by encoding repeated sequences more compactly.
 * These transformations are used during the encoding process, and their
 * inverses are applied during decoding to reconstruct the original sequence.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_MATCH_SUB_SEQ_TRANSFORM_H_
#define SRC_GENIE_ENTROPY_GABAC_MATCH_SUB_SEQ_TRANSFORM_H_

// -----------------------------------------------------------------------------

#include <vector>

#include "genie/entropy/paramcabac/subsequence.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Applies the match coding transformation to the specified subsequence.
 *
 * This function transforms the provided subsequence according to the specified
 * configuration. The match coding transformation compresses repeated sequences
 * by encoding them in a more compact form, thereby reducing the overall Size of
 * the data.
 *
 * @param sub_sequence_cfg The configuration of the subsequence to be
 * transformed.
 * @param transformed_sub_sequences A pointer to a vector containing the
 * transformed subsequence data blocks.
 */
void TransformMatchCoding(
    const paramcabac::Subsequence& sub_sequence_cfg,
    std::vector<util::DataBlock>* transformed_sub_sequences);

/**
 * @brief Reverses the match coding transformation on the specified subsequence.
 *
 * This function applies the inverse transformation to restore the original data
 * from the transformed subsequence. The inverse transformation reconstructs the
 * original sequence using the match coding configuration and restores any
 * repeated data segments to their original positions.
 *
 * @param transformed_sub_sequences A pointer to a vector containing the
 * transformed subsequence data blocks.
 */
void InverseTransformMatchCoding(
    std::vector<util::DataBlock>* transformed_sub_sequences);

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_MATCH_SUB_SEQ_TRANSFORM_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
