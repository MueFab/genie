/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_LUT_TRANSFORM_H_
#define GABAC_LUT_TRANSFORM_H_

namespace genie {
namespace util {
class DataBlock;
}
}  // namespace genie

namespace genie {
namespace entropy {
namespace gabac {

void transformLutTransform(unsigned order, util::DataBlock *transformedSymbols, util::DataBlock *inverseLUT,
                           util::DataBlock *inverseLUT1);

void inverseTransformLutTransform(unsigned order, util::DataBlock *symbols, util::DataBlock *inverseLUT,
                                  util::DataBlock *inverseLUT1);

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_LUT_TRANSFORM_H_
