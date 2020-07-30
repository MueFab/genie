
#include "block_header.h"

namespace genie {
namespace format {
namespace mpegg_p1 {

BlockHeader::BlockHeader(uint8_t _desc_ID, std::vector<uint8_t> &payload)
    : descriptor_ID(_desc_ID),
      block_payload_size(payload.size()){}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t BlockHeader::getDescriptorID() const {return descriptor_ID;}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t BlockHeader::getBlockPayloadSize() const {return block_payload_size;}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t getLength(){

    // reserved u(1) + descriptor_ID u(7)
    uint64_t len = 1;

    // block_payload_size u(32)
    len += 4;

    return len;
}

// ---------------------------------------------------------------------------------------------------------------------

void BlockHeader::writeToFile(genie::util::BitWriter &bit_writer) const {

    // reserved u(1)
    bit_writer.write(0, 1);

    // descriptor_ID u(7)
    bit_writer.write(descriptor_ID, 7);

    // block_payload_size u(32)
    bit_writer.write(block_payload_size, 32);
}

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

