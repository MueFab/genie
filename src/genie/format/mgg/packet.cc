/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/packet.h"
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

Packet::Packet(PacketHeader _header, std::string _data) : header(_header), data(std::move(_data)) {}

// ---------------------------------------------------------------------------------------------------------------------

Packet::Packet(util::BitReader& reader) : header(reader) {
    data.resize(header.getPacketSize() - genie::format::mgg::PacketHeader::getLength());
    reader.readAlignedBytes(data.data(), data.length());
}

// ---------------------------------------------------------------------------------------------------------------------

void Packet::write(util::BitWriter& writer) const {
    header.write(writer);
    writer.writeAlignedBytes(data.data(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------

const PacketHeader& Packet::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Packet::getData() const { return data; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
