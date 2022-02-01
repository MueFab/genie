/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/block.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(util::BitReader& reader) : header(reader), block_payload(), internal_reader(&reader) {
    payloadLoaded = false;
    payloadPosition = reader.getPos();
    reader.skip(header.getPayloadSize());
}

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
    if (!isPayloadLoaded() && internal_reader) {
        auto tmp = _internal_loadPayload(*internal_reader);
        writer.writeBypass(tmp.getData(), tmp.getRawSize());
    } else {
        writer.writeBypass(block_payload.getData(), block_payload.getRawSize());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool Block::operator==(const Block& other) const {
    return header == other.header && block_payload == other.block_payload;
}

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(genie::core::GenDesc _desc_id, genie::util::DataBlock payload)
    : header(false, _desc_id, 0, payload.getRawSize()),
      block_payload(std::move(payload)),
      payloadLoaded(true),
      payloadPosition(-1),
      internal_reader(nullptr) {}

// ---------------------------------------------------------------------------------------------------------------------

const genie::util::DataBlock& Block::getPayload() const { return block_payload; }

// ---------------------------------------------------------------------------------------------------------------------

genie::util::DataBlock&& Block::movePayload() { return std::move(block_payload); }

// ---------------------------------------------------------------------------------------------------------------------

genie::core::GenDesc Block::getDescID() const { return header.getDescriptorID(); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
