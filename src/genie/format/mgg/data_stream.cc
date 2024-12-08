/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/data_stream.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------
bool DataStream::operator==(const DataStream& other) const {
  return data_type_ == other.data_type_ && reserved_ == other.reserved_ &&
         data_sid_ == other.data_sid_;
}

// -----------------------------------------------------------------------------
DataStream::DataStream(const Type data_type, const uint8_t reserved,
                       const uint16_t data_sid)
    : data_type_(data_type), reserved_(reserved), data_sid_(data_sid) {}

// -----------------------------------------------------------------------------
DataStream::DataStream(util::BitReader& reader) {
  data_type_ = reader.Read<Type>(8);
  reserved_ = reader.Read<uint8_t>(3);
  data_sid_ = reader.Read<uint16_t>(13);
}

// -----------------------------------------------------------------------------
void DataStream::Write(util::BitWriter& writer) const {
  writer.WriteBits(static_cast<uint8_t>(data_type_), 8);
  writer.WriteBits(reserved_, 3);
  writer.WriteBits(data_sid_, 13);
}

// -----------------------------------------------------------------------------
DataStream::Type DataStream::GetDataType() const { return data_type_; }

// -----------------------------------------------------------------------------
uint8_t DataStream::GetReserved() const { return reserved_; }

// -----------------------------------------------------------------------------
uint16_t DataStream::GetDataSid() const { return data_sid_; }

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
