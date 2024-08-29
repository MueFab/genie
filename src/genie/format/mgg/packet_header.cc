/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/packet_header.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

uint16_t PacketHeader::getSID() const { return sid; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t PacketHeader::getReserved() const { return reserved; }

// ---------------------------------------------------------------------------------------------------------------------

bool PacketHeader::getMarkerBit() const { return marker_bit; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t PacketHeader::getSequenceNumber() const { return sequence_number; }

// ---------------------------------------------------------------------------------------------------------------------

uint16_t PacketHeader::getPacketSize() const { return packet_size; }

// ---------------------------------------------------------------------------------------------------------------------

PacketHeader::PacketHeader(uint16_t _sid, uint8_t _reserved, bool _marker_bit, uint8_t _sequence_number,
                           uint16_t _packet_size)
    : sid(_sid),
      reserved(_reserved),
      marker_bit(_marker_bit),
      sequence_number(_sequence_number),
      packet_size(_packet_size) {}

// ---------------------------------------------------------------------------------------------------------------------

PacketHeader::PacketHeader(util::BitReader& reader) {
    sid = reader.read<uint16_t>(13);
    reserved = reader.read<uint8_t>(3);
    marker_bit = reader.read<bool>(1);
    sequence_number = reader.read<uint8_t>(8);
    packet_size = reader.read<uint16_t>(15);
}

// ---------------------------------------------------------------------------------------------------------------------

void PacketHeader::write(util::BitWriter& writer) const {
    writer.write(sid, 13);
    writer.write(reserved, 3);
    writer.write(marker_bit, 1);
    writer.write(sequence_number, 8);
    writer.write(packet_size, 15);
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t PacketHeader::getLength() { return 5; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
