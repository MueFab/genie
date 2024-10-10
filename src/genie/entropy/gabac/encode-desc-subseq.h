/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_ENCODE_DESC_SUBSEQ_H_
#define SRC_GENIE_ENTROPY_GABAC_ENCODE_DESC_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include "genie/entropy/paramcabac/subsequence.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

// ---------------------------------------------------------------------------------------------------------------------

struct IOConfiguration;
struct EncodingConfiguration;

void doSubsequenceTransform(const paramcabac::Subsequence& subseqCfg, std::vector<util::DataBlock>* transformedSubseqs);

/**
 * @brief
 * @param conf
 * @param enConf
 * @return
 */
uint64_t encodeDescSubsequence(const IOConfiguration& conf, const EncodingConfiguration& enConf);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_ENCODE_DESC_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
