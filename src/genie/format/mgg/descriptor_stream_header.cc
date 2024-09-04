/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/descriptor_stream_header.h"
#include <utility>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg {

// ---------------------------------------------------------------------------------------------------------------------

bool DescriptorStreamHeader::getReserved() const { return reserved; }

// ---------------------------------------------------------------------------------------------------------------------

genie::core::GenDesc DescriptorStreamHeader::getDescriptorID() const { return descriptor_id; }

// ---------------------------------------------------------------------------------------------------------------------

core::record::ClassType DescriptorStreamHeader::getClassType() const { return class_id; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t DescriptorStreamHeader::getNumBlocks() const { return num_blocks; }

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorStreamHeader::addBlock() { num_blocks++; }

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStreamHeader::DescriptorStreamHeader()
    : DescriptorStreamHeader(false, genie::core::GenDesc(0), core::record::ClassType::NONE, 0) {}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStreamHeader::DescriptorStreamHeader(bool _reserved, genie::core::GenDesc _genDesc,
                                               core::record::ClassType _class_id, uint32_t _num_blocks)
    : reserved(_reserved), descriptor_id(_genDesc), class_id(_class_id), num_blocks(_num_blocks) {}

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorStreamHeader::box_write(genie::util::BitWriter& bitWriter) const {
    bitWriter.writeBits(reserved, 1);
    bitWriter.writeBits(static_cast<uint8_t>(descriptor_id), 7);
    bitWriter.writeBits(static_cast<uint8_t>(class_id), 4);
    bitWriter.writeBits(num_blocks, 32);
    bitWriter.flushBits();
}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStreamHeader::DescriptorStreamHeader(genie::util::BitReader& reader) {
    auto start_pos = reader.getStreamPosition() - 4;
    auto length = reader.readAlignedInt<uint64_t>();
    reserved = reader.read<bool>(1);
    descriptor_id = reader.read<genie::core::GenDesc>(7);
    class_id = reader.read<core::record::ClassType>(4);
    num_blocks = reader.read<uint32_t>(32);
    reader.flushHeldBits();
    UTILS_DIE_IF(start_pos + length != uint64_t(reader.getStreamPosition()), "Invalid length");
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DescriptorStreamHeader::getKey() const {
    static const std::string key = "dshd";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

bool DescriptorStreamHeader::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const DescriptorStreamHeader&>(info);
    return reserved == other.reserved && descriptor_id == other.descriptor_id && class_id == other.class_id &&
           num_blocks == other.num_blocks;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
