/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_DIFF_CODING_H_
#define GABAC_DIFF_CODING_H_

namespace genie {
namespace util {
class DataBlock;
}
}  // namespace genie

namespace genie {
namespace entropy {
namespace gabac {

void transformDiffCoding(util::DataBlock *transformedSymbols);

void inverseTransformDiffCoding(util::DataBlock *transformedSymbols);

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_DIFF_CODING_H_
