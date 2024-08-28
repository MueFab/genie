/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_ENCODE_DESC_SUBSEQ_H_
#define SRC_GENIE_ENTROPY_GABAC_ENCODE_DESC_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include "genie/util/data-block.h"
#include "genie/entropy/paramcabac/subsequence.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

struct IOConfiguration;
struct EncodingConfiguration;

void doSubsequenceTransform(const paramcabac::Subsequence &subseqCfg,
                            std::vector<util::DataBlock> *transformedSubseqs);

/**
 * @brief
 * @param conf
 * @param enConf
 * @return
 */
uint64_t encodeDescSubsequence(const IOConfiguration& conf, const EncodingConfiguration& enConf);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_ENCODE_DESC_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
