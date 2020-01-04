#include "block.h"
#include <memory>

#include "util/bitwriter.h"

// -----------------------------------------------------------------------------------------------------------------

namespace format {
Block::Block(uint8_t _descriptor_ID, BlockPayloadSet::DescriptorPayload _payload)
    : reserved(0), descriptor_ID(_descriptor_ID), reserved_2(0), payload(std::move(_payload)) {}

// -----------------------------------------------------------------------------------------------------------------

Block::Block() : reserved(0), descriptor_ID(0), reserved_2(0), block_payload_size(0), payload(GenDesc(0)) {}

// -----------------------------------------------------------------------------------------------------------------

void Block::write(util::BitWriter &writer) const {
    writer.write(reserved, 1);
    writer.write(descriptor_ID, 7);
    writer.write(reserved_2, 3);

    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    payload.write(tmp_writer);
    tmp_writer.flush();
    uint64_t bits = tmp_writer.getBitsWritten();

    writer.write(bits / 8, 29);
    writer.write(&ss);
    writer.flush();
}
}  // namespace format