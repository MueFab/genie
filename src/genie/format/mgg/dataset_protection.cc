/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/dataset_protection.h"
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetProtection::getKey() const {
    static const std::string key = "dtpr";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetProtection::DatasetProtection(genie::util::BitReader& bitreader, genie::core::MPEGMinorVersion _version)
    : version(_version) {
    auto start_pos = bitreader.getPos();
    auto length = bitreader.readBypassBE<uint64_t>();
    auto protection_length = length - GenInfo::getHeaderLength();
    if (version != genie::core::MPEGMinorVersion::V1900) {
        dataset_group_id = bitreader.readBypassBE<uint8_t>();
        dataset_id = bitreader.readBypassBE<uint16_t>();
        protection_length -= sizeof(uint8_t);
        protection_length -= sizeof(uint16_t);
    }
    dg_protection_value.resize(protection_length);
    bitreader.readBypass(dg_protection_value);
    UTILS_DIE_IF(start_pos + length != uint64_t(bitreader.getPos()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetProtection::DatasetProtection(uint8_t _dataset_group_id, uint16_t _dataset_id, std::string _dg_protection_value,
                                     genie::core::MPEGMinorVersion _version)
    : version(_version),
      dataset_group_id(_dataset_group_id),
      dataset_id(_dataset_id),
      dg_protection_value(std::move(_dg_protection_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetProtection::box_write(genie::util::BitWriter& bitWriter) const {
    if (version != genie::core::MPEGMinorVersion::V1900) {
        bitWriter.writeBypassBE(dataset_group_id);
        bitWriter.writeBypassBE(dataset_id);
    }
    bitWriter.writeBypass(dg_protection_value.data(), dg_protection_value.length());
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

void DatasetProtection::patchID(uint8_t _groupID, uint16_t _setID) {
    dataset_group_id = _groupID;
    dataset_id = _setID;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
