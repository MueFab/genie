/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of functions for applying and reversing the merge subsequence transform.
 *
 * This file contains the declarations of the transformMergeCoding and inverseTransformMergeCoding functions,
 * which implement the transformation and inverse transformation of merge coding on subsequences using the
 * GABAC (Genomic Adaptive Binary Arithmetic Coding) engine.
 *
 * @details Merge coding is a transformation applied to specific genomic data sequences to reduce redundancy
 * and improve compression. These transformations are used during encoding, and their inverses are applied during
 * the decoding process to reconstruct the original sequence.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_MERGE_SUBSEQ_TRANSFORM_H_
#define SRC_GENIE_ENTROPY_GABAC_MERGE_SUBSEQ_TRANSFORM_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include "genie/entropy/paramcabac/subsequence.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Applies the merge coding transformation to the specified subsequence.
 *
 * This function transforms the provided subsequence according to the specified
 * configuration. The merge coding transformation helps in encoding similar sequences
 * more efficiently by merging redundant data.
 *
 * @param subseqCfg The configuration of the subsequence to be transformed.
 * @param transformedSubseqs A pointer to a vector containing the transformed subsequence data blocks.
 */
void transformMergeCoding(const paramcabac::Subsequence& subseqCfg, std::vector<util::DataBlock>* transformedSubseqs);

/**
 * @brief Reverses the merge coding transformation on the specified subsequence.
 *
 * This function applies the inverse transformation to restore the original data
 * from the transformed subsequence. It uses the specified configuration to properly
 * reverse the merge coding and reconstruct the initial subsequence data.
 *
 * @param subseqCfg The configuration of the subsequence to be inverse transformed.
 * @param transformedSubseqs A pointer to a vector containing the transformed subsequence data blocks.
 */
void inverseTransformMergeCoding(const paramcabac::Subsequence& subseqCfg,
                                 std::vector<util::DataBlock>* transformedSubseqs);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_MERGE_SUBSEQ_TRANSFORM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
