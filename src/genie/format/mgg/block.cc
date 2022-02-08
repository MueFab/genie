/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/block.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(util::BitReader& reader) : header(reader), payload(reader, header.getPayloadSize()) {}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Block::getPayloadSize() const { return header.getPayloadSize(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Block::getLength() const {
    uint64_t len = header.getLength() + header.getPayloadSize();

    /// block_payload[] u(8)
    //    len += block_payload.size() * sizeof(uint8_t);

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void Block::write(genie::util::BitWriter& writer) const {
    header.write(writer);
    payload.write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

bool Block::operator==(const Block& other) const {
    return header == other.header && payload.getPayload() == other.getPayload();
}

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(genie::core::GenDesc _desc_id, genie::util::DataBlock _payload)
    : header(false, _desc_id, 0, static_cast<uint32_t>(_payload.getRawSize())), payload(std::move(_payload)) {}

// ---------------------------------------------------------------------------------------------------------------------

const genie::util::DataBlock& Block::getPayload() const { return payload.getPayload(); }

// ---------------------------------------------------------------------------------------------------------------------

genie::core::GenDesc Block::getDescID() const { return header.getDescriptorID(); }

// ---------------------------------------------------------------------------------------------------------------------

void Block::print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const {
    print_offset(output, depth, max_depth, "* Block");
    print_offset(output, depth + 1, max_depth,
                 "Block descriptor ID: " + genie::core::getDescriptor(header.getDescriptorID()).name);
    print_offset(output, depth + 1, max_depth, "Block payload size: " + std::to_string(header.getPayloadSize()));
}

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(format::mgb::Block b)
    : header(false, core::GenDesc(b.getDescriptorID()), 0,
             static_cast<uint32_t>(b.getPayloadUnparsed().getPayloadSize())),
      payload(std::move(b.getPayloadUnparsed())) {}

// ---------------------------------------------------------------------------------------------------------------------

core::Payload Block::movePayload() { return std::move(boost::get<core::Payload>(payload)); }

// ---------------------------------------------------------------------------------------------------------------------

format::mgb::Block Block::decapsulate() { return {header.getDescriptorID(), movePayload()}; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
