/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_DECODE_DESC_SUBSEQ_H_
#define SRC_GENIE_ENTROPY_GABAC_DECODE_DESC_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingConfiguration;
struct IOConfiguration;

/**
 * @brief
 * @param ioConf
 * @param enConf
 * @return
 */
uint64_t decodeDescSubsequence(const IOConfiguration& ioConf, const EncodingConfiguration& enConf);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_DECODE_DESC_SUBSEQ_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
