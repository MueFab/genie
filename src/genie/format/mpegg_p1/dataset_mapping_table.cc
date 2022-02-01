/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset_mapping_table.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

bool DataSetMappingTable::DataStream::operator==(const DataStream& other) const {
    return data_type == other.data_type && reserved == other.reserved && data_SID == other.data_SID;
}

// ---------------------------------------------------------------------------------------------------------------------

DataSetMappingTable::DataStream::DataStream(Type _data_type, uint8_t _reserved, uint16_t _data_SID)
    : data_type(_data_type), reserved(_reserved), data_SID(_data_SID) {}

// ---------------------------------------------------------------------------------------------------------------------

DataSetMappingTable::DataStream::DataStream(util::BitReader& reader) {
    data_type = reader.read<Type>(8);
    reserved = reader.read<uint8_t>(3);
    data_SID = reader.read<uint16_t>(13);
}

// ---------------------------------------------------------------------------------------------------------------------

void DataSetMappingTable::DataStream::write(util::BitWriter& writer) const {
    writer.write(static_cast<uint8_t>(data_type), 8);
    writer.write(reserved, 3);
    writer.write(data_SID, 13);
}

// ---------------------------------------------------------------------------------------------------------------------

DataSetMappingTable::DataStream::Type DataSetMappingTable::DataStream::getDataType() const { return data_type; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DataSetMappingTable::DataStream::getReserved() const { return reserved; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t DataSetMappingTable::DataStream::getDataSID() const { return data_SID; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DataSetMappingTable::getKey() const {
    static const std::string key = "dmtb";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

void DataSetMappingTable::write(genie::util::BitWriter& bitWriter) const {
    GenInfo::write(bitWriter);
    bitWriter.writeBypassBE(dataset_id);
    for (const auto& s : streams) {
        s.write(bitWriter);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t DataSetMappingTable::getSize() const {
    return GenInfo::getSize() + sizeof(uint16_t) + (sizeof(uint8_t) + sizeof(uint16_t)) * streams.size();
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t DataSetMappingTable::getDatasetID() const { return dataset_id; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<DataSetMappingTable::DataStream>& DataSetMappingTable::getDataStreams() const { return streams; }

// ---------------------------------------------------------------------------------------------------------------------

void DataSetMappingTable::addDataStream(DataStream d) { streams.emplace_back(std::move(d)); }

// ---------------------------------------------------------------------------------------------------------------------

DataSetMappingTable::DataSetMappingTable(uint16_t _dataset_id) : dataset_id(_dataset_id) {}

// ---------------------------------------------------------------------------------------------------------------------

DataSetMappingTable::DataSetMappingTable(util::BitReader& reader) {
    uint64_t length = reader.readBypassBE<uint64_t>();
    dataset_id = reader.readBypassBE<uint16_t>();
    size_t num_data_streams = (length - 14) / 3;
    for (size_t i = 0; i < num_data_streams; ++i) {
        streams.emplace_back(reader);
    }
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

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
