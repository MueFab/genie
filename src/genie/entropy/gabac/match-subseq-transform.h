/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of functions for applying and reversing the match subsequence transform.
 *
 * This file contains the declarations of the transformMatchCoding and inverseTransformMatchCoding functions,
 * which implement the transformation and inverse transformation of match coding on subsequences using the
 * GABAC (Genomic Adaptive Binary Arithmetic Coding) engine.
 *
 * @details Match coding is a transformation applied to specific genomic data sequences to reduce redundancy
 * by encoding repeated sequences more compactly. These transformations are used during the encoding process,
 * and their inverses are applied during decoding to reconstruct the original sequence.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_MATCH_SUBSEQ_TRANSFORM_H_
#define SRC_GENIE_ENTROPY_GABAC_MATCH_SUBSEQ_TRANSFORM_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include "genie/entropy/paramcabac/subsequence.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief Applies the match coding transformation to the specified subsequence.
 *
 * This function transforms the provided subsequence according to the specified
 * configuration. The match coding transformation compresses repeated sequences
 * by encoding them in a more compact form, thereby reducing the overall size of the data.
 *
 * @param subseqCfg The configuration of the subsequence to be transformed.
 * @param transformedSubseqs A pointer to a vector containing the transformed subsequence data blocks.
 */
void transformMatchCoding(const paramcabac::Subsequence &subseqCfg, std::vector<util::DataBlock> *transformedSubseqs);

/**
 * @brief Reverses the match coding transformation on the specified subsequence.
 *
 * This function applies the inverse transformation to restore the original data
 * from the transformed subsequence. The inverse transformation reconstructs the
 * original sequence using the match coding configuration and restores any repeated
 * data segments to their original positions.
 *
 * @param transformedSubseqs A pointer to a vector containing the transformed subsequence data blocks.
 */
void inverseTransformMatchCoding(std::vector<util::DataBlock> *transformedSubseqs);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_MATCH_SUBSEQ_TRANSFORM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
