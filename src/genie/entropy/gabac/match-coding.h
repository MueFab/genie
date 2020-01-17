/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_MATCH_CODING_H_
#define GABAC_MATCH_CODING_H_

#include <cstdint>

namespace genie {
namespace util {
class DataBlock;
}
}  // namespace genie

namespace genie {
namespace entropy {
namespace gabac {

void transformMatchCoding(uint32_t windowSize, util::DataBlock *rawValues, util::DataBlock *pointers,
                          util::DataBlock *lengths);

void inverseTransformMatchCoding(util::DataBlock *rawValues, util::DataBlock *pointers, util::DataBlock *lengths);

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_MATCH_CODING_H_
