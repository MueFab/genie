/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "block.h"
#include <genie/util/bitwriter.h>
#include <memory>
#include <sstream>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(uint8_t _descriptor_ID, core::AccessUnitPayload::DescriptorPayload _payload)
    : descriptor_ID(_descriptor_ID), payload(std::move(_payload)) {}

// ---------------------------------------------------------------------------------------------------------------------

Block::Block() : descriptor_ID(0), block_payload_size(0), payload(core::GenDesc(0)) {}

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(size_t qv_count, util::BitReader &reader) {
    reader.read(1);
    descriptor_ID = reader.read<uint8_t>(7);
    reader.read(3);
    block_payload_size = reader.read<uint32_t>(29);

    /*   for(size_t i = 0; i < block_payload_size; ++i) {
           reader.read(8);
       } */

    auto count = core::GenDesc(descriptor_ID) == core::GenDesc::QV
                     ? qv_count
                     : core::getDescriptor(core::GenDesc(descriptor_ID)).subseqs.size();
    payload =
        core::AccessUnitPayload::DescriptorPayload(core::GenDesc(descriptor_ID), count, block_payload_size, reader);

    reader.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

void Block::write(util::BitWriter &writer) const {
    writer.write(0, 1);
    writer.write(descriptor_ID, 7);
    writer.write(0, 3);

    std::stringstream ss;
    util::BitWriter tmp_writer(&ss);
    payload.write(tmp_writer);
    tmp_writer.flush();
    uint64_t bits = tmp_writer.getBitsWritten();

    writer.write(bits / 8, 29);
    writer.writeBypass(&ss);
    writer.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnitPayload::DescriptorPayload &&Block::movePayload() { return std::move(payload); }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Block::getDescriptorID() const { return descriptor_ID; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------