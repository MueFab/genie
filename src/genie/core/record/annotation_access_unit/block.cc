/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/record/annotation_access_unit/block.h"
#include <algorithm>
#include <string>
#include <iostream>
#include <utility>
#include "genie/util/bitreader.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

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
    std::cerr << "block " << AnnotDescToString(block_header.getDescriptorID()) << std::endl;
    block_header.write(writer);
    std::cerr << "payload...";
    block_payload.write(writer);
    std::cerr << "end" << std::endl;
}

void Block::set(BlockVectorData blockData) {
    numChrs = 0;
    const bool attribute_contiguity = false;
    const bool indexed = false;
    uint32_t blockSize = static_cast<uint32_t>(blockData.getDataSize());
    BlockHeader header(attribute_contiguity, blockData.getDescriptorID(), blockData.getAttributeID(), indexed,
                       blockSize);
    BlockPayload payload(blockData.getDescriptorID(), numChrs, blockSize, blockData.getData());
    block_header = header;
    block_payload = payload;
}

void Block::set(BlockData& blockData) {
    numChrs = 0;
    const bool attribute_contiguity = false;
    const bool indexed = false;
    uint32_t blockSize = static_cast<uint32_t>(blockData.getDataSize());
    BlockHeader header(attribute_contiguity, blockData.getDescriptorID(), blockData.getAttributeID(), indexed,
                       blockSize);
    BlockPayload payload(blockData.getDescriptorID(), numChrs, blockSize, blockData.getData());
    block_header = header;
    block_payload = payload;
}

size_t Block::getSize(core::Writer& writesize) const {
    write(writesize);
    return writesize.getBitsWritten();
}

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
