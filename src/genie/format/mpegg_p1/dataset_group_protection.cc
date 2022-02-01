/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset_group_protection.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

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

DatasetGroupProtection::DatasetGroupProtection(genie::util::BitReader& bitreader,
                                               genie::core::MPEGMinorVersion _version)
    : version(_version) {
    auto length = bitreader.readBypassBE<uint64_t>();
    auto protection_length = length - GenInfo::getSize();
    if (version != genie::core::MPEGMinorVersion::V1900) {
        dataset_group_id = bitreader.readBypassBE<uint8_t>();
        protection_length -= sizeof(uint8_t);
    }
    dg_protection_value.resize(protection_length);
    bitreader.readBypass(dg_protection_value);
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroupProtection::DatasetGroupProtection(uint8_t _dataset_group_id, std::string _dg_protection_value,
                                               genie::core::MPEGMinorVersion _version)
    : version(_version), dataset_group_id(_dataset_group_id), dg_protection_value(std::move(_dg_protection_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupProtection::write(genie::util::BitWriter& bitWriter) const {
    GenInfo::write(bitWriter);
    if (version != genie::core::MPEGMinorVersion::V1900) {
        bitWriter.writeBypassBE(dataset_group_id);
    }
    bitWriter.writeBypass(dg_protection_value.data(), dg_protection_value.length());
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetGroupProtection::getSize() const {
    return GenInfo::getSize() + dg_protection_value.size() +
           (version != genie::core::MPEGMinorVersion::V1900 ? sizeof(uint8_t) : 0);
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetGroupProtection::getDatasetGroupID() const { return dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetGroupProtection::getProtection() const { return dg_protection_value; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
