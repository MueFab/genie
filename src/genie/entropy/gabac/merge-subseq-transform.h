/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_MERGE_CODING_H_
#define GABAC_MERGE_CODING_H_

#include <cstdint>
#include <genie/entropy/paramcabac/subsequence.h>

namespace genie {
namespace util {
class DataBlock;
}
}  // namespace genie

namespace genie {
namespace entropy {
namespace gabac {

void transformMergeCoding(const paramcabac::Subsequence& subseqCfg, util::DataBlock *symbols, std::vector<util::DataBlock *> transformedSubseqs);

void inverseTransformMergeCoding(const paramcabac::Subsequence& subseqCfg, util::DataBlock *symbols, std::vector<util::DataBlock *> transformedSubseqs);

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_MERGE_CODING_H_
