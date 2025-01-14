/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "contact_matrix_parameters.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace contact {

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixParameters::ContactMatrixParameters()
    : sample_infos(),
      chr_infos(),
      bin_size(0),
      tile_size(0),
      bin_size_multipliers(),
      norm_method_infos(),
      norm_mat_infos()
{
    // TODO (Yeremia): Check if interval multipliers are valid
    // TODO (Yeremia): Set default value so that bin_size_multipliers is valid
}

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixParameters::ContactMatrixParameters(
    std::unordered_map<uint16_t, SampleInformation>&& _sample_infos,
    std::unordered_map<uint8_t, ChromosomeInformation>&& _chr_infos,
    uint32_t _interval,
    uint32_t _tile_size,
    std::vector<uint32_t>&& _interval_multipliers,
    std::unordered_map<uint8_t, NormalizationMethodInformation>&& _norm_method_infos,
    std::unordered_map<uint8_t, NormalizedMatrixInformations>&& _norm_mat_infos
):
    sample_infos(std::move(_sample_infos)),
    chr_infos(std::move(_chr_infos)),
    bin_size(_interval),
    tile_size(_tile_size),
      bin_size_multipliers(std::move(_interval_multipliers)),
    norm_method_infos(std::move(_norm_method_infos)),
    norm_mat_infos(std::move(_norm_mat_infos)) {

    // TODO (Yeremia): check if interval multipliers are valid
    for (uint32_t mult: _interval_multipliers){
        UTILS_DIE_IF(tile_size % mult != 0, "Invalid multiplier!");
    }

    UTILS_DIE_IF(sample_infos.size() > UINT16_MAX, "sample_infos is not uint8!");
    UTILS_DIE_IF(chr_infos.size() > UINT8_MAX, "chr_infos is not uint8!");
    UTILS_DIE_IF(bin_size_multipliers.size() > UINT8_MAX, "bin_size_multipliers is not uint8!");
    UTILS_DIE_IF(norm_method_infos.size() > UINT8_MAX, "norm_method_infos is not uint8!");
    UTILS_DIE_IF(norm_mat_infos.size() > UINT8_MAX, "norm_mat_infos is not uint8!");
}

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixParameters::ContactMatrixParameters(util::BitReader& reader){
    auto num_samples = reader.readBypassBE<uint16_t>();
    for (uint16_t i = 0; i<num_samples; i++){
        auto ID = reader.readBypassBE<uint16_t>();
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

    bin_size = reader.readBypassBE<uint32_t>();
    tile_size = reader.readBypassBE<uint32_t>();
    auto num_interval_mults = reader.readBypassBE<uint8_t>();
    for (uint8_t i = 0; i<num_interval_mults; i++){
        bin_size_multipliers.push_back(reader.readBypassBE<uint32_t>());
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

//    auto num_scm = reader.readBypassBE<uint16_t>();
//    for (uint16_t i = 0; i<num_scm; i++){
//        auto chr1_ID = reader.readBypassBE<uint8_t>();
//        auto chr2_ID = reader.readBypassBE<uint8_t>();

//        auto scm_param =
//            SubcontactMatrixParameters(
//            reader,
//            chr1_ID,
//            chr2_ID,
//            getNumTiles(chr1_ID),
//            getNumTiles(chr2_ID)
//        );
//
//        auto chr_pair = ChrIDPair(chr1_ID, chr2_ID);
//        UTILS_DIE_IF(scm_params.find(chr_pair) != scm_params.end(), "Chromosome pair already exist!");
//        scm_params.emplace(chr_pair, std::move(scm_param));
//    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t ContactMatrixParameters::getNumSamples() const { return static_cast<uint16_t>(sample_infos.size()); }

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::addSample(SampleInformation&& sample_info) {
    UTILS_DIE_IF(sample_infos.find(sample_info.ID) != sample_infos.end(), "sample_ID already exists!");
    sample_infos.emplace(sample_info.ID, std::move(sample_info));
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::addSample(uint16_t ID, std::string&& name, bool exist_ok) {
    auto it = sample_infos.find(ID);
    if (it == sample_infos.end()) {
        SampleInformation sample_info = {ID, std::move(name)};
        sample_infos.emplace(ID, std::move(sample_info));
    } else if (exist_ok){
        UTILS_DIE_IF(it->second.name != name,
                     "name differs for the same sample_ID");
    } else{
        UTILS_DIE("sample_ID already exists!");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::unordered_map<uint16_t, SampleInformation>& ContactMatrixParameters::getSamples() const{ return sample_infos; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ContactMatrixParameters::getSampleName(
    uint16_t _sample_ID
) const{
    auto sample_obj = sample_infos.find(_sample_ID);
    UTILS_DIE_IF(sample_obj == sample_infos.end(), "sample_ID does not exist!");

    return sample_obj->second.name;
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::upsertSample(
    uint16_t ID,
    const std::string& name,
    bool exist_ok
){

    auto _name = std::string(name);

    upsertSample(
        ID,
        std::move(_name),
        exist_ok
    );
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::upsertSample(
    uint16_t ID,
    std::string&& name,
    bool exist_ok
){

    auto it = sample_infos.find(ID);
    if (it == sample_infos.end()){
        SampleInformation sample_info = {ID, std::move(name)};
        sample_infos.emplace(ID, std::move(sample_info));
    } else if (exist_ok){
        it->second.name = std::move(name);

    } else{
        UTILS_DIE("chr_ID already exists!");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactMatrixParameters::getNumChromosomes() const { return static_cast<uint8_t>(chr_infos.size()); }

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::addChromosome(ChromosomeInformation&& chr_info) {chr_infos.emplace(chr_info.ID, std::move(chr_info));}

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::upsertChromosome(
    uint8_t ID,
    const std::string& name,
    uint64_t length,
    bool exist_ok
){
    auto _name = std::string(name);

    upsertChromosome(
        ID,
        std::move(_name),
        length,
        exist_ok
    );
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::upsertChromosome(
    uint8_t ID,
    std::string&& name,
    uint64_t length,
    bool exist_ok
){

    auto it = chr_infos.find(ID);
    if (it == chr_infos.end()){
        ChromosomeInformation chr_info = {ID, std::move(name), length};
        chr_infos.emplace(ID, std::move(chr_info));
    } else if (exist_ok){
        UTILS_DIE_IF(it->second.name != name,
                     "name differs for the same chr_ID");

        it->second.name = std::move(name);
        it->second.length = length;

    } else{
        UTILS_DIE("chr_ID already exists!");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::unordered_map<uint8_t, ChromosomeInformation>& ContactMatrixParameters::getChromosomes() const{ return chr_infos; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ContactMatrixParameters::getChromosomeLength(
    uint8_t chr_ID
) const {
    UTILS_DIE_IF(bin_size == 0, "Please set the bin size!");
    auto chr_obj = chr_infos.find(chr_ID);
    UTILS_DIE_IF(chr_obj == chr_infos.end(), "chr_ID does not exist!");

    return chr_obj->second.length;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactMatrixParameters::getBinSize() const { return bin_size; }

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::setBinSize(uint32_t _bin_size) { bin_size = _bin_size;}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactMatrixParameters::getTileSize() const { return tile_size; }

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::setTileSize(uint32_t _tile_size) {tile_size = _tile_size;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactMatrixParameters::getNumBinSizeMultipliers() const {
    return static_cast<uint8_t>(bin_size_multipliers.size());
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::upsertBinSizeMultiplier(
    size_t bin_size_multiplier
){
    if (bin_size_multiplier != 1){
        UTILS_DIE_IF(bin_size % bin_size_multiplier != 0, "Invalid bin_size_multiplier!");
        auto iter = std::find(bin_size_multipliers.begin(), bin_size_multipliers.end(), bin_size_multiplier);
        if (iter == bin_size_multipliers.end()){
            bin_size_multipliers.push_back(static_cast<uint32_t>(bin_size_multiplier));
        }
    }
}


// ---------------------------------------------------------------------------------------------------------------------

bool ContactMatrixParameters::isBinSizeMultiplierValid(
    size_t target_interv_mult
) const{

    // By default 1 is always valid
    if (target_interv_mult == 1){
        return true;
    }

    for (const auto& interv_mult: bin_size_multipliers){
        if (interv_mult == target_interv_mult){
            return true;
        }
    }

    auto iter = std::find(
        bin_size_multipliers.begin(),
        bin_size_multipliers.end(),
        target_interv_mult
    );

    if (iter != bin_size_multipliers.end()){
        return true;
    } else {
        return false;
    }
};


// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactMatrixParameters::getNumNormMethods() const { return static_cast<uint8_t>(norm_method_infos.size()); }

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::addNormMethod(uint8_t ID, NormalizationMethodInformation&& norm_method_info){
    norm_method_infos.emplace(ID, std::move(norm_method_info));
}

// ---------------------------------------------------------------------------------------------------------------------

const std::unordered_map<uint8_t, NormalizationMethodInformation>& ContactMatrixParameters::getNormMethods() const{
    return norm_method_infos;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactMatrixParameters::getNumNormMats() const { return static_cast<uint8_t>(norm_mat_infos.size()); }

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::addNormMat(uint8_t ID, NormalizedMatrixInformations&& norm_mat_info){
    norm_mat_infos.emplace(ID, norm_mat_info);
}

// ---------------------------------------------------------------------------------------------------------------------

const std::unordered_map<uint8_t, NormalizedMatrixInformations>& ContactMatrixParameters::getNormMats() const {return norm_mat_infos;}

// ---------------------------------------------------------------------------------------------------------------------

//uint16_t ContactMatrixParameters::getNumSCMParams() const {return static_cast<uint16_t>(scm_params.size());}

// ---------------------------------------------------------------------------------------------------------------------

//void ContactMatrixParameters::addSCMParam(SubcontactMatrixParameters&& scm_param){
//    auto chr_pair = scm_param.getChrPair();
//    UTILS_DIE_IF(scm_params.find(chr_pair) != scm_params.end(), "SCM parameter already exists!");
//    scm_params.emplace(chr_pair, std::move(scm_param));
//}

// ---------------------------------------------------------------------------------------------------------------------

//const SCMParamsDtype& ContactMatrixParameters::getSCMParams() const {return scm_params;}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ContactMatrixParameters::getNumBinEntries(
    uint8_t chr_ID,
    uint32_t interv_mult
){
    UTILS_DIE_IF(bin_size == 0, "Please set the bin size!");
    UTILS_DIE_IF(chr_infos.find(chr_ID) == chr_infos.end(), "chr_ID does not exist!");

    uint64_t chr_len = chr_infos[chr_ID].length;

    UTILS_DIE_IF(chr_len == 0, "Please set the chromosome length!");

    // This is ceil operation for integer
    return static_cast<uint64_t>(1 + ((chr_len - 1) / (interv_mult* bin_size)));
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactMatrixParameters::getNumTiles(
    uint8_t chr_ID,
    uint32_t interv_mult
){
    UTILS_DIE_IF(tile_size == 0, "Please set the tile size!");

    uint64_t num_bins = getNumBinEntries(chr_ID, interv_mult);

    // This is ceil operation for integer
    return static_cast<uint32_t>(1 + ((num_bins - 1) / (tile_size)));
}

// ---------------------------------------------------------------------------------------------------------------------

size_t ContactMatrixParameters::getSize() const {
    size_t size = 0;

    size += sizeof(uint16_t);
    for (const auto& sample_info : sample_infos) {
        size += sizeof(uint16_t); // ID
        size += sample_info.second.name.size() + 1; // +1 for the null terminator
    }

    size += sizeof(uint8_t);
    for (const auto& chr_info : chr_infos) {
        size += sizeof(uint8_t); // ID
        size += chr_info.second.name.size() + 1;
        size += sizeof(uint64_t); // length
    }

    size += sizeof(uint32_t); // bin_size
    size += sizeof(uint32_t); // tile_size
    size += sizeof(uint8_t);  // number of bin_size_multipliers
    size += bin_size_multipliers.size() * sizeof(uint32_t);

    size += sizeof(uint8_t); // number of normalization methods
    for (const auto& norm_method_info : norm_method_infos) {
        size += sizeof(uint8_t);
        size += norm_method_info.second.name.size() + 1;
        size += sizeof(uint8_t); // mult_flag
    }

    size += sizeof(uint8_t); // number of normalized matrices
    for (const auto& norm_mat_info : norm_mat_infos) {
        size += sizeof(uint8_t);
        size += norm_mat_info.second.name.size() + 1;
    }

    return size;
}

// ---------------------------------------------------------------------------------------------------------------------

//TODO(yeremia): use util::Bitwriter instead of core::Writer
void ContactMatrixParameters::write(core::Writer& writer) const {
    writer.write(static_cast<uint16_t>(sample_infos.size()),16);
    for(const auto& sample_info : sample_infos) {
        writer.write(sample_info.second.ID, 16);
        writer.write(static_cast<uint16_t>(sample_info.second.name.size()), 8);
        writer.write(sample_info.second.name);
    }

    writer.write(static_cast<uint8_t>(chr_infos.size()),8);
    for(const auto& chr_info : chr_infos) {
        writer.write(chr_info.second.ID, 8);
        writer.write(static_cast<uint16_t>(chr_info.second.name.size()), 8);
        writer.write(chr_info.second.name);
        writer.write(chr_info.second.length, 64);
    }

    writer.write(bin_size, 32);
    writer.write(tile_size, 32);

    writer.write(static_cast<uint8_t>(bin_size_multipliers.size()),8);
    for(const auto& bin_size_multiplier : bin_size_multipliers) {
        writer.write(bin_size_multiplier, 32);
    }

    writer.write(static_cast<uint8_t>(norm_method_infos.size()),8);
    for(const auto& method_info : norm_method_infos) {
        writer.write(method_info.second.ID, 8);
        writer.write(static_cast<uint16_t>(method_info.second.name.size()), 8);
        writer.write(method_info.second.name);
        writer.write_reserved(7);
        writer.write(method_info.second.mult_flag, 1);
    }

    writer.write(static_cast<uint8_t>(norm_mat_infos.size()),8);
    for(const auto& mat_info : norm_mat_infos) {
        writer.write(mat_info.second.ID, 8);
        writer.write(static_cast<uint16_t>(mat_info.second.name.size()), 8);
        writer.write(mat_info.second.name);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename KeyType, typename T>
bool mapsEqual(const std::unordered_map<KeyType, T>& current, const std::unordered_map<KeyType, T>& other) {
    if (current.size() != other.size()) {
        return false;
    }
    for (const auto& pair : current) {
        auto iter = other.find(pair.first);
        if (iter == other.end()) {
            return false;
        }
        if (!(pair.second == iter->second)){
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

bool ContactMatrixParameters::operator==(const ContactMatrixParameters& other) {
    if (this->getNumSamples() != other.getNumSamples()){
        return false;
    }
    if (!mapsEqual(this->sample_infos, other.sample_infos)){
        return false;
    }

    if (this->getNumChromosomes() != other.getNumChromosomes()){
        return false;
    }
    if (!mapsEqual(this->getChromosomes(), other.getChromosomes())){
        return false;
    }

    if (this->getBinSize() != other.getBinSize()){
        return false;
    }
    if (this->getTileSize() != other.getTileSize()){
        return false;
    }

    if (this->getNumNormMethods() != other.getNumNormMethods()){
        return false;
    }
    if (!mapsEqual(this->norm_method_infos, other.norm_method_infos)){
        return false;
    }

    if (this->getNumNormMats() != other.getNumNormMats()){
        return false;
    }
    if (!mapsEqual(this->norm_mat_infos, other.norm_mat_infos)){
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace contact
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
