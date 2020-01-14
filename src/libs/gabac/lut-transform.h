/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_LUT_TRANSFORM_H_
#define GABAC_LUT_TRANSFORM_H_

namespace util {
class DataBlock;
}

namespace genie {
namespace gabac {

void transformLutTransform(unsigned order, util::DataBlock *transformedSymbols, util::DataBlock *inverseLUT,
                           util::DataBlock *inverseLUT1);

void inverseTransformLutTransform(unsigned order, util::DataBlock *symbols, util::DataBlock *inverseLUT,
                                  util::DataBlock *inverseLUT1);

}  // namespace gabac
}  // namespace genie

#endif  // GABAC_LUT_TRANSFORM_H_
