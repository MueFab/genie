/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/packet.h"

#include <string>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------
Packet::Packet(const PacketHeader header, std::string data)
    : header_(header), data_(std::move(data)) {}

// -----------------------------------------------------------------------------
Packet::Packet(util::BitReader& reader) : header_(reader) {
  data_.resize(header_.GetPacketSize() - PacketHeader::GetLength());
  reader.ReadAlignedBytes(data_.data(), data_.length());
}

// -----------------------------------------------------------------------------
void Packet::Write(util::BitWriter& writer) const {
  header_.Write(writer);
  writer.WriteAlignedBytes(data_.data(), data_.size());
}

// -----------------------------------------------------------------------------
const PacketHeader& Packet::GetHeader() const { return header_; }

// -----------------------------------------------------------------------------
const std::string& Packet::GetData() const { return data_; }

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
