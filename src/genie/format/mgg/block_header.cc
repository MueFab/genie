/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/block_header.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgg {

// ---------------------------------------------------------------------------------------------------------------------

BlockHeader::BlockHeader(util::BitReader& reader) {
    reserved1 = reader.read<bool>(1);
    descriptor_ID = reader.read<genie::core::GenDesc>(7);
    reserved2 = reader.read<uint8_t>(3);
    block_payload_size = reader.read<uint32_t>(29);
}

// ---------------------------------------------------------------------------------------------------------------------

genie::core::GenDesc BlockHeader::getDescriptorID() const { return descriptor_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t BlockHeader::getPayloadSize() const { return block_payload_size; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BlockHeader::getLength() const { return 1 + 4; }  /// 1 + 7 + 3 + 29}

// ---------------------------------------------------------------------------------------------------------------------

void BlockHeader::write(genie::util::BitWriter& writer) const {
    writer.write(reserved1, 1);
    writer.write(static_cast<uint8_t>(descriptor_ID), 7);
    writer.write(reserved2, 3);
    writer.write(block_payload_size, 29);
}

// ---------------------------------------------------------------------------------------------------------------------

BlockHeader::BlockHeader(bool _reserved1, genie::core::GenDesc _desc_id, uint8_t _reserved2, uint32_t payload_size)
    : reserved1(_reserved1), descriptor_ID(_desc_id), reserved2(_reserved2), block_payload_size(payload_size) {}

// ---------------------------------------------------------------------------------------------------------------------

bool BlockHeader::getReserved1() const { return reserved1; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t BlockHeader::getReserved2() const { return reserved2; }

// ---------------------------------------------------------------------------------------------------------------------

bool BlockHeader::operator==(const BlockHeader& other) const {
    return reserved1 == other.reserved1 && descriptor_ID == other.descriptor_ID && reserved2 == other.reserved2 &&
           block_payload_size == other.block_payload_size;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgg
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
