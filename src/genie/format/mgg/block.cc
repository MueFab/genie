/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/block.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(util::BitReader& reader) : header(reader), payload(reader, header.getPayloadSize()) {}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Block::getPayloadSize() const { return header.getPayloadSize(); }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Block::getLength() const {
    uint64_t len = BlockHeader::getLength() + header.getPayloadSize();

    /// block_payload[] u(8)
    //    len += block_payload.size() * sizeof(uint8_t);

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void Block::write(util::BitWriter& writer) const {
    header.write(writer);
    payload.write(writer);
}

// ---------------------------------------------------------------------------------------------------------------------

bool Block::operator==(const Block& other) const {
    return header == other.header && payload.getPayload() == other.getPayload();
}

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(core::GenDesc _desc_id, util::DataBlock _payload)
    : header(false, _desc_id, 0, static_cast<uint32_t>(_payload.getRawSize())), payload(std::move(_payload)) {}

// ---------------------------------------------------------------------------------------------------------------------

const util::DataBlock& Block::getPayload() const { return payload.getPayload(); }

// ---------------------------------------------------------------------------------------------------------------------

core::GenDesc Block::getDescID() const { return header.getDescriptorID(); }

// ---------------------------------------------------------------------------------------------------------------------

void Block::print_debug(std::ostream& output, uint8_t depth, uint8_t max_depth) const {
    print_offset(output, depth, max_depth, "* Block");
    print_offset(output, depth + 1, max_depth,
                 "Block descriptor ID: " + getDescriptor(header.getDescriptorID()).name);
    print_offset(output, depth + 1, max_depth, "Block payload size: " + std::to_string(header.getPayloadSize()));
}

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(mgb::Block b)
    : header(false, static_cast<core::GenDesc>(b.getDescriptorID()), 0,
             static_cast<uint32_t>(b.getPayloadUnparsed().getPayloadSize())),
      payload(std::move(b.getPayloadUnparsed())) {}

// ---------------------------------------------------------------------------------------------------------------------

core::Payload Block::movePayload() { return std::move(payload); }

// ---------------------------------------------------------------------------------------------------------------------

mgb::Block Block::decapsulate() { return {header.getDescriptorID(), movePayload()}; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
