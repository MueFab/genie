/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "BlockHeader.h"
#include <algorithm>
#include <string>
#include <utility>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

BlockHeader::BlockHeader()
    : attribute_contiguity(false),
      descriptor_ID(AnnotDesc::ATTRIBUTE),
      attribute_ID(0),
      indexed(false),
      block_payload_size(0) {}

BlockHeader::BlockHeader(bool attribute_contiguity,
                         AnnotDesc descriptor_ID,
                         uint16_t attribute_ID, bool indexed, uint32_t block_payload_size)
    : attribute_contiguity(attribute_contiguity),
      descriptor_ID(descriptor_ID),
      attribute_ID(attribute_ID),
      indexed(indexed),
      block_payload_size(block_payload_size) {}

void BlockHeader::read(genie::util::BitReader& reader) {
    descriptor_ID = static_cast<AnnotDesc>(reader.read_b(8));
    if (descriptor_ID == AnnotDesc::ATTRIBUTE)
        attribute_ID = static_cast<uint16_t>(reader.read_b(16));
    reader.read_b(2);
    indexed = static_cast<bool>(reader.read_b(1));
    block_payload_size = static_cast<uint32_t>(reader.read_b(29));
}

void BlockHeader::write(core::Writer& writer) const {
    writer.write(static_cast<uint8_t>(descriptor_ID), 8);
    if (descriptor_ID == AnnotDesc::ATTRIBUTE)
        writer.write(attribute_ID, 16);
    writer.write_reserved(2);
    writer.write(indexed, 1);
    writer.write(block_payload_size, 29);
}

size_t BlockHeader::getSize(core::Writer& writesize) const {
    write(writesize);
    return writesize.getBitsWritten();
}
}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
