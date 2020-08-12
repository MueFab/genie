
#include "block.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(uint8_t _desc_ID, core::AccessUnit::Descriptor &&_payload)
    : mgb::Block(_desc_ID, std::move(_payload)){}
// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint8_t> &Block::getPayload() const {return block_payload;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Block::getID() const {return getDescriptorID();}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Block::getPayloadSize() const {return getPayload().getWrittenSize();}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Block::getLength() const {
    // reserved u(1) + descriptor_ID u(7)
    uint64_t len = 1;

    // block_payload_size u(32)
    len += 4;

    // block_payload[]
    len += mgb::Block::getPayload().getWrittenSize();

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void Block::write(genie::util::BitWriter &bit_writer) const {

    // block_header - reserved u(1)
    bit_writer.write(0, 1);

    // block_header - descriptor_ID(7)
    bit_writer.write(getDescriptorID(), 7);

    // block_payload_size u(32)
    bit_writer.write(getLength(), 29);

//    // block_payload
    mgb::Block::getPayload().write(bit_writer);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie