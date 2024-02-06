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
    norm_mat_infos() {
    // TODO (Yeremia): Check if interval multipliers are valid
    // TODO (Yeremia): Set default value so that interval_multipliers is valid
}

// ---------------------------------------------------------------------------------------------------------------------

ContactParameters::ContactParameters(
    std::unordered_map<uint8_t, SampleInformation>&& _sample_infos,
    std::unordered_map<uint8_t, ChromosomeInformation>&& _chr_infos,
    uint32_t _interval,
    uint32_t _tile_size,
    std::vector<uint32_t>&& _interval_multipliers,
    std::unordered_map<uint8_t, NormalizationMethodInformation>&& _norm_method_infos,
    std::unordered_map<uint8_t, NormalizedMatrixInformations>&& _norm_mat_infos
):
    sample_infos(_sample_infos),
    chr_infos(_chr_infos),
    interval(_interval),
    tile_size(_tile_size),
    interval_multipliers(_interval_multipliers),
    norm_method_infos(_norm_method_infos),
    norm_mat_infos(_norm_mat_infos) {

    // TODO (Yeremia): check if interval multipliers are valid
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

ContactParameters::ContactParameters(util::BitReader& reader){
    auto num_samples = reader.readBypassBE<uint8_t>();
    for (uint8_t i = 0; i<num_samples; i++){
        auto ID = reader.readBypassBE<uint8_t>();
        std::string name;
        reader.readBypass_null_terminated(name);

        SampleInformation sample_info = {
            ID,
            std::move(name)
        };

        sample_infos.emplace(ID, std::move(sample_info));
    }

    auto num_chrs = reader.readBypassBE<uint8_t>();
    for (uint8_t i = 0; i<num_chrs; i++){
        auto ID = reader.readBypassBE<uint8_t>();
        std::string name;
        reader.readBypass_null_terminated(name);
        auto length = reader.readBypassBE<uint64_t>();

        ChromosomeInformation chr_info = {
            ID,
            std::move(name),
            length
        };

        chr_infos.emplace(ID, std::move(chr_info));
    }

    interval = reader.readBypassBE<uint32_t>();
    tile_size = reader.readBypassBE<uint32_t>();
    auto num_interval_mults = reader.readBypassBE<uint8_t>();
    for (uint8_t i = 0; i<num_interval_mults; i++){
        interval_multipliers.push_back(reader.readBypassBE<uint32_t>());
    }

    auto num_norm_methods = reader.readBypassBE<uint8_t>();
    for (uint8_t i = 0; i<num_norm_methods; i++){
        auto ID = reader.readBypassBE<uint8_t>();
        std::string name;
        reader.readBypass_null_terminated(name);
        bool mult_flag = reader.readBypassBE<uint8_t>();

        NormalizationMethodInformation norm_method_info = {
            ID,
            std::move(name),
            mult_flag
        };

        norm_method_infos.emplace(ID, std::move(norm_method_info));
    }

    auto num_norm_matrices = reader.readBypassBE<uint8_t>();
    for (uint8_t i = 0; i<num_norm_matrices; i++){
        auto ID = reader.readBypassBE<uint8_t>();
        std::string name;
        reader.readBypass_null_terminated(name);

        NormalizedMatrixInformations norm_mat_info = {
            ID,
            std::move(name)
        };

        norm_mat_infos.emplace(ID, std::move(norm_mat_info));
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactParameters::getNumberSamples() const {return static_cast<uint8_t>(sample_infos.size());}

// ---------------------------------------------------------------------------------------------------------------------

void ContactParameters::AddSample(uint8_t ID, SampleInformation&& _sample_info) {
    sample_infos.emplace(ID, std::move(_sample_info));
}

// ---------------------------------------------------------------------------------------------------------------------

const std::unordered_map<uint8_t, SampleInformation>& ContactParameters::getSamples() const{ return sample_infos;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactParameters::getNumberChromosomes() const {return static_cast<uint8_t>(chr_infos.size());}

// ---------------------------------------------------------------------------------------------------------------------

void ContactParameters::AddChromosome(uint8_t ID, ChromosomeInformation&& _chr_info) {
    chr_infos.emplace(ID, std::move(_chr_info));
}

// ---------------------------------------------------------------------------------------------------------------------

const std::unordered_map<uint8_t, ChromosomeInformation>& ContactParameters::getChromosomes() const{ return chr_infos;}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactParameters::getInterval() const {return interval;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactParameters::getNumberIntervalMultipliers() const {return static_cast<uint8_t>(interval_multipliers.size());}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactParameters::getNumberNormMethods() const {return static_cast<uint8_t>(norm_method_infos.size());}

// ---------------------------------------------------------------------------------------------------------------------

void ContactParameters::AddNormMethod(uint8_t ID, NormalizationMethodInformation&& norm_method_info){
    norm_method_infos.emplace(ID, std::move(norm_method_info));
}

// ---------------------------------------------------------------------------------------------------------------------

const std::unordered_map<uint8_t, NormalizationMethodInformation>& ContactParameters::getNormMethods() const{
    return norm_method_infos;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactParameters::getNumberNormMats() const {return static_cast<uint8_t>(norm_mat_infos.size());}

// ---------------------------------------------------------------------------------------------------------------------

void ContactParameters::AddNormMat(uint8_t ID, NormalizedMatrixInformations&& norm_mat_info){
    norm_mat_infos.emplace(ID, norm_mat_info);
}

// ---------------------------------------------------------------------------------------------------------------------

const std::unordered_map<uint8_t, NormalizedMatrixInformations>& ContactParameters::getNormMats() const {
    return norm_mat_infos;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactParameters::getNumBinEntries(uint8_t chr_ID, uint8_t interv_mult){
    uint64_t chr_len = chr_infos[chr_ID].length;

    // This is ceil operation for integer
    return static_cast<uint32_t>(1 + ((chr_len - 1) / (interv_mult*interval)));
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactParameters::getNumTiles(uint8_t chr_ID, uint8_t interv_mult){
    uint32_t num_bins = getNumBinEntries(chr_ID, interv_mult);

    // This is ceil operation for integer
    return static_cast<uint32_t>(1 + ((num_bins - 1) / (tile_size)));
}

// ---------------------------------------------------------------------------------------------------------------------

// TODO (Yeremia): implement this!
void ContactParameters::write(core::Writer& writer) const {}

// ---------------------------------------------------------------------------------------------------------------------

// TODO (Yeremia): implement this!
size_t ContactParameters::getSize(core::Writer& writesize) const {return 0;}

// ---------------------------------------------------------------------------------------------------------------------

bool isSymmetrical(uint8_t chr1_ID, uint8_t chr2_ID){return chr1_ID == chr2_ID;}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
