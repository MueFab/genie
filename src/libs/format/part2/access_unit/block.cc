#include "block.h"

#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
Block::Block(uint8_t _descriptor_ID, std::vector<uint8_t> *_payload)
    : reserved(0),
      descriptor_ID(_descriptor_ID),
      reserved_2(0),
      block_payload_size(_payload->size()),
      payload(std::move(*_payload)) {}

// -----------------------------------------------------------------------------------------------------------------

Block::Block() : reserved(0), descriptor_ID(0), reserved_2(0), block_payload_size(0), payload(0) {}

// -----------------------------------------------------------------------------------------------------------------

void Block::write(util::BitWriter *writer) {
    writer->write(reserved, 1);
    writer->write(descriptor_ID, 7);
    writer->write(reserved_2, 3);
    writer->write(block_payload_size, 29);
    for (auto &i : payload) {
        writer->write(i, 8);
    }
    writer->flush();
}

uint32_t Block::getTotalSize() { return block_payload_size + 5; }
}  // namespace format
