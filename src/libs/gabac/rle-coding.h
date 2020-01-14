/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_RLE_CODING_H_
#define GABAC_RLE_CODING_H_

#include <cstdint>

#include "util/data-block.h"
namespace genie {
namespace gabac {

void transformRleCoding(uint64_t guard, util::DataBlock *rawValues, util::DataBlock *lengths);

void inverseTransformRleCoding(uint64_t guard, util::DataBlock *rawValues, util::DataBlock *lengths);

}  // namespace gabac
}  // namespace genie
#endif  // GABAC_RLE_CODING_H_
