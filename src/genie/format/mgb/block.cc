/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgb/block.h"
#include <memory>
#include <sstream>
#include <utility>
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mgb {

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(uint8_t _descriptor_ID, core::AccessUnit::Descriptor &&_payload)
    : descriptor_ID(_descriptor_ID), payload(std::move(_payload)) {
    count = static_cast<uint8_t>(boost::get<core::AccessUnit::Descriptor>(payload).getSize());
}

// ---------------------------------------------------------------------------------------------------------------------

Block::Block() : descriptor_ID(0), block_payload_size(0), payload(core::AccessUnit::Descriptor(core::GenDesc(0))) {}

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(size_t qv_count, util::BitReader &reader) : payload(core::AccessUnit::Descriptor()) {
    reader.read_b(1);
    descriptor_ID = reader.read<uint8_t>(7);
    reader.read_b(3);
    block_payload_size = reader.read<uint32_t>(29);

    /*   for(size_t i = 0; i < block_payload_size; ++i) {
           reader.read(8);
       } */

    count = static_cast<uint8_t>(core::GenDesc(descriptor_ID) == core::GenDesc::kQv
                                     ? qv_count
                                     : core::GetDescriptor(core::GenDesc(descriptor_ID)).sub_seqs.size());
    // payload = core::AccessUnit::Descriptor(core::GenDesc(descriptor_ID), count, block_payload_size, reader);
    payload = core::Payload(reader, block_payload_size);

    reader.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

void Block::write(util::BitWriter &writer) const {
    writer.write(0, 1);
    writer.write(descriptor_ID, 7);
    writer.write(0, 3);

    if (payload.type() == typeid(genie::core::Payload)) {
        writer.write(boost::get<core::Payload>(payload).getPayloadSize(), 29);
        boost::get<core::Payload>(payload).write(writer);
    } else {
        writer.write(boost::get<core::AccessUnit::Descriptor>(payload).getWrittenSize(), 29);
        boost::get<core::AccessUnit::Descriptor>(payload).write(writer);
    }
    writer.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

core::AccessUnit::Descriptor &&Block::movePayload() {
    return std::move(boost::get<core::AccessUnit::Descriptor>(payload));
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Block::getDescriptorID() const { return descriptor_ID; }

// ---------------------------------------------------------------------------------------------------------------------

size_t Block::getWrittenSize() const {
    if (payload.type() == typeid(genie::core::Payload)) {
        return boost::get<core::Payload>(payload).getPayloadSize() + sizeof(uint32_t) + sizeof(uint8_t);
    } else {
        return boost::get<core::AccessUnit::Descriptor>(payload).getWrittenSize() + sizeof(uint32_t) + sizeof(uint8_t);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

bool Block::isLoaded() const {
    return (payload.type() == typeid(genie::core::Payload) &&
            boost::get<genie::core::Payload>(payload).isPayloadLoaded()) ||
           payload.type() == typeid(genie::core::AccessUnit::Descriptor);
}

// ---------------------------------------------------------------------------------------------------------------------

bool Block::isParsed() const { return payload.type() == typeid(genie::core::AccessUnit::Descriptor); }

// ---------------------------------------------------------------------------------------------------------------------

void Block::load() {
    if (isLoaded()) {
        return;
    }
    boost::get<genie::core::Payload>(payload).loadPayload();
}

// ---------------------------------------------------------------------------------------------------------------------

void Block::unload() {
    if (!isLoaded()) {
        return;
    }
    if (!isParsed()) {
        boost::get<genie::core::Payload>(payload).unloadPayload();
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Block::parse() {
    if (!isLoaded()) {
        load();
    }
    std::stringstream ss;

    ss.write(static_cast<const char *>(boost::get<genie::core::Payload>(payload).getPayload().getData()),
             static_cast<std::streamsize>(boost::get<genie::core::Payload>(payload).getPayload().getRawSize()));

    util::BitReader reader(ss);

    payload = core::AccessUnit::Descriptor(core::GenDesc(descriptor_ID), count, block_payload_size, reader);
}

// ---------------------------------------------------------------------------------------------------------------------

void Block::pack() {
    if (!isParsed()) {
        return;
    }
    std::stringstream ss;
    util::BitWriter writer(&ss);

    boost::get<core::AccessUnit::Descriptor>(payload).write(writer);

    util::BitReader reader(ss);

    payload = core::Payload(reader, ss.str().length());
}

// ---------------------------------------------------------------------------------------------------------------------

Block::Block(genie::core::GenDesc _descriptor_ID, core::Payload _payload)
    : descriptor_ID(uint8_t(_descriptor_ID)),
      block_payload_size(static_cast<uint32_t>(_payload.getPayloadSize())),
      count(0),
      payload(std::move(_payload)) {}

// ---------------------------------------------------------------------------------------------------------------------

core::Payload Block::movePayloadUnparsed() { return std::move(boost::get<core::Payload>(payload)); }

// ---------------------------------------------------------------------------------------------------------------------

core::Payload &Block::getPayloadUnparsed() { return boost::get<core::Payload>(payload); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mgb
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
