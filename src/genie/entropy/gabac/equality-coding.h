/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_EQUALITY_CODING_H_
#define GABAC_EQUALITY_CODING_H_

namespace genie {
namespace util {
class DataBlock;
}
}  // namespace genie
namespace genie {
namespace entropy {
namespace gabac {

void transformEqualityCoding(util::DataBlock *values, util::DataBlock *equalityFlags);

void inverseTransformEqualityCoding(util::DataBlock *values, util::DataBlock *equalityFlags);

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_EQUALITY_CODING_H_
