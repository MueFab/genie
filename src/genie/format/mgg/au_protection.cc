/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/au_protection.h"
#include <string>
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

const std::string& AUProtection::getKey() const {
    static const std::string key = "aupr";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

AUProtection::AUProtection(util::BitReader& bitreader, const core::MPEGMinorVersion _version) : version(_version) {
    const auto start_pos = bitreader.getStreamPosition() - 4;
    const auto length = bitreader.readAlignedInt<uint64_t>();
    auto metadata_length = length - getHeaderLength();
    if (version != core::MPEGMinorVersion::V1900) {
        dataset_group_id = bitreader.readAlignedInt<uint8_t>();
        dataset_id = bitreader.readAlignedInt<uint16_t>();
        metadata_length -= sizeof(uint8_t);
        metadata_length -= sizeof(uint16_t);
    }
    au_protection_value.resize(metadata_length);
    bitreader.readAlignedBytes(au_protection_value.data(), au_protection_value.length());
    UTILS_DIE_IF(start_pos + length != static_cast<uint64_t>(bitreader.getStreamPosition()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

AUProtection::AUProtection(const uint8_t _dataset_group_id, const uint16_t _dataset_id,
                           std::string _au_protection_value, const core::MPEGMinorVersion _version)
    : version(_version),
      dataset_group_id(_dataset_group_id),
      dataset_id(_dataset_id),
      au_protection_value(std::move(_au_protection_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

void AUProtection::box_write(util::BitWriter& bitWriter) const {
    if (version != core::MPEGMinorVersion::V1900) {
        bitWriter.writeAlignedInt(dataset_group_id);
        bitWriter.writeAlignedInt(dataset_id);
    }
    bitWriter.writeAlignedBytes(au_protection_value.data(), au_protection_value.length());
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t AUProtection::getDatasetGroupID() const { return dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t AUProtection::getDatasetID() const { return dataset_id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& AUProtection::getInformation() const { return au_protection_value; }

// ---------------------------------------------------------------------------------------------------------------------

bool AUProtection::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const AUProtection&>(info);
    return version == other.version && dataset_group_id == other.dataset_group_id &&
           dataset_group_id == other.dataset_group_id && dataset_id == other.dataset_id &&
           au_protection_value == other.au_protection_value;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string AUProtection::decapsulate() { return std::move(au_protection_value); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
