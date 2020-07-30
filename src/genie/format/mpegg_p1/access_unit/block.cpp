
#include "block.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(uint8_t _desc_ID, std::vector<uint8_t> &&payload)
    : block_header(_desc_ID, payload),
      block_payload(std::move(payload)){}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint8_t> &Block::getPayload() const {return block_payload;}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Block::getDescriptorID() const {return block_header.getDescriptorID();}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Block::getBlockPayloadSize() const {return block_header.getBlockPayloadSize();}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Block::getLength() const {
    // reserved u(1) + descriptor_ID u(7)
    uint64_t len = block_header.getLength();

    // block_payload_size u(32)
    len += block_payload.size();

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void Block::writeToFile(genie::util::BitWriter &bit_writer) const {

    // block_header
    block_header.writeToFile(bit_writer);

    // block_payload
    for (auto val: block_payload){
        bit_writer.write(val, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie