/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/data_stream.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

// ---------------------------------------------------------------------------------------------------------------------

bool DataStream::operator==(const DataStream& other) const {
    return data_type == other.data_type && reserved == other.reserved && data_SID == other.data_SID;
}

// ---------------------------------------------------------------------------------------------------------------------

DataStream::DataStream(Type _data_type, uint8_t _reserved, uint16_t _data_SID)
    : data_type(_data_type), reserved(_reserved), data_SID(_data_SID) {}

// ---------------------------------------------------------------------------------------------------------------------

DataStream::DataStream(util::BitReader& reader) {
    data_type = reader.read<Type>(8);
    reserved = reader.read<uint8_t>(3);
    data_SID = reader.read<uint16_t>(13);
}

// ---------------------------------------------------------------------------------------------------------------------

void DataStream::write(util::BitWriter& writer) const {
    writer.write(static_cast<uint8_t>(data_type), 8);
    writer.write(reserved, 3);
    writer.write(data_SID, 13);
}

// ---------------------------------------------------------------------------------------------------------------------

DataStream::Type DataStream::getDataType() const { return data_type; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t DataStream::getReserved() const { return reserved; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t DataStream::getDataSID() const { return data_SID; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
