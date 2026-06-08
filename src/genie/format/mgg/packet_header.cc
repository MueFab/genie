/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/packet_header.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------

uint16_t PacketHeader::GetSid() const { return sid_; }

// -----------------------------------------------------------------------------

uint8_t PacketHeader::GetReserved() const { return reserved_; }

// -----------------------------------------------------------------------------

bool PacketHeader::GetMarkerBit() const { return marker_bit_; }

// -----------------------------------------------------------------------------

uint8_t PacketHeader::GetSequenceNumber() const { return sequence_number_; }

// -----------------------------------------------------------------------------

uint16_t PacketHeader::GetPacketSize() const { return packet_size_; }

// -----------------------------------------------------------------------------

PacketHeader::PacketHeader(const uint16_t sid, const uint8_t reserved,
                           const bool marker_bit,
                           const uint8_t sequence_number,
                           const uint16_t packet_size)
    : sid_(sid),
      reserved_(reserved),
      marker_bit_(marker_bit),
      sequence_number_(sequence_number),
      packet_size_(packet_size) {}

// -----------------------------------------------------------------------------

PacketHeader::PacketHeader(util::BitReader& reader) {
  sid_ = reader.Read<uint16_t>(13);
  reserved_ = reader.Read<uint8_t>(3);
  marker_bit_ = reader.Read<bool>(1);
  sequence_number_ = reader.Read<uint8_t>(8);
  packet_size_ = reader.Read<uint16_t>(15);
}

// -----------------------------------------------------------------------------

void PacketHeader::Write(util::BitWriter& writer) const {
  writer.WriteBits(sid_, 13);
  writer.WriteBits(reserved_, 3);
  writer.WriteBits(marker_bit_, 1);
  writer.WriteBits(sequence_number_, 8);
  writer.WriteBits(packet_size_, 15);
}

// -----------------------------------------------------------------------------

uint64_t PacketHeader::GetLength() { return 5; }

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
