/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset_metadata.h"
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetMetadata::getKey() const {
    static const std::string key = "dtmd";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetMetadata::DatasetMetadata(genie::util::BitReader& bitreader, genie::core::MPEGMinorVersion _version)
    : version(_version) {
    auto start_pos = bitreader.getPos() - 4;
    auto length = bitreader.readBypassBE<uint64_t>();
    auto metadata_length = length - GenInfo::getHeaderLength();
    if (version != genie::core::MPEGMinorVersion::V1900) {
        dataset_group_id = bitreader.readBypassBE<uint8_t>();
        dataset_id = bitreader.readBypassBE<uint16_t>();
        metadata_length -= sizeof(uint8_t);
        metadata_length -= sizeof(uint16_t);
    }
    dg_metatdata_value.resize(metadata_length);
    bitreader.readBypass(dg_metatdata_value);
    UTILS_DIE_IF(start_pos + length != uint64_t(bitreader.getPos()), "Invalid length");
    UTILS_DIE_IF(!bitreader.isGood(), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetMetadata::DatasetMetadata(uint8_t _dataset_group_id, uint16_t _dataset_id, std::string _dg_metatdata_value,
                                 genie::core::MPEGMinorVersion _version)
    : version(_version),
      dataset_group_id(_dataset_group_id),
      dataset_id(_dataset_id),
      dg_metatdata_value(std::move(_dg_metatdata_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetMetadata::box_write(genie::util::BitWriter& bitWriter) const {
    if (version != genie::core::MPEGMinorVersion::V1900) {
        bitWriter.writeBypassBE(dataset_group_id);
        bitWriter.writeBypassBE(dataset_id);
    }
    bitWriter.writeBypass(dg_metatdata_value.data(), dg_metatdata_value.length());
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetMetadata::getDatasetGroupID() const { return dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t DatasetMetadata::getDatasetID() const { return dataset_id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetMetadata::getMetadata() const { return dg_metatdata_value; }

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetMetadata::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const DatasetMetadata&>(info);
    return version == other.version && dataset_group_id == other.dataset_group_id && dataset_id == other.dataset_id &&
           dg_metatdata_value == other.dg_metatdata_value;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string DatasetMetadata::decapsulate() { return std::move(dg_metatdata_value); }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetMetadata::patchID(uint8_t _groupID, uint16_t _setID) {
    dataset_group_id = _groupID;
    dataset_id = _setID;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
