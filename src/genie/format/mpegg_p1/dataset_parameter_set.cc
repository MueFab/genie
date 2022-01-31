/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset_parameter_set.h"
#include <sstream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetParameterSet::USignature::operator==(const USignature& other) const {
    return u_signature_length == other.u_signature_length;
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetParameterSet::USignature::USignature() : u_signature_length(boost::none) {}

// ---------------------------------------------------------------------------------------------------------------------

DatasetParameterSet::USignature::USignature(genie::util::BitReader& reader) {
    if (reader.read<bool>(1)) {
        u_signature_length = reader.read<uint8_t>();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetParameterSet::USignature::isConstantLength() const { return u_signature_length != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetParameterSet::USignature::getConstLength() const { return *u_signature_length; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::USignature::setConstLength(uint8_t length) { u_signature_length = length; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::USignature::write(genie::util::BitWriter& writer) const {
    writer.write(u_signature_length != boost::none, 1);
    if (u_signature_length != boost::none) {
        writer.write(*u_signature_length, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetParameterSet::ParameterUpdateInfo::operator==(const ParameterUpdateInfo& other) const {
    return multiple_alignment_flag == other.multiple_alignment_flag && pos_40_bits_flag == other.pos_40_bits_flag &&
           alphabetId == other.alphabetId && u_signature == other.u_signature;
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetParameterSet::ParameterUpdateInfo::ParameterUpdateInfo(bool _multiple_alignment_flag, bool _pos_40_bits_flag,
                                                              core::AlphabetID _alphabetId)
    : multiple_alignment_flag(_multiple_alignment_flag), pos_40_bits_flag(_pos_40_bits_flag), alphabetId(_alphabetId) {}

// ---------------------------------------------------------------------------------------------------------------------

DatasetParameterSet::ParameterUpdateInfo::ParameterUpdateInfo(genie::util::BitReader& reader) {
    multiple_alignment_flag = reader.read<bool>(1);
    pos_40_bits_flag = reader.read<bool>(1);
    alphabetId = reader.read<core::AlphabetID>(8);
    if (reader.read<bool>(1)) {
        u_signature = USignature(reader);
    }
    reader.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::ParameterUpdateInfo::write(genie::util::BitWriter& writer) const {
    writer.write(multiple_alignment_flag, 1);
    writer.write(pos_40_bits_flag, 1);
    writer.write(static_cast<uint8_t>(alphabetId), 8);
    writer.write(u_signature != boost::none, 1);
    if (u_signature != boost::none) {
        u_signature->write(writer);
    }
    writer.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::ParameterUpdateInfo::addUSignature(USignature signature) { u_signature = signature; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetParameterSet::ParameterUpdateInfo::getMultipleAlignmentFlag() const { return multiple_alignment_flag; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetParameterSet::ParameterUpdateInfo::getPos40BitsFlag() const { return pos_40_bits_flag; }

// ---------------------------------------------------------------------------------------------------------------------

core::AlphabetID DatasetParameterSet::ParameterUpdateInfo::getAlphabetID() const { return alphabetId; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetParameterSet::ParameterUpdateInfo::hasUSignature() const { return u_signature != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

const DatasetParameterSet::USignature& DatasetParameterSet::ParameterUpdateInfo::getUSignature() const {
    return *u_signature;
}

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetParameterSet::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const DatasetParameterSet&>(info);
    return dataset_group_id == other.dataset_group_id && dataset_id == other.dataset_id &&
           parameter_set_ID == other.parameter_set_ID && parent_parameter_set_ID == other.parent_parameter_set_ID &&
           param_update == other.param_update && param_update == other.param_update && version == other.version;
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetParameterSet::DatasetParameterSet(uint8_t _dataset_group_id, uint16_t _dataset_id, uint8_t _parameter_set_ID,
                                         uint8_t _parent_parameter_set_ID, genie::core::parameter::EncodingSet ps,
                                         core::MPEGMinorVersion _version)
    : dataset_group_id(_dataset_group_id),
      dataset_id(_dataset_id),
      parameter_set_ID(_parameter_set_ID),
      parent_parameter_set_ID(_parent_parameter_set_ID),
      params(std::move(ps)),
      version(_version) {}

// ---------------------------------------------------------------------------------------------------------------------

DatasetParameterSet::DatasetParameterSet(genie::util::BitReader& reader, core::MPEGMinorVersion _version,
                                         bool parameters_update_flag)
    : version(_version) {
    reader.readBypassBE<uint64_t>();
    dataset_group_id = reader.readBypassBE<uint8_t>();
    dataset_id = reader.readBypassBE<uint16_t>();
    parameter_set_ID = reader.readBypassBE<uint8_t>();
    parent_parameter_set_ID = reader.readBypassBE<uint8_t>();
    if (version != genie::core::MPEGMinorVersion::V1900 && parameters_update_flag) {
        param_update = ParameterUpdateInfo(reader);
    }
    params = genie::core::parameter::EncodingSet(reader);
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::write(genie::util::BitWriter& writer) const {
    GenInfo::write(writer);
    writer.writeBypassBE(dataset_group_id);
    writer.writeBypassBE(dataset_id);
    writer.writeBypassBE(parameter_set_ID);
    writer.writeBypassBE(parent_parameter_set_ID);
    if (param_update != boost::none) {
        param_update->write(writer);
    }
    params.write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::addParameterUpdate(ParameterUpdateInfo update) {
    if (version != core::MPEGMinorVersion::V1900) {
        param_update = std::move(update);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetParameterSet::getSize() const {
    std::stringstream stream;
    genie::util::BitWriter writer(&stream);
    write(writer);
    return stream.str().length();
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetParameterSet::getKey() const {
    static const std::string key = "pars";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetParameterSet::getDatasetGroupID() const { return dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t DatasetParameterSet::getDatasetID() const { return dataset_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetParameterSet::getParameterSetID() const { return parameter_set_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetParameterSet::getParentParameterSetID() const { return parent_parameter_set_ID; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetParameterSet::hasParameterUpdate() const { return param_update != boost::none; }

// ---------------------------------------------------------------------------------------------------------------------

const DatasetParameterSet::ParameterUpdateInfo& DatasetParameterSet::getParameterUpdate() const {
    return *param_update;
}

// ---------------------------------------------------------------------------------------------------------------------

const genie::core::parameter::EncodingSet& DatasetParameterSet::getEncodingSet() const { return params; }

// ---------------------------------------------------------------------------------------------------------------------

genie::core::parameter::EncodingSet&& DatasetParameterSet::moveParameterSet() { return std::move(params); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
