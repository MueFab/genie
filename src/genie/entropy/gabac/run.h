/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_RUN_H_
#define SRC_GENIE_ENTROPY_GABAC_RUN_H_

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
 * @brief Start a run of gabac core
 * @param conf i/o configuration
 * @param enConf gabac configuration
 * @param decode If you want to decode (true) or encode (false).
 */
uint64_t run(const IOConfiguration& conf, const EncodingConfiguration& enConf, bool decode);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_RUN_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
