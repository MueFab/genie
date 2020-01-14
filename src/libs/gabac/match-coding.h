/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_MATCH_CODING_H_
#define GABAC_MATCH_CODING_H_

#include <cstdint>

namespace util {
class DataBlock;
}

namespace genie {
namespace gabac {

void transformMatchCoding(uint32_t windowSize, util::DataBlock *rawValues, util::DataBlock *pointers,
                          util::DataBlock *lengths);

void inverseTransformMatchCoding(util::DataBlock *rawValues, util::DataBlock *pointers, util::DataBlock *lengths);

}  // namespace gabac
}  // namespace genie
#endif  // GABAC_MATCH_CODING_H_
