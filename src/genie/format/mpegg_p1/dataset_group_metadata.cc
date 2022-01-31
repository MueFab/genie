/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset_group_metadata.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

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
    auto length = bitreader.readBypassBE<uint64_t>();
    auto metadata_length = length - GenInfo::getSize();
    if (version != genie::core::MPEGMinorVersion::V1900) {
        dataset_group_id = bitreader.readBypassBE<uint8_t>();
        metadata_length -= sizeof(uint8_t);
    }
    dg_metatdata_value.resize(metadata_length);
    bitreader.readBypass(dg_metatdata_value);
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroupMetadata::DatasetGroupMetadata(uint8_t _dataset_group_id, std::string _dg_metatdata_value,
                                           genie::core::MPEGMinorVersion _version)
    : version(_version), dataset_group_id(_dataset_group_id), dg_metatdata_value(std::move(_dg_metatdata_value)) {}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupMetadata::write(genie::util::BitWriter& bitWriter) const {
    GenInfo::write(bitWriter);
    if (version != genie::core::MPEGMinorVersion::V1900) {
        bitWriter.writeBypassBE(dataset_group_id);
    }
    bitWriter.writeBypass(dg_metatdata_value.data(), dg_metatdata_value.length());
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetGroupMetadata::getSize() const {
    return GenInfo::getSize() + dg_metatdata_value.size() +
           (version != genie::core::MPEGMinorVersion::V1900 ? sizeof(uint8_t) : 0);
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetGroupMetadata::getDatasetGroupID() const { return dataset_group_id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetGroupMetadata::getMetadata() const { return dg_metatdata_value; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
