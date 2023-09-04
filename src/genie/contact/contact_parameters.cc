/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "contact_parameters.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

ContactParameters::ContactParameters():
    sample_infos(),
    chr_infos(),
    interval(1),
    tile_size(1),
    interval_multipliers(),
    norm_method_infos(),
    norm_mat_infos()
{
    // TODO @Yeremia: Check if interval multipliers are valid
    // TODO @Yeremia: Set default value so that interval_multipliers is valid
}

// ---------------------------------------------------------------------------------------------------------------------

ContactParameters::ContactParameters(
    std::vector<SampleInformation>&& _sample_infos,
    std::vector<ChromosomeInformation>&& _chr_infos,
    uint32_t _interval,
    uint32_t _tile_size,
    std::vector<uint32_t>&& _interval_multipliers,
    std::vector<NormalizationMethodInformation>&& _norm_method_infos,
    std::vector<NormalizedMatrixInformations>&& _norm_mat_infos
):
    sample_infos(_sample_infos),
    chr_infos(_chr_infos),
    interval(_interval),
    tile_size(_tile_size),
    interval_multipliers(_interval_multipliers),
    norm_method_infos(_norm_method_infos),
    norm_mat_infos(_norm_mat_infos)
{
    // TODO @Yeremia: check if interval multipliers are valid
    for (uint32_t mult: _interval_multipliers){
        UTILS_DIE_IF(tile_size % mult != 0, "Invalid multiplier!");
    }

    UTILS_DIE_IF(sample_infos.size() > UINT8_MAX, "sample_infos is not uint8!");
    UTILS_DIE_IF(chr_infos.size() > UINT8_MAX, "chr_infos is not uint8!");
    UTILS_DIE_IF(interval_multipliers.size() > UINT8_MAX, "interval_multipliers is not uint8!");
    UTILS_DIE_IF(norm_method_infos.size() > UINT8_MAX, "norm_method_infos is not uint8!");
    UTILS_DIE_IF(norm_mat_infos.size() > UINT8_MAX, "norm_mat_infos is not uint8!");
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactParameters::getNumberSamples() const {return static_cast<uint8_t>(sample_infos.size());}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactParameters::getNumberChromosomes() const {return static_cast<uint8_t>(chr_infos.size());}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactParameters::getInterval() const {return interval;}

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