/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>
#include "genie/util/bitreader.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
#include "genie/core/record/annotation_access_unit/block.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

Block::Block() : block_header{}, block_payload{}, numChrs(0) {}

Block::Block(util::BitReader& reader, uint8_t num_Chrs) : block_header{}, block_payload{}, numChrs(num_Chrs) {
    read(reader);
}

Block::Block(BlockHeader block_header, BlockPayload block_payload, uint8_t numChrs)
    : block_header(block_header), block_payload(block_payload), numChrs(numChrs) {}

void Block::read(util::BitReader& reader) {
    block_header.read(reader);
    block_payload.read(reader, block_header.getDescriptorID(), numChrs);
}

void Block::read(util::BitReader& reader, uint8_t num_Chrs) {
    numChrs = num_Chrs;
    read(reader);
}

void Block::write(core::Writer& writer) const {
    block_header.write(writer);
    block_payload.write(writer);
}

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
