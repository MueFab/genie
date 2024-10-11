/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_group_protection.h"
#include <string>
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetGroupProtection::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const DatasetGroupProtection&>(info);
    return version == other.version && dataset_group_id == other.dataset_group_id &&
           dg_protection_value == other.dg_protection_value;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetGroupProtection::getKey() const {
    static const std::string key = "dgpr";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroupProtection::DatasetGroupProtection(util::BitReader& bitreader, const core::MPEGMinorVersion _version)
    : version(_version) {
    const auto start_pos = bitreader.getStreamPosition() - 4;
    const auto length = bitreader.readAlignedInt<uint64_t>();
    auto protection_length = length - getHeaderLength();
    if (version != core::MPEGMinorVersion::V1900) {
        dataset_group_id = bitreader.readAlignedInt<uint8_t>();
        protection_length -= sizeof(uint8_t);
    }
    dg_protection_value.resize(protection_length);
    bitreader.readAlignedBytes(dg_protection_value.data(), dg_protection_value.length());
    UTILS_DIE_IF(start_pos + length != static_cast<uint64_t>(bitreader.getStreamPosition()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroupProtection::DatasetGroupProtection(const uint8_t _dataset_group_id, std::string _dg_protection_value,
                                               const core::MPEGMinorVersion _version)
    : version(_version), dataset_group_id(_dataset_group_id), dg_protection_value(std::move(_dg_protection_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupProtection::box_write(util::BitWriter& bitWriter) const {
    if (version != core::MPEGMinorVersion::V1900) {
        bitWriter.writeAlignedInt(dataset_group_id);
    }
    bitWriter.writeAlignedBytes(dg_protection_value.data(), dg_protection_value.length());
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetGroupProtection::getDatasetGroupID() const { return dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetGroupProtection::getProtection() const { return dg_protection_value; }

// ---------------------------------------------------------------------------------------------------------------------

std::string DatasetGroupProtection::decapsulate() { return std::move(dg_protection_value); }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupProtection::patchID(const uint8_t groupID) { dataset_group_id = groupID; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
