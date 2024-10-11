/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/au_information.h"
#include <string>
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

const std::string& AUInformation::getKey() const {
    static const std::string key = "auin";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

AUInformation::AUInformation(util::BitReader& bitreader, const core::MPEGMinorVersion _version) : version(_version) {
    const auto start_pos = bitreader.getStreamPosition() - 4;
    const auto length = bitreader.readAlignedInt<uint64_t>();
    auto metadata_length = length - getHeaderLength();
    if (version != core::MPEGMinorVersion::V1900) {
        dataset_group_id = bitreader.readAlignedInt<uint8_t>();
        dataset_id = bitreader.readAlignedInt<uint16_t>();
        metadata_length -= sizeof(uint8_t);
        metadata_length -= sizeof(uint16_t);
    }
    au_information_value.resize(metadata_length);
    bitreader.readAlignedBytes(au_information_value.data(), au_information_value.length());
    UTILS_DIE_IF(start_pos + length != static_cast<uint64_t>(bitreader.getStreamPosition()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

AUInformation::AUInformation(const uint8_t _dataset_group_id, const uint16_t _dataset_id,
                             std::string _au_information_value, const core::MPEGMinorVersion _version)
    : version(_version),
      dataset_group_id(_dataset_group_id),
      dataset_id(_dataset_id),
      au_information_value(std::move(_au_information_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

void AUInformation::box_write(util::BitWriter& bitWriter) const {
    if (version != core::MPEGMinorVersion::V1900) {
        bitWriter.writeAlignedInt(dataset_group_id);
        bitWriter.writeAlignedInt(dataset_id);
    }
    bitWriter.writeAlignedBytes(au_information_value.data(), au_information_value.length());
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t AUInformation::getDatasetGroupID() const { return dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t AUInformation::getDatasetID() const { return dataset_id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& AUInformation::getInformation() const { return au_information_value; }

// ---------------------------------------------------------------------------------------------------------------------

bool AUInformation::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const AUInformation&>(info);
    return version == other.version && dataset_group_id == other.dataset_group_id &&
           dataset_group_id == other.dataset_group_id && dataset_id == other.dataset_id &&
           au_information_value == other.au_information_value;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string AUInformation::decapsulate() { return std::move(au_information_value); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
