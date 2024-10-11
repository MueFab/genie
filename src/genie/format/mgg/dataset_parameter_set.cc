/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_parameter_set.h"
#include <sstream>
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {
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
    auto start_pos = reader.getStreamPosition() - 4;
    auto length = reader.readAlignedInt<uint64_t>();
    dataset_group_id = reader.readAlignedInt<uint8_t>();
    dataset_id = reader.readAlignedInt<uint16_t>();
    parameter_set_ID = reader.readAlignedInt<uint8_t>();
    parent_parameter_set_ID = reader.readAlignedInt<uint8_t>();
    if (version != genie::core::MPEGMinorVersion::V1900 && parameters_update_flag) {
        param_update = dataset_parameterset::UpdateInfo(reader);
    }
    params = genie::core::parameter::EncodingSet(reader);
    UTILS_DIE_IF(start_pos + length != static_cast<uint64_t>(reader.getStreamPosition()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::box_write(genie::util::BitWriter& writer) const {
    writer.writeAlignedInt(dataset_group_id);
    writer.writeAlignedInt(dataset_id);
    writer.writeAlignedInt(parameter_set_ID);
    writer.writeAlignedInt(parent_parameter_set_ID);
    if (param_update != std::nullopt) {
        param_update->write(writer);
    }
    params.write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::addParameterUpdate(dataset_parameterset::UpdateInfo update) {
    if (version != core::MPEGMinorVersion::V1900) {
        param_update = update;
    }
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

bool DatasetParameterSet::hasParameterUpdate() const { return param_update != std::nullopt; }

// ---------------------------------------------------------------------------------------------------------------------

const dataset_parameterset::UpdateInfo& DatasetParameterSet::getParameterUpdate() const { return *param_update; }

// ---------------------------------------------------------------------------------------------------------------------

const genie::core::parameter::EncodingSet& DatasetParameterSet::getEncodingSet() const { return params; }

// ---------------------------------------------------------------------------------------------------------------------

genie::core::parameter::EncodingSet&& DatasetParameterSet::moveParameterSet() { return std::move(params); }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetParameterSet::patchID(uint8_t _groupID, uint16_t _setID) {
    dataset_group_id = _groupID;
    dataset_id = _setID;
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetParameterSet::DatasetParameterSet(uint8_t _dataset_group_id, uint16_t _dataset_id,
                                         genie::core::parameter::ParameterSet set, core::MPEGMinorVersion _version)
    : DatasetParameterSet(_dataset_group_id, _dataset_id, set.getID(), set.getParentID(),
                          std::move(set.getEncodingSet()), _version) {}

// ---------------------------------------------------------------------------------------------------------------------

genie::core::parameter::ParameterSet DatasetParameterSet::descapsulate() {
    return {parameter_set_ID, parent_parameter_set_ID, std::move(params)};
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
