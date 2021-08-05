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

Block::Block(util::BitReader &reader, FileHeader& fhd) : header(reader, fhd) {

    for (auto i = 0; i < header.getPayloadSize(); i++) {
        block_payload.push_back(reader.read<uint8_t>());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t Block::getPayloadSize() const { return header.getPayloadSize(); }

// ---------------------------------------------------------------------------------------------------------------------

const std::list<uint8_t> &Block::getPayload() const { return block_payload; }

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

    for (auto& data : block_payload) {
        writer.write(data, 8);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
