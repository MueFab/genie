/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_MERGE_CODING_H_
#define GABAC_MERGE_CODING_H_

#include <genie/entropy/paramcabac/subsequence.h>
#include <genie/util/data-block.h>
#include <cstdint>

namespace genie {
namespace entropy {
namespace gabac {

void transformMergeCoding(const paramcabac::Subsequence& subseqCfg,
                          std::vector<util::DataBlock>* const transformedSubseqs);

void inverseTransformMergeCoding(const paramcabac::Subsequence& subseqCfg,
                                 std::vector<util::DataBlock>* const transformedSubseqs);

}  // namespace gabac
}  // namespace entropy
}  // namespace genie
#endif  // GABAC_MERGE_CODING_H_
