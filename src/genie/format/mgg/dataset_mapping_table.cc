/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_mapping_table.h"
#include <string>
#include <utility>
#include <vector>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DataSetMappingTable::getKey() const {
    static const std::string key = "dmtb";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

void DataSetMappingTable::box_write(util::BitWriter& bitWriter) const {
    bitWriter.writeAlignedInt(dataset_id);
    for (const auto& s : streams) {
        s.write(bitWriter);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t DataSetMappingTable::getDatasetID() const { return dataset_id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<DataStream>& DataSetMappingTable::getDataStreams() const { return streams; }

// ---------------------------------------------------------------------------------------------------------------------

void DataSetMappingTable::addDataStream(DataStream d) { streams.emplace_back(d); }

// ---------------------------------------------------------------------------------------------------------------------

DataSetMappingTable::DataSetMappingTable(const uint16_t _dataset_id) : dataset_id(_dataset_id) {}

// ---------------------------------------------------------------------------------------------------------------------

DataSetMappingTable::DataSetMappingTable(util::BitReader& reader) {
    const auto start_pos = reader.getStreamPosition() - 4;
    const auto length = reader.readAlignedInt<uint64_t>();
    dataset_id = reader.readAlignedInt<uint16_t>();
    const size_t num_data_streams = (length - 14) / 3;
    for (size_t i = 0; i < num_data_streams; ++i) {
        streams.emplace_back(reader);
    }
    UTILS_DIE_IF(start_pos + length != static_cast<uint64_t>(reader.getStreamPosition()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

bool DataSetMappingTable::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const DataSetMappingTable&>(info);
    return dataset_id == other.dataset_id && streams == other.streams;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
