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
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

struct SampleInformation {
    uint8_t ID;
    std::string name;
};

// ---------------------------------------------------------------------------------------------------------------------

struct ChromosomeInformation {
    uint8_t ID;
    std::string name;
    uint64_t length;
};

// ---------------------------------------------------------------------------------------------------------------------

struct NormalizationMethodInformation {
    uint8_t ID;
    std::string name;
    bool mult_flag;
};

// ---------------------------------------------------------------------------------------------------------------------

struct NormalizedMatrixInformations {
    uint8_t ID;
    std::string name;
};

// ---------------------------------------------------------------------------------------------------------------------

class ContactParameters {
 private:
    std::unordered_map<uint8_t, SampleInformation> sample_infos;
    std::unordered_map<uint8_t, ChromosomeInformation> chr_infos;

    uint32_t interval;
    uint32_t tile_size;
    std::vector<uint32_t> interval_multipliers;

    std::unordered_map<uint8_t, NormalizationMethodInformation> norm_method_infos;
    std::unordered_map<uint8_t, NormalizedMatrixInformations> norm_mat_infos;

 public:
    ContactParameters();

    ContactParameters(
        std::unordered_map<uint8_t, SampleInformation>&& sample_infos,
        std::unordered_map<uint8_t, ChromosomeInformation>&& chr_infos,
        uint32_t interval,
        uint32_t tile_size,
        std::vector<uint32_t>&& interval_multipliers,
        std::unordered_map<uint8_t, NormalizationMethodInformation>&& norm_method_infos,
        std::unordered_map<uint8_t, NormalizedMatrixInformations>&& norm_mat_infos
    );

    ContactParameters(util::BitReader& reader);

    uint8_t getNumberSamples() const;
    void AddSample(uint8_t ID, SampleInformation&& sample_info);
    const std::unordered_map<uint8_t, SampleInformation>& getSamples() const;
    uint8_t getNumberChromosomes() const;
    void AddChromosome(uint8_t ID, ChromosomeInformation&& chr_info);
    const std::unordered_map<uint8_t, ChromosomeInformation>& getChromosomes() const;
    uint32_t getInterval() const;
    uint8_t getNumberIntervalMultipliers() const;
    uint8_t getNumberNormMethods() const;
    void AddNormMethod(uint8_t ID, NormalizationMethodInformation&& norm_method_info);
    const std::unordered_map<uint8_t, NormalizationMethodInformation>& getNormMethods() const;
    uint8_t getNumberNormMats() const;
    void AddNormMat(uint8_t ID, NormalizedMatrixInformations&& norm_mat_info);
    const std::unordered_map<uint8_t, NormalizedMatrixInformations>& getNormMats() const;

    uint32_t getNumBinEntries(uint8_t chr_ID, uint8_t interv_mult=1);
    uint32_t getNumTiles(uint8_t chr_ID, uint8_t interv_mult=1);

    void write(core::Writer& writer) const;
    size_t getSize(core::Writer& writesize) const;
};

bool isSymmetrical(uint8_t chr1_ID, uint8_t chr2_ID);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_CONTACT_PARAMETERS_H

// ---------------------------------------------------------------------------------------------------------------------
