/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_DECODE_DESC_SUBSEQ_H_
#define SRC_GENIE_ENTROPY_GABAC_DECODE_DESC_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <vector>
#include "genie/entropy/paramcabac/subsequence.h"
#include "genie/util/data-block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingConfiguration;
struct IOConfiguration;

/**
 * @brief
 * @param subseqCfg
 * @param transformedSubseqs
 */
void doInverseSubsequenceTransform(const paramcabac::Subsequence& subseqCfg,
                                   std::vector<util::DataBlock>* const transformedSubseqs);

/**
 * @brief
 * @param ioConf
 * @param enConf
 * @return
 */
uint64_t decodeDescSubsequence(const IOConfiguration& ioConf, const EncodingConfiguration& enConf);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_DECODE_DESC_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
