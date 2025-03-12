/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "contact_matrix_parameters.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::contact {

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixParameters::ContactMatrixParameters()
    : sample_infos_(),
      chr_infos_(),
      bin_size_(0),
      tile_size_(0),
      bin_size_multipliers_(),
      norm_method_infos_(),
      norm_mat_infos_()
{
    // TODO (Yeremia): Check if interval multipliers are valid
    // TODO (Yeremia): Set default value so that bin_size_multipliers_ is valid
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
): sample_infos_(std::move(_sample_infos)),
      chr_infos_(std::move(_chr_infos)),
      bin_size_(_interval),
      tile_size_(_tile_size),
      bin_size_multipliers_(std::move(_interval_multipliers)),
      norm_method_infos_(std::move(_norm_method_infos)),
      norm_mat_infos_(std::move(_norm_mat_infos)) {

    // TODO (Yeremia): check if interval multipliers are valid
    for (uint32_t mult: _interval_multipliers){
        UTILS_DIE_IF(tile_size_ % mult != 0, "Invalid multiplier!");
    }

    UTILS_DIE_IF(sample_infos_.size() > UINT16_MAX, "sample_infos_ is not uint8!");
    UTILS_DIE_IF(chr_infos_.size() > UINT8_MAX, "chr_infos_ is not uint8!");
    UTILS_DIE_IF(bin_size_multipliers_.size() > UINT8_MAX, "bin_size_multipliers_ is not uint8!");
    UTILS_DIE_IF(norm_method_infos_.size() > UINT8_MAX, "norm_method_infos_ is not uint8!");
    UTILS_DIE_IF(norm_mat_infos_.size() > UINT8_MAX, "norm_mat_infos_ is not uint8!");
}

// ---------------------------------------------------------------------------------------------------------------------

