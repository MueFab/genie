/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/au_protection.h"
#include <utility>
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

// ---------------------------------------------------------------------------------------------------------------------

const std::string& AUProtection::getKey() const {
    static const std::string key = "aupr";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

AUProtection::AUProtection(genie::util::BitReader& bitreader, genie::core::MpegMinorVersion _version)
    : version(_version) {
    auto start_pos = bitreader.getPos() - 4;
    auto length = bitreader.readBypassBE<uint64_t>();
    auto metadata_length = length - GenInfo::getHeaderLength();
    if (version != genie::core::MpegMinorVersion::kV1900) {
        dataset_group_id = bitreader.readBypassBE<uint8_t>();
        dataset_id = bitreader.readBypassBE<uint16_t>();
        metadata_length -= sizeof(uint8_t);
        metadata_length -= sizeof(uint16_t);
    }
    au_protection_value.resize(metadata_length);
    bitreader.readBypass(au_protection_value);
    UTILS_DIE_IF(start_pos + length != uint64_t(bitreader.getPos()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

AUProtection::AUProtection(uint8_t _dataset_group_id, uint16_t _dataset_id, std::string _au_protection_value,
                           genie::core::MpegMinorVersion _version)
    : version(_version),
      dataset_group_id(_dataset_group_id),
      dataset_id(_dataset_id),
      au_protection_value(std::move(_au_protection_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

void AUProtection::box_write(genie::util::BitWriter& bitWriter) const {
    if (version != genie::core::MpegMinorVersion::kV1900) {
        bitWriter.writeBypassBE(dataset_group_id);
        bitWriter.writeBypassBE(dataset_id);
    }
    bitWriter.writeBypass(au_protection_value.data(), au_protection_value.length());
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

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
