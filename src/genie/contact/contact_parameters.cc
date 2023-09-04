/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "contact_parameters.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactParameters::getNumberSamples() const {return static_cast<uint8_t>(sample_infos.size());}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactParameters::getNumberChromosomes() const {return static_cast<uint8_t>(chr_infos.size());}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactParameters::getNumberIntervalMultipliers() const {return static_cast<uint8_t>(interval_multipliers.size());}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactParameters::getNumberNormMethods() const {return static_cast<uint8_t>(norm_method_infos.size());}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactParameters::getNumberNormMats() const {return static_cast<uint8_t>(norm_mat_infos.size());}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------