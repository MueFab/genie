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
#include <genie/core/constants.h>
#include <genie/core/writer.h>
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>
#include "contact_subcm_parameters.h"

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

// TODO (Yeremia): This is a dummy hash pair function, update this function if necessary
struct chr_pair_hash {
    template <class T1, class T2>
    size_t operator()(const std::pair<T1, T2>& p) const
    {
//        auto hash1 = std::hash<T1>{}(p.first);
//        auto hash2 = std::hash<T2>{}(p.second);
//
//        if (hash1 != hash2) {
//            return hash1 ^ hash2;
//        }
//
//        // If hash1 == hash2, their XOR is zero.
//        return hash1;
        return (static_cast<size_t>(p.first) << 16) + static_cast<size_t>(p.second);
    }
};

// ---------------------------------------------------------------------------------------------------------------------

using ChrIDPair = std::pair<uint8_t, uint8_t>;
using SCMParamsDtype = std::unordered_map<ChrIDPair, ContactSubmatParameters, chr_pair_hash>;

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
    SCMParamsDtype scm_params;

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
    void addSample(SampleInformation&& sample_info);
    void addSample(uint8_t ID, std::string&& sample_name);
    const std::unordered_map<uint8_t, SampleInformation>& getSamples() const;
    uint8_t getNumberChromosomes() const;
    void addChromosome(ChromosomeInformation&& chr_info);
    void upsertChromosome(uint8_t ID, const std::string& name, uint64_t length);
    const std::unordered_map<uint8_t, ChromosomeInformation>& getChromosomes() const;
    uint32_t getInterval() const;
    void setInterval(uint32_t interval);
    uint32_t getTileSize() const;
    void setTileSize(uint32_t tile_size);
    uint8_t getNumberIntervalMultipliers() const;
    uint8_t getNumberNormMethods() const;
    void addNormMethod(uint8_t ID, NormalizationMethodInformation&& norm_method_info);
    const std::unordered_map<uint8_t, NormalizationMethodInformation>& getNormMethods() const;
    uint8_t getNumberNormMats() const;
    void addNormMat(uint8_t ID, NormalizedMatrixInformations&& norm_mat_info);
    const std::unordered_map<uint8_t, NormalizedMatrixInformations>& getNormMats() const;
    uint16_t getNumSCMParams() const;
    void addSCMParam(ContactSubmatParameters&& scm_param);
    const SCMParamsDtype& getSCMParams() const;

    uint32_t getNumBinEntries(uint8_t chr_ID, uint8_t interv_mult=1);
    uint32_t getNumTiles(uint8_t chr_ID, uint8_t interv_mult=1);

    void write(core::Writer& writer) const;
    size_t getSize(core::Writer& writesize) const;
};

// ---------------------------------------------------------------------------------------------------------------------

bool isSymmetrical(uint8_t chr1_ID, uint8_t chr2_ID);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CONTACT_CONTACT_PARAMETERS_H

// ---------------------------------------------------------------------------------------------------------------------
