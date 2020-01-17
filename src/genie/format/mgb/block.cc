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
    : reserved(0), descriptor_ID(_descriptor_ID), reserved_2(0), payload(std::move(_payload)) {}

// ---------------------------------------------------------------------------------------------------------------------

Block::Block() : reserved(0), descriptor_ID(0), reserved_2(0), block_payload_size(0), payload(core::GenDesc(0)) {}

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(util::BitReader &reader) {
    reserved = reader.read(1);
    descriptor_ID = reader.read(7);
    reserved_2 = reader.read(3);
    block_payload_size = reader.read(29);

    /*   for(size_t i = 0; i < block_payload_size; ++i) {
           reader.read(8);
       } */

    payload = core::AccessUnitPayload::DescriptorPayload(core::GenDesc(descriptor_ID), block_payload_size, reader);

    reader.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

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