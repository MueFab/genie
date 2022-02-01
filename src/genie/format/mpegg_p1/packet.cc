/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/packet.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Packet::Packet(PacketHeader _header, std::string _data) : header(_header), data(std::move(_data)) {}

// ---------------------------------------------------------------------------------------------------------------------

Packet::Packet(util::BitReader& reader) : header(reader) {
    data.resize(header.getPacketSize() - header.getLength());
    reader.readBypass(data);
}

// ---------------------------------------------------------------------------------------------------------------------

void Packet::write(util::BitWriter& writer) const {
    header.write(writer);
    writer.writeBypass(data.data(), data.size());
}

// ---------------------------------------------------------------------------------------------------------------------

const PacketHeader& Packet::getHeader() const { return header; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Packet::getData() const { return data; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
