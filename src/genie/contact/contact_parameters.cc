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

uint8_t ContactParameters::getNumberSamples() const {return sample_infos.size();}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactParameters::getNumberChromosomes() const {return chr_infos.size();}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactParameters::getNumberIntervalMultipliers() const {return interval_multipliers.size();}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactParameters::getNumberNormMethods() const {return norm_method_infos.size();}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactParameters::getNumberNormMats() const {return norm_mat_infos.size();}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------