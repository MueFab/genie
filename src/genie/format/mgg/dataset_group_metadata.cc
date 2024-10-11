/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_group_metadata.h"
#include <string>
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetGroupMetadata::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const DatasetGroupMetadata&>(info);
    return version == other.version && dataset_group_id == other.dataset_group_id &&
           dg_metatdata_value == other.dg_metatdata_value;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetGroupMetadata::getKey() const {
    static const std::string key = "dgmd";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroupMetadata::DatasetGroupMetadata(genie::util::BitReader& bitreader, genie::core::MPEGMinorVersion _version)
    : version(_version) {
    auto start_pos = bitreader.getStreamPosition() - 4;
    auto length = bitreader.readAlignedInt<uint64_t>();
    auto metadata_length = length - GenInfo::getHeaderLength();
    if (version != genie::core::MPEGMinorVersion::V1900) {
        dataset_group_id = bitreader.readAlignedInt<uint8_t>();
        metadata_length -= sizeof(uint8_t);
    }
    dg_metatdata_value.resize(metadata_length);
    bitreader.readAlignedBytes(dg_metatdata_value.data(), dg_metatdata_value.length());
    UTILS_DIE_IF(start_pos + length != static_cast<uint64_t>(bitreader.getStreamPosition()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroupMetadata::DatasetGroupMetadata(uint8_t _dataset_group_id, std::string _dg_metatdata_value,
                                           genie::core::MPEGMinorVersion _version)
    : version(_version), dataset_group_id(_dataset_group_id), dg_metatdata_value(std::move(_dg_metatdata_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupMetadata::box_write(genie::util::BitWriter& bitWriter) const {
    if (version != genie::core::MPEGMinorVersion::V1900) {
        bitWriter.writeAlignedInt(dataset_group_id);
    }
    bitWriter.writeAlignedBytes(dg_metatdata_value.data(), dg_metatdata_value.length());
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetGroupMetadata::getDatasetGroupID() const { return dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetGroupMetadata::getMetadata() const { return dg_metatdata_value; }

// ---------------------------------------------------------------------------------------------------------------------

std::string DatasetGroupMetadata::decapsulate() { return std::move(dg_metatdata_value); }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupMetadata::patchID(uint8_t groupID) { dataset_group_id = groupID; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
