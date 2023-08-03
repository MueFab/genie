/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_CONTACT_CONTACT_PARAMETERS_H
#define GENIE_CONTACT_CONTACT_PARAMETERS_H

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <boost/optional/optional.hpp>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/core/constants.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

struct SampleInformation {
    uint8_t ID;
    std::string name;
};

struct ChromosomeInformation {
    uint8_t ID;
    std::string name;
    uint64_t length;
};

struct NormalizationMethodInformation {
    uint8_t ID;
    std::string name;
    bool mult_flag;
};

struct NormalizedMatrixInformations {
    uint8_t ID;
    std::string name;
};

// ---------------------------------------------------------------------------------------------------------------------

class ContactParameters {
 private:
    std::vector<SampleInformation> sample_infos;
    std::vector<ChromosomeInformation> chr_infos;

    uint32_t interval;
    uint32_t tile_size;
    std::vector<uint32_t> interval_multipliers;

    std::vector<NormalizationMethodInformation> norm_method_infos;

    std::vector<NormalizedMatrixInformations> norm_mat_infos;
 public:

    uint8_t getNumberSamples() const;
    uint8_t getNumberChromosomes() const;
    uint8_t getNumberIntervalMultipliers() const;
    uint8_t getNumberNormMethods() const;
    uint8_t getNumberNormMats() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_CONTACT_PARAMETERS_H

// ---------------------------------------------------------------------------------------------------------------------
