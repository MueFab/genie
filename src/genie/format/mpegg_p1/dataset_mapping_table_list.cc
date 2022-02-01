/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset_mapping_table_list.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

bool DataSetMappingTableList::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const DataSetMappingTableList&>(info);
    return dataset_group_ID == other.dataset_group_ID && dataset_mapping_table_SID == other.dataset_mapping_table_SID;
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DataSetMappingTableList::getKey() const {
    static const std::string key = "dmtl";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

DataSetMappingTableList::DataSetMappingTableList(uint8_t _ds_group_id) : dataset_group_ID(_ds_group_id) {}

// ---------------------------------------------------------------------------------------------------------------------

DataSetMappingTableList::DataSetMappingTableList(util::BitReader& reader) {
    size_t length = reader.readBypassBE<uint64_t>();
    dataset_group_ID = reader.readBypassBE<uint8_t>();
    size_t num_SIDs = (length - 13) / 2;
    for (size_t i = 0; i < num_SIDs; ++i) {
        dataset_mapping_table_SID.emplace_back(reader.readBypassBE<uint16_t>());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void DataSetMappingTableList::addDatasetMappingTableSID(uint16_t sid) { dataset_mapping_table_SID.emplace_back(sid); }

// ---------------------------------------------------------------------------------------------------------------------

void DataSetMappingTableList::write(genie::util::BitWriter& bitWriter) const {
    GenInfo::write(bitWriter);
    bitWriter.writeBypassBE(dataset_group_ID);
    for (const auto& s : dataset_mapping_table_SID) {
        bitWriter.writeBypassBE(s);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DataSetMappingTableList::getDatasetGroupID() const { return dataset_group_ID; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint16_t>& DataSetMappingTableList::getDatasetMappingTableSIDs() const {
    return dataset_mapping_table_SID;
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DataSetMappingTableList::getSize() const {
    return GenInfo::getSize() + sizeof(uint8_t) + sizeof(uint16_t) * dataset_mapping_table_SID.size();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
