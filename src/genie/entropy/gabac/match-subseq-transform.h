/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_MATCH_CODING_H_
#define GABAC_MATCH_CODING_H_

#include <genie/entropy/paramcabac/subsequence.h>
#include <genie/util/data-block.h>
#include <cstdint>

namespace genie {
namespace entropy {
namespace gabac {

void transformMatchCoding(const paramcabac::Subsequence &subseqCfg,
                          std::vector<util::DataBlock> *const transformedSubseqs);

void inverseTransformMatchCoding(std::vector<util::DataBlock> *const transformedSubseqs);

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_MATCH_CODING_H_
