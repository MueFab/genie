/**
 * @file
 * @copyright This file is part of GABAC. See LICENSE and/or
 * https://github.com/mitogen/gabac for more details.
 */

#ifndef GABAC_DIFF_CODING_H_
#define GABAC_DIFF_CODING_H_

namespace util {
class DataBlock;
}

namespace genie {
namespace gabac {

void transformDiffCoding(util::DataBlock *transformedSymbols);

void inverseTransformDiffCoding(util::DataBlock *transformedSymbols);

}  // namespace gabac
}  // namespace genie
#endif  // GABAC_DIFF_CODING_H_
