/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_EQUALITY_SUBSEQ_TRANSFORM_H_
#define SRC_GENIE_ENTROPY_GABAC_EQUALITY_SUBSEQ_TRANSFORM_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <vector>
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

/**
 * @brief
 * @param transformedSubseqs
 */
void transformEqualityCoding(std::vector<util::DataBlock> *transformedSubseqs);

/**
 * @brief
 * @param transformedSubseqs
 */
void inverseTransformEqualityCoding(std::vector<util::DataBlock> *transformedSubseqs);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_EQUALITY_SUBSEQ_TRANSFORM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
