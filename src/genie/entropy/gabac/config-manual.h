/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_CONFIG_MANUAL_H_
#define SRC_GENIE_ENTROPY_GABAC_CONFIG_MANUAL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>
#include <string>
#include "genie/core/constants.h"
#include "genie/entropy/gabac/gabac.h"
#include "genie/entropy/paramcabac/subsequence.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief
 * @param sub
 * @return
 */
genie::entropy::paramcabac::Subsequence getEncoderConfigManual(core::GenSubIndex sub);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_CONFIG_MANUAL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
