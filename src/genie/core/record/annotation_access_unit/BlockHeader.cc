/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "BlockHeader.h"
#include <algorithm>
#include <string>
#include <utility>
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

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
    descriptor_ID = static_cast<AnnotDesc>(reader.ReadBits(8));
    if (descriptor_ID == AnnotDesc::ATTRIBUTE)
        attribute_ID = static_cast<uint16_t>(reader.ReadBits(16));
    reader.ReadBits(2);
    indexed = static_cast<bool>(reader.ReadBits(1));
    block_payload_size = static_cast<uint32_t>(reader.ReadBits(29));
}

void BlockHeader::write(core::Writer& writer) const {
  writer.Write(static_cast<uint8_t>(descriptor_ID), 8);
    if (descriptor_ID == AnnotDesc::ATTRIBUTE) writer.Write(attribute_ID, 16);
    writer.WriteReserved(2);
    writer.Write(indexed, 1);
    writer.Write(block_payload_size, 29);
}

void BlockHeader::write(util::BitWriter& writer) const {
  writer.WriteBits(static_cast<uint8_t>(descriptor_ID), 8);
  if (descriptor_ID == AnnotDesc::ATTRIBUTE) writer.WriteBits(attribute_ID, 16);
  writer.WriteReserved(2);
  writer.WriteBits(indexed, 1);
  writer.WriteBits(block_payload_size, 29);
}

size_t BlockHeader::getSize(core::Writer& writesize) const {
    write(writesize);
    return writesize.GetBitsWritten();
}
}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
