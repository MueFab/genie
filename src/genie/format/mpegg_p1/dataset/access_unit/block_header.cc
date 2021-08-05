/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <genie/format/mpegg_p1//dataset/access_unit/block_header.h>

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

BlockHeader::BlockHeader(util::BitReader& reader, FileHeader& fhd){
    reader.read_b(1);
    descriptor_ID = reader.read<uint8_t>(7);

    reader.read_b(3);
    block_payload_size = reader.read<uint32_t>(29);

}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t BlockHeader::getDescriptorID() const { return descriptor_ID;}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t BlockHeader::getPayloadSize() const { return block_payload_size;}

// ---------------------------------------------------------------------------------------------------------------------

uint64_t BlockHeader::getLength() const {return 1 + 4;}  /// 1 + 7 + 3 + 29}

// ---------------------------------------------------------------------------------------------------------------------

void BlockHeader::write(genie::util::BitWriter & writer) const {
    writer.write(0, 1);
    writer.write(descriptor_ID, 7);
    writer.write(0, 3);
    writer.write(block_payload_size, 29);
}

}
}
}
// ---------------------------------------------------------------------------------------------------------------------
