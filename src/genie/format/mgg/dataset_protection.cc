/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_protection.h"
#include <string>
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetProtection::getKey() const {
    static const std::string key = "dtpr";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetProtection::DatasetProtection(util::BitReader& bitreader, const core::MPEGMinorVersion _version)
    : version(_version) {
    const auto start_pos = bitreader.getStreamPosition();
    const auto length = bitreader.readAlignedInt<uint64_t>();
    auto protection_length = length - getHeaderLength();
    if (version != core::MPEGMinorVersion::V1900) {
        dataset_group_id = bitreader.readAlignedInt<uint8_t>();
        dataset_id = bitreader.readAlignedInt<uint16_t>();
        protection_length -= sizeof(uint8_t);
        protection_length -= sizeof(uint16_t);
    }
    dg_protection_value.resize(protection_length);
    bitreader.readAlignedBytes(dg_protection_value.data(), dg_protection_value.length());
    UTILS_DIE_IF(start_pos + length != static_cast<uint64_t>(bitreader.getStreamPosition()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetProtection::DatasetProtection(const uint8_t _dataset_group_id, const uint16_t _dataset_id,
                                     std::string _dg_protection_value, const core::MPEGMinorVersion _version)
    : version(_version),
      dataset_group_id(_dataset_group_id),
      dataset_id(_dataset_id),
      dg_protection_value(std::move(_dg_protection_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetProtection::box_write(util::BitWriter& bitWriter) const {
    if (version != core::MPEGMinorVersion::V1900) {
        bitWriter.writeAlignedInt(dataset_group_id);
        bitWriter.writeAlignedInt(dataset_id);
    }
    bitWriter.writeAlignedBytes(dg_protection_value.data(), dg_protection_value.length());
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetProtection::getDatasetGroupID() const { return dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t DatasetProtection::getDatasetID() const { return dataset_id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetProtection::getProtection() const { return dg_protection_value; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetProtection::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const DatasetProtection&>(info);
    return version == other.version && dataset_group_id == other.dataset_group_id && dataset_id == other.dataset_id &&
           dg_protection_value == other.dg_protection_value;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string DatasetProtection::decapsulate() { return std::move(dg_protection_value); }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetProtection::patchID(const uint8_t _groupID, const uint16_t _setID) {
    dataset_group_id = _groupID;
    dataset_id = _setID;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
