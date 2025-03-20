/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of functions for applying and reversing the merge
 * subsequence transform.
 *
 * This file contains the declarations of the TransformMergeCoding and
 * InverseTransformMergeCoding functions, which implement the transformation and
 * inverse transformation of merge coding on subsequences using the GABAC
 * (Genomic Adaptive Binary Arithmetic Coding) engine.
 *
 * @details Merge coding is a transformation applied to specific genomic data
 * sequences to reduce redundancy and improve compression. These transformations
 * are used during encoding, and their inverses are applied during the decoding
 * process to reconstruct the original sequence.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_MERGE_SUB_SEQ_TRANSFORM_H_
#define SRC_GENIE_ENTROPY_GABAC_MERGE_SUB_SEQ_TRANSFORM_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <vector>

#include "genie/entropy/paramcabac/subsequence.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Applies the merge coding transformation to the specified subsequence.
 *
 * This function transforms the provided subsequence according to the specified
 * configuration. The merge coding transformation helps in encoding similar
 * sequences more efficiently by merging redundant data.
 *
 * @param sub_seq_cfg The configuration of the subsequence to be transformed.
 * @param transformed_sub_seqs A pointer to a vector containing the transformed
 * subsequence data blocks.
 */
void TransformMergeCoding(const paramcabac::Subsequence& sub_seq_cfg,
                          std::vector<util::DataBlock>* transformed_sub_seqs);

/**
 * @brief Reverses the merge coding transformation on the specified subsequence.
 *
 * This function applies the inverse transformation to restore the original data
 * from the transformed subsequence. It uses the specified configuration to
 * properly reverse the merge coding and reconstruct the initial subsequence
 * data.
 *
 * @param sub_seq_cfg The configuration of the subsequence to be inverse
 * transformed.
 * @param transformed_sub_seqs A pointer to a vector containing the transformed
 * subsequence data blocks.
 */
void InverseTransformMergeCoding(
    const paramcabac::Subsequence& sub_seq_cfg,
    std::vector<util::DataBlock>* transformed_sub_seqs);

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_MERGE_SUB_SEQ_TRANSFORM_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------