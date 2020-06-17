/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_DECODING_H_
#define GABAC_DECODING_H_

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

struct EncodingConfiguration;
struct IOConfiguration;

/**
 *
 * @param ioConf
 * @param enConf
 * @return
 */
unsigned long decodeDescSubsequence(const IOConfiguration& ioConf, const EncodingConfiguration& enConf);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GABAC_DECODING_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------