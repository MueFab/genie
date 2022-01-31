/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset_group_header.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

bool DatasetGroupHeader::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const DatasetGroupHeader&>(info);
    return ID == other.ID && version == other.version && dataset_IDs == other.dataset_IDs;
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroupHeader::DatasetGroupHeader() : DatasetGroupHeader(0, 0) {}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroupHeader::DatasetGroupHeader(uint8_t _id, uint8_t _version) : ID(_id), version(_version) {}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DatasetGroupHeader::getKey() const {
    static const std::string key = "dghd";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

DatasetGroupHeader::DatasetGroupHeader(genie::util::BitReader& reader) {
    auto length = reader.readBypassBE<uint64_t>();
    auto num_datasets = (length - 14) / 2;
    dataset_IDs.resize(num_datasets);
    ID = reader.readBypassBE<uint8_t>();
    version = reader.readBypassBE<uint8_t>();
    for (auto& d : dataset_IDs) {
        d = reader.readBypassBE<uint16_t>();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetGroupHeader::getID() const { return ID; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupHeader::setID(uint8_t _ID) { ID = _ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DatasetGroupHeader::getVersion() const { return version; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint16_t>& DatasetGroupHeader::getDatasetIDs() const { return dataset_IDs; }

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupHeader::addDatasetID(uint16_t _id) { ID = _id; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DatasetGroupHeader::getSize() const {
    return GenInfo::getSize() + sizeof(uint8_t) * 2 + sizeof(uint16_t) * dataset_IDs.size();
}

// ---------------------------------------------------------------------------------------------------------------------

void DatasetGroupHeader::write(genie::util::BitWriter& writer) const {
    GenInfo::write(writer);
    writer.writeBypassBE<uint8_t>(ID);
    writer.writeBypassBE<uint8_t>(version);
    for (auto& d : dataset_IDs) {
        writer.writeBypassBE<uint16_t>(d);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
