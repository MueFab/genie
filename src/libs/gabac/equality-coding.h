/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_EQUALITY_CODING_H_
#define GABAC_EQUALITY_CODING_H_

namespace util {
class DataBlock;
}
namespace genie {
namespace gabac {

void transformEqualityCoding(util::DataBlock *values, util::DataBlock *equalityFlags);

void inverseTransformEqualityCoding(util::DataBlock *values, util::DataBlock *equalityFlags);

}  // namespace gabac
}  // namespace genie

#endif  // GABAC_EQUALITY_CODING_H_
