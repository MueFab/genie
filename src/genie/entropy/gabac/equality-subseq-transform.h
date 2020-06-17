/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_EQUALITY_CODING_H_
#define GABAC_EQUALITY_CODING_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/util/data-block.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

/**
 *
 * @param transformedSubseqs
 */
void transformEqualityCoding(std::vector<util::DataBlock> *const transformedSubseqs);

/**
 *
 * @param transformedSubseqs
 */
void inverseTransformEqualityCoding(std::vector<util::DataBlock> *const transformedSubseqs);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GABAC_EQUALITY_CODING_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
