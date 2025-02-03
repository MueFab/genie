/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_RLE_SUBSEQ_TRANSFORM_H_
#define SRC_GENIE_ENTROPY_GABAC_RLE_SUBSEQ_TRANSFORM_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include "genie/entropy/paramcabac/subsequence.h"
#include "genie/util/data_block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 * @brief
 * @param subseqCfg
 * @param transformedSubseqs
 */
void transformRleCoding(const paramcabac::Subsequence& subseqCfg,
                        std::vector<util::DataBlock>* const transformedSubseqs);

/**
 * @brief
 * @param subseqCfg
 * @param transformedSubseqs
 */
void inverseTransformRleCoding(const paramcabac::Subsequence& subseqCfg,
                               std::vector<util::DataBlock>* const transformedSubseqs);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_RLE_SUBSEQ_TRANSFORM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
