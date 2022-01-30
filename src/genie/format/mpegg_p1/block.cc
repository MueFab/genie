/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "block.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(util::BitReader &reader) : header(reader), block_payload() {
    block_payload.resize(header.getPayloadSize());
    reader.readBypass(block_payload.getData(), header.getPayloadSize());
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

void Block::write(genie::util::BitWriter &writer) const {
    header.write(writer);
    writer.writeBypass(block_payload.getData(), block_payload.getRawSize());
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
