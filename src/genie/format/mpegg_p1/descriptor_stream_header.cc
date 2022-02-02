/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/descriptor_stream_header.h"
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

DSProtection::DSProtection(std::string _DSProtectionValue) : DSProtectionValue(std::move(_DSProtectionValue)) {}

// ---------------------------------------------------------------------------------------------------------------------

DSProtection::DSProtection(genie::util::BitReader& reader) {
    uint64_t length = reader.readBypassBE<uint64_t>();
    DSProtectionValue.resize(length);
    reader.readBypass(DSProtectionValue);
}


// ---------------------------------------------------------------------------------------------------------------------

const std::string& DSProtection::getProtectionValue() const { return DSProtectionValue; }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& DSProtection::getKey() const {
    static const std::string key = "dspr";
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

void DSProtection::box_write(genie::util::BitWriter& bitWriter) const {
    bitWriter.writeBypass(DSProtectionValue.data(), DSProtectionValue.size());
}

// ---------------------------------------------------------------------------------------------------------------------

bool DSProtection::operator==(const GenInfo& info) const {
    if (!GenInfo::operator==(info)) {
        return false;
    }
    const auto& other = dynamic_cast<const DSProtection&>(info);
    return DSProtectionValue == other.DSProtectionValue;
}

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
    : DescriptorStreamHeader(0, genie::core::GenDesc(0), core::record::ClassType::NONE, 0) {}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStreamHeader::DescriptorStreamHeader(bool _reserved, genie::core::GenDesc _genDesc,
                                               core::record::ClassType _class_id, uint32_t _num_blocks)
    : reserved(_reserved), descriptor_id(_genDesc), class_id(_class_id), num_blocks(_num_blocks) {}

// ---------------------------------------------------------------------------------------------------------------------

void DescriptorStreamHeader::box_write(genie::util::BitWriter& bitWriter) const {
    bitWriter.write(reserved, 1);
    bitWriter.write(static_cast<uint8_t>(descriptor_id), 7);
    bitWriter.write(static_cast<uint8_t>(class_id), 4);
    bitWriter.write(num_blocks, 32);
    bitWriter.flush();
}

// ---------------------------------------------------------------------------------------------------------------------

DescriptorStreamHeader::DescriptorStreamHeader(genie::util::BitReader& reader) {
    reader.readBypassBE<uint64_t>();
    reserved = reader.read<bool>(1);
    descriptor_id = reader.read<genie::core::GenDesc>(7);
    class_id = reader.read<core::record::ClassType>(4);
    num_blocks = reader.read<uint32_t>(32);
    reader.flush();
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

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
