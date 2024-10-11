/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Declaration of the manual configuration function for GABAC.
 *
 * This file provides the declaration of a function to retrieve a default, manually specified
 * GABAC encoding configuration for a given descriptor subsequence. The returned configuration
 * defines the transformation operations and parameters to be applied to the subsequence during
 * entropy encoding.
 *
 * @details The `getEncoderConfigManual` function generates a default configuration that is
 * guaranteed to be compatible with the specified subsequence. This configuration can be used as
 * a baseline or for testing purposes in GABAC workflows. Users may override this configuration
 * by providing custom parameters during the encoding setup.
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

/**
 * @brief Retrieves a default GABAC encoding configuration for a given descriptor subsequence.
 *
 * This function returns a manually specified `paramcabac::Subsequence` object that contains the
 * default configuration parameters for the given descriptor subsequence. The configuration is
 * tailored to handle basic encoding scenarios and serves as a starting point for custom setups.
 * It can be used when an automatic configuration is not available or when a specific baseline
 * setup is desired.
 *
 * @param sub The descriptor subsequence index to configure.
 * @return A `paramcabac::Subsequence` object containing the default configuration for the given subsequence.
 */
genie::entropy::paramcabac::Subsequence getEncoderConfigManual(core::GenSubIndex sub);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_CONFIG_MANUAL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
