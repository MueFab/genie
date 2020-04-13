/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_RLE_CODING_H_
#define GABAC_RLE_CODING_H_

#include <cstdint>

#include <genie/util/data-block.h>
namespace genie {
namespace entropy {
namespace gabac {

void transformRleCoding(uint64_t guard, util::DataBlock *rawValues, util::DataBlock *lengths);

void inverseTransformRleCoding(uint64_t guard, util::DataBlock *rawValues, util::DataBlock *lengths);
}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_RLE_CODING_H_
