/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/dataset_group_protection.h"
#include <utility>
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

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
                                               genie::core::MpegMinorVersion _version)
    : version(_version) {
    auto start_pos = bitreader.getPos() - 4;
    auto length = bitreader.readBypassBE<uint64_t>();
    auto protection_length = length - GenInfo::getHeaderLength();
    if (version != genie::core::MpegMinorVersion::kV1900) {
        dataset_group_id = bitreader.readBypassBE<uint8_t>();
        protection_length -= sizeof(uint8_t);
    }
    dg_protection_value.resize(protection_length);
    bitreader.readBypass(dg_protection_value);
    UTILS_DIE_IF(start_pos + length != uint64_t(bitreader.getPos()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroupProtection::DatasetGroupProtection(uint8_t _dataset_group_id, std::string _dg_protection_value,
                                               genie::core::MpegMinorVersion _version)
    : version(_version), dataset_group_id(_dataset_group_id), dg_protection_value(std::move(_dg_protection_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupProtection::box_write(genie::util::BitWriter& bitWriter) const {
    if (version != genie::core::MpegMinorVersion::kV1900) {
        bitWriter.writeBypassBE(dataset_group_id);
    }
    bitWriter.writeBypass(dg_protection_value.data(), dg_protection_value.length());
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetGroupProtection::getDatasetGroupID() const { return dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetGroupProtection::getProtection() const { return dg_protection_value; }

// ---------------------------------------------------------------------------------------------------------------------

std::string DatasetGroupProtection::decapsulate() { return std::move(dg_protection_value); }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupProtection::patchID(uint8_t groupID) { dataset_group_id = groupID; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
