/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/dataset/access_unit/block.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(util::BitReader &reader) : descriptor_ID(), block_payload(0) {
    // reserved
    reader.read<uint8_t>(1);
    descriptor_ID = reader.read<uint8_t>(7);
    auto block_payload_size = reader.read<uint32_t>();

    for (uint32_t i = 0; i < block_payload_size; i++) {
        block_payload.push_back(reader.read<uint8_t>());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(uint8_t _desc_ID, std::list<uint8_t> &_block_payload)
    : descriptor_ID(_desc_ID), block_payload(std::move(_block_payload)) {}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Block::getDescID() const { return descriptor_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Block::getPayloadSize() const { return block_payload.size(); }

// ---------------------------------------------------------------------------------------------------------------------

const std::list<uint8_t> &Block::getPayload() const { return block_payload; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Block::getHeaderLength() const {
    // reserved u(1) + descriptor_ID u(7)
    uint64_t len = sizeof(uint8_t);

    // block_payload_size u(32)
    len += sizeof(uint32_t);
    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void Block::writeHeader(util::BitWriter &writer) const {
    // block_header - reserved u(1)
    writer.write(0, 1);

    // block_header - descriptor_ID(7)
    writer.write(descriptor_ID, 7);

    // block_payload_size u(32)
    writer.write(getPayloadSize(), 32);
}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Block::getLength() const {
    uint64_t len = getHeaderLength();

    // block_payload[]
    len += block_payload.size() * sizeof(uint8_t);

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void Block::write(genie::util::BitWriter &writer) const {
    writeHeader(writer);

    for (auto data : block_payload) {
        writer.write(data, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