ContactMatrixParameters::ContactMatrixParameters(util::BitReader& reader){
    auto num_samples = reader.ReadAlignedInt<uint16_t>();
    for (uint16_t i = 0; i<num_samples; i++){
        auto ID = reader.ReadAlignedInt<uint16_t>();

        std::string name;
        name.resize(reader.ReadAlignedInt<uint8_t>());
        reader.ReadAlignedBytes(&name[0], name.size());

        SampleInformation sample_info = {
            ID,
            std::move(name)
        };

        sample_infos_.emplace(ID, std::move(sample_info));
    }

    auto num_chrs = reader.ReadAlignedInt<uint8_t>();
    for (uint8_t i = 0; i<num_chrs; i++){
        auto ID = reader.ReadAlignedInt<uint8_t>();
        std::string name;
        name.resize(reader.ReadAlignedInt<uint8_t>());
        reader.ReadAlignedBytes(&name[0], name.size());
        auto length = reader.ReadAlignedInt<uint64_t>();

        ChromosomeInformation chr_info = {
            ID,
            std::move(name),
            length
        };

        chr_infos_.emplace(ID, std::move(chr_info));
    }

    bin_size_ = reader.ReadAlignedInt<uint32_t>();
    tile_size_ = reader.ReadAlignedInt<uint32_t>();
    auto num_interval_mults = reader.ReadAlignedInt<uint8_t>();
    for (uint8_t i = 0; i<num_interval_mults; i++){
      bin_size_multipliers_.push_back(reader.ReadAlignedInt<uint32_t>());
    }

    auto num_norm_methods = reader.ReadAlignedInt<uint8_t>();
    for (uint8_t i = 0; i<num_norm_methods; i++){
        auto ID = reader.ReadAlignedInt<uint8_t>();
        std::string name;
        name.resize(reader.ReadAlignedInt<uint8_t>());
        reader.ReadAlignedBytes(&name[0], name.size());
        bool mult_flag = reader.ReadAlignedInt<uint8_t>();

        NormalizationMethodInformation norm_method_info = {
            ID,
            std::move(name),
            mult_flag
        };

        norm_method_infos_.emplace(ID, std::move(norm_method_info));
    }

    auto num_norm_matrices = reader.ReadAlignedInt<uint8_t>();
    for (uint8_t i = 0; i<num_norm_matrices; i++){
        auto ID = reader.ReadAlignedInt<uint8_t>();
        std::string name;
        name.resize(reader.ReadAlignedInt<uint8_t>());
        reader.ReadAlignedBytes(&name[0], name.size());

        NormalizedMatrixInformations norm_mat_info = {
            ID,
            std::move(name)
        };

        norm_mat_infos_.emplace(ID, std::move(norm_mat_info));
    }

//    auto num_scm = reader.ReadAlignedInt<uint16_t>();
//    for (uint16_t i = 0; i<num_scm; i++){
//        auto chr1_ID_ = reader.ReadAlignedInt<uint8_t>();
//        auto chr2_ID_ = reader.ReadAlignedInt<uint8_t>();

//        auto scm_param =
//            SubcontactMatrixParameters(
//            reader,
//            chr1_ID_,
//            chr2_ID_,
//            GetNumTiles(chr1_ID_),
//            GetNumTiles(chr2_ID_)
//        );
//
//        auto chr_pair = ChrIDPair(chr1_ID_, chr2_ID_);
//        UTILS_DIE_IF(scm_params.find(chr_pair) != scm_params.end(), "Chromosome pair already exist!");
//        scm_params.emplace(chr_pair, std::move(scm_param));
//    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t ContactMatrixParameters::GetNumSamples() const { return static_cast<uint16_t>(sample_infos_.size()); }

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void ContactMatrixParameters::AddSample(SampleInformation&& sample_info) {
    UTILS_DIE_IF(sample_infos_.find(sample_info.ID) != sample_infos_.end(), "sample_ID_ already exists!");
    sample_infos_.emplace(sample_info.ID, std::move(sample_info));
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::AddSample(uint16_t ID, std::string&& name, bool exist_ok) {
    auto it = sample_infos_.find(ID);
    if (it == sample_infos_.end()) {
        SampleInformation sample_info = {ID, std::move(name)};
        sample_infos_.emplace(ID, std::move(sample_info));
    } else if (exist_ok){
        UTILS_DIE_IF(it->second.name != name,
                     "name differs for the same sample_ID_");
    } else{
        UTILS_DIE("sample_ID_ already exists!");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] const std::unordered_map<uint16_t, SampleInformation>& ContactMatrixParameters::GetSamples() const{ return sample_infos_; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& ContactMatrixParameters::GetSampleName(uint16_t sample_ID) const{
    auto sample_obj = sample_infos_.find(sample_ID);
    UTILS_DIE_IF(sample_obj == sample_infos_.end(), "sample_ID_ does not exist!");

    return sample_obj->second.name;
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::UpsertSample(
    uint16_t ID,
    const std::string& name,
    bool exist_ok
){

    auto _name = std::string(name);

    UpsertSample(ID, std::move(_name), exist_ok);
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::UpsertSample(
    uint16_t ID,
    std::string&& name,
    bool exist_ok
){

    auto it = sample_infos_.find(ID);
    if (it == sample_infos_.end()){
        SampleInformation sample_info = {ID, std::move(name)};
        sample_infos_.emplace(ID, std::move(sample_info));
    } else if (exist_ok){
        it->second.name = std::move(name);

    } else{
        UTILS_DIE("chr_ID_ already exists!");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactMatrixParameters::GetNumChromosomes() const { return static_cast<uint8_t>(chr_infos_.size()); }

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void ContactMatrixParameters::AddChromosome(ChromosomeInformation&& chr_info) {
  chr_infos_.emplace(chr_info.ID, std::move(chr_info));}

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::UpsertChromosome(
    uint8_t ID,
    const std::string& name,
    uint64_t length,
    bool exist_ok
){
    auto _name = std::string(name);

    UpsertChromosome(ID, std::move(_name), length, exist_ok);
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::UpsertChromosome(
    uint8_t ID,
    std::string&& name,
    uint64_t length,
    bool exist_ok
){

    auto it = chr_infos_.find(ID);
    if (it == chr_infos_.end()){
        ChromosomeInformation chr_info = {ID, std::move(name), length};
        chr_infos_.emplace(ID, std::move(chr_info));
    } else if (exist_ok){
        UTILS_DIE_IF(it->second.name != name,
                     "name differs for the same chr_ID_");

        it->second.name = std::move(name);
        it->second.length = length;

    } else{
        UTILS_DIE("chr_ID_ already exists!");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

const std::unordered_map<uint8_t, ChromosomeInformation>& ContactMatrixParameters::GetChromosomes() const{ return chr_infos_; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t ContactMatrixParameters::GetChromosomeLength(
    uint8_t chr_ID
) const {
    UTILS_DIE_IF(bin_size_ == 0, "Please set the bin size!");
    auto chr_obj = chr_infos_.find(chr_ID);
    UTILS_DIE_IF(chr_obj == chr_infos_.end(), "chr_ID_ does not exist!");

    return chr_obj->second.length;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactMatrixParameters::GetBinSize() const { return bin_size_; }

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::SetBinSize(uint32_t _bin_size) {
  bin_size_ = _bin_size;}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactMatrixParameters::GetTileSize() const { return tile_size_; }

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::SetTileSize(uint32_t tile_size) {tile_size_ = tile_size;}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] uint8_t ContactMatrixParameters::GetNumBinSizeMultipliers() const {
    return static_cast<uint8_t>(bin_size_multipliers_.size());
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::UpsertBinSizeMultiplier(
    size_t bin_size_multiplier
){
    if (bin_size_multiplier != 1){
        UTILS_DIE_IF(bin_size_ % bin_size_multiplier != 0, "Invalid bin_size_multiplier_!");
        auto iter = std::find(bin_size_multipliers_.begin(),
                              bin_size_multipliers_.end(), bin_size_multiplier);
        if (iter == bin_size_multipliers_.end()){
          bin_size_multipliers_.push_back(static_cast<uint32_t>(bin_size_multiplier));
        }
    }
}


// ---------------------------------------------------------------------------------------------------------------------

bool ContactMatrixParameters::IsBinSizeMultiplierValid(
    size_t target_interv_mult
) const{

    // By default 1 is always valid
    if (target_interv_mult == 1){
        return true;
    }

    for (const auto& interv_mult: bin_size_multipliers_){
        if (interv_mult == target_interv_mult){
            return true;
        }
    }

    auto iter = std::find(bin_size_multipliers_.begin(),
                          bin_size_multipliers_.end(),
        target_interv_mult
    );

    if (iter != bin_size_multipliers_.end()){
        return true;
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactMatrixParameters::GetNumNormMethods() const { return static_cast<uint8_t>(norm_method_infos_.size()); }

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void ContactMatrixParameters::AddNormMethod(uint8_t ID, NormalizationMethodInformation&& norm_method_info){
  norm_method_infos_.emplace(ID, std::move(norm_method_info));
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] const std::unordered_map<uint8_t, NormalizationMethodInformation>& ContactMatrixParameters::GetNormMethods() const{
    return norm_method_infos_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ContactMatrixParameters::GetNumNormMats() const { return static_cast<uint8_t>(norm_mat_infos_.size()); }

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] void ContactMatrixParameters::AddNormMat(uint8_t ID, NormalizedMatrixInformations&& norm_mat_info){
  norm_mat_infos_.emplace(ID, norm_mat_info);
}

// ---------------------------------------------------------------------------------------------------------------------

[[maybe_unused]] const std::unordered_map<uint8_t, NormalizedMatrixInformations>& ContactMatrixParameters::GetNormMats() const {return norm_mat_infos_;}

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

uint64_t ContactMatrixParameters::GetNumBinEntries(
    uint8_t chr_ID,
    uint32_t interv_mult
){
    UTILS_DIE_IF(bin_size_ == 0, "Please set the bin size!");
    UTILS_DIE_IF(chr_infos_.find(chr_ID) == chr_infos_.end(), "chr_ID_ does not exist!");

    uint64_t chr_len = chr_infos_[chr_ID].length;

    UTILS_DIE_IF(chr_len == 0, "Please set the chromosome length!");

    // This is ceil operation for integer
    return static_cast<uint64_t>(1 + ((chr_len - 1) / (interv_mult* bin_size_)));
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t ContactMatrixParameters::GetNumTiles(
    uint8_t chr_ID,
    uint32_t interv_mult
){
    UTILS_DIE_IF(tile_size_ == 0, "Please set the tile size!");

    uint64_t num_bins = GetNumBinEntries(chr_ID, interv_mult);

    // This is ceil operation for integer
    return static_cast<uint32_t>(1 + ((num_bins - 1) / (tile_size_)));
}

// ---------------------------------------------------------------------------------------------------------------------

size_t ContactMatrixParameters::GetSize() const {
    size_t size = 0;

    size += sizeof(uint16_t);
    for (const auto& sample_info : sample_infos_) {
        size += sizeof(uint16_t); // ID
        size += sample_info.second.name.size() + 1; // +1 for the null terminator
    }

    size += sizeof(uint8_t);
    for (const auto& chr_info : chr_infos_) {
        size += sizeof(uint8_t); // ID
        size += chr_info.second.name.size() + 1;
        size += sizeof(uint64_t); // length
    }

    size += sizeof(uint32_t); // bin_size
    size += sizeof(uint32_t); // tile_size_
    size += sizeof(uint8_t);  // number of bin_size_multipliers_
    size += bin_size_multipliers_.size() * sizeof(uint32_t);

    size += sizeof(uint8_t); // number of normalization methods
    for (const auto& norm_method_info : norm_method_infos_) {
        size += sizeof(uint8_t);
        size += norm_method_info.second.name.size() + 1;
        size += sizeof(uint8_t); // mult_flag
    }

    size += sizeof(uint8_t); // number of normalized matrices
    for (const auto& norm_mat_info : norm_mat_infos_) {
        size += sizeof(uint8_t);
        size += norm_mat_info.second.name.size() + 1;
    }

    return size;
}

// ---------------------------------------------------------------------------------------------------------------------

//TODO(yeremia): use util::Bitwriter instead of core::Writer
void ContactMatrixParameters::Write(core::Writer& writer) const {
  writer.Write(static_cast<uint16_t>(sample_infos_.size()), 16);
    for(const auto& sample_info : sample_infos_) {
      writer.Write(sample_info.second.ID, 16);
        writer.Write(static_cast<uint16_t>(sample_info.second.name.size()), 8);
        writer.Write(sample_info.second.name);
    }

    writer.Write(static_cast<uint8_t>(chr_infos_.size()), 8);
    for(const auto& chr_info : chr_infos_) {
      writer.Write(chr_info.second.ID, 8);
        writer.Write(static_cast<uint16_t>(chr_info.second.name.size()), 8);
        writer.Write(chr_info.second.name);
        writer.Write(chr_info.second.length, 64);
    }

    writer.Write(bin_size_, 32);
    writer.Write(tile_size_, 32);

    writer.Write(static_cast<uint8_t>(bin_size_multipliers_.size()), 8);
    for(const auto& bin_size_multiplier : bin_size_multipliers_) {
      writer.Write(bin_size_multiplier, 32);
    }

    writer.Write(static_cast<uint8_t>(norm_method_infos_.size()), 8);
    for(const auto& method_info : norm_method_infos_) {
      writer.Write(method_info.second.ID, 8);
        writer.Write(static_cast<uint16_t>(method_info.second.name.size()), 8);
        writer.Write(method_info.second.name);
        writer.WriteReserved(7);
        writer.Write(method_info.second.mult_flag, 1);
    }

    writer.Write(static_cast<uint8_t>(norm_mat_infos_.size()), 8);
    for(const auto& mat_info : norm_mat_infos_) {
      writer.Write(mat_info.second.ID, 8);
        writer.Write(static_cast<uint16_t>(mat_info.second.name.size()), 8);
        writer.Write(mat_info.second.name);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ContactMatrixParameters::Write(util::BitWriter& writer) const {
  writer.WriteBits(static_cast<uint16_t>(sample_infos_.size()), 16);
  for(const auto& sample_info : sample_infos_) {
    writer.WriteBits(sample_info.second.ID, 16);
    writer.WriteBits(static_cast<uint16_t>(sample_info.second.name.size()), 8);
    writer.Write(sample_info.second.name);
  }

  writer.WriteBits(static_cast<uint8_t>(chr_infos_.size()), 8);
  for(const auto& chr_info : chr_infos_) {
    writer.WriteBits(chr_info.second.ID, 8);
    writer.WriteBits(static_cast<uint16_t>(chr_info.second.name.size()), 8);
    writer.Write(chr_info.second.name);
    writer.WriteBits(chr_info.second.length, 64);
  }

  writer.WriteBits(bin_size_, 32);
  writer.WriteBits(tile_size_, 32);

  writer.WriteBits(static_cast<uint8_t>(bin_size_multipliers_.size()), 8);
  for(const auto& bin_size_multiplier : bin_size_multipliers_) {
    writer.WriteBits(bin_size_multiplier, 32);
  }

  writer.WriteBits(static_cast<uint8_t>(norm_method_infos_.size()), 8);
  for(const auto& method_info : norm_method_infos_) {
    writer.WriteBits(method_info.second.ID, 8);
    writer.WriteBits(static_cast<uint16_t>(method_info.second.name.size()), 8);
    writer.Write(method_info.second.name);
    writer.WriteReserved(7);
    writer.WriteBits(method_info.second.mult_flag, 1);
  }

  writer.WriteBits(static_cast<uint8_t>(norm_mat_infos_.size()), 8);
  for(const auto& mat_info : norm_mat_infos_) {
    writer.WriteBits(mat_info.second.ID, 8);
    writer.WriteBits(static_cast<uint16_t>(mat_info.second.name.size()), 8);
    writer.Write(mat_info.second.name);
  }
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename KeyType, typename T>
bool mapsEqual(const std::unordered_map<KeyType, T>& current, const std::unordered_map<KeyType, T>& other) {
    if (current.size() != other.size()) {
        return false;
    }
    //TODO(yeremia): Improve this
    for (const auto& pair : current) {
        auto iter = other.find(pair.first);
        if (iter == other.end()) {
            return false;
        }
        //TODO(yeremia): operator != does not work
        if (!(pair.second == iter->second)){
            return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

bool ContactMatrixParameters::operator==(const ContactMatrixParameters& other) {
    if (this->GetNumSamples() != other.GetNumSamples()){
        return false;
    }
    if (!mapsEqual(this->sample_infos_, other.sample_infos_)){
        return false;
    }

    if (this->GetNumChromosomes() != other.GetNumChromosomes()){
        return false;
    }
    if (!mapsEqual(this->GetChromosomes(), other.GetChromosomes())){
        return false;
    }

    if (this->GetBinSize() != other.GetBinSize()){
        return false;
    }
    if (this->GetTileSize() != other.GetTileSize()){
        return false;
    }

    if (this->GetNumNormMethods() != other.GetNumNormMethods()){
        return false;
    }
    if (!mapsEqual(this->norm_method_infos_, other.norm_method_infos_)){
        return false;
    }

    if (this->GetNumNormMats() != other.GetNumNormMats()){
        return false;
    }
    if (!mapsEqual(this->norm_mat_infos_, other.norm_mat_infos_)){
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::contact

// ---------------------------------------------------------------------------------------------------------------------
