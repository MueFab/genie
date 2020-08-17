/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GABAC_CONFIG_MANUAL_H_
#define GABAC_CONFIG_MANUAL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>
#include <string>

#include <genie/core/constants.h>
#include <genie/entropy/gabac/gabac.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace entropy {
namespace gabac {

// ---------------------------------------------------------------------------------------------------------------------

using namespace genie::core;

/**
 *
 * @param sub
 * @return
 */
paramcabac::Subsequence getEncoderConfigManual(const core::GenSubIndex sub);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace entropy
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GABAC_CONFIG_MANUAL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------