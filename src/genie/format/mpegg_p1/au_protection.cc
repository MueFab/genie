/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/au_protection.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

const std::string& AUProtection::getKey() const {
    static const std::string key = "aupr";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

AUProtection::AUProtection(genie::util::BitReader& bitreader, genie::core::MPEGMinorVersion _version)
    : version(_version) {
    auto length = bitreader.readBypassBE<uint64_t>();
    auto metadata_length = length - GenInfo::getSize();
    if (version != genie::core::MPEGMinorVersion::V1900) {
        dataset_group_id = bitreader.readBypassBE<uint8_t>();
        dataset_id = bitreader.readBypassBE<uint16_t>();
        metadata_length -= sizeof(uint8_t);
        metadata_length -= sizeof(uint16_t);
    }
    au_protection_value.resize(metadata_length);
    bitreader.readBypass(au_protection_value);
}

// ---------------------------------------------------------------------------------------------------------------------

AUProtection::AUProtection(uint8_t _dataset_group_id, uint16_t _dataset_id, std::string _au_protection_value,
                           genie::core::MPEGMinorVersion _version)
    : version(_version),
      dataset_group_id(_dataset_group_id),
      dataset_id(_dataset_id),
      au_protection_value(std::move(_au_protection_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

void AUProtection::write(genie::util::BitWriter& bitWriter) const {
    GenInfo::write(bitWriter);
    if (version != genie::core::MPEGMinorVersion::V1900) {
        bitWriter.writeBypassBE(dataset_group_id);
        bitWriter.writeBypassBE(dataset_id);
    }
    bitWriter.writeBypass(au_protection_value.data(), au_protection_value.length());
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t AUProtection::getSize() const {
    return GenInfo::getSize() + au_protection_value.size() +
           (version != genie::core::MPEGMinorVersion::V1900 ? sizeof(uint8_t) + sizeof(uint16_t) : 0);
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

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
