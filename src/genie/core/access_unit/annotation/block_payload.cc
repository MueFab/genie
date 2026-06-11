/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "block_payload.h"
#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace access_unit {
namespace annotation {

BlockPayload::BlockPayload()
    : descriptor_ID(AnnotDesc::GENOTYPE), num_chrs(0), block_payload_size(0), generic_payload{} {}

BlockPayload::BlockPayload(AnnotDesc descriptorID, uint32_t block_payload_size,
                           const std::vector<uint8_t>& generic_payload)
    : descriptor_ID(descriptorID),
      num_chrs(0),
      block_payload_size(block_payload_size),
      generic_payload(generic_payload) {}

BlockPayload::BlockPayload(util::BitReader& reader, AnnotDesc descriptorID, uint8_t numChrs) {
    descriptor_ID = descriptorID;
    num_chrs = numChrs;
    read(reader);
}

BlockPayload::BlockPayload(AnnotDesc descriptorID, uint8_t numChrs, uint32_t block_payload_size,
                           const std::vector<uint8_t>& genericPayload)
    : descriptor_ID(descriptorID),
      num_chrs(numChrs),
      block_payload_size(block_payload_size),
      generic_payload(genericPayload) {}

BlockPayload::BlockPayload(AnnotDesc descriptorID, uint8_t numChrs, uint32_t block_payload_size,
                           std::stringstream& _generic_payload_stream)
    : descriptor_ID(descriptorID), num_chrs(numChrs), block_payload_size(block_payload_size) {
    generic_payload_stream << _generic_payload_stream.rdbuf();
}

void BlockPayload::read(util::BitReader& reader) {
    if (descriptor_ID == AnnotDesc::GENOTYPE) {
    } else if (descriptor_ID == AnnotDesc::LIKELIHOOD) {
    } else if (descriptor_ID == AnnotDesc::CONTACT) {
    } else {
        generic_payload.push_back(static_cast<uint8_t>(reader.ReadBits(8)));
    }
    reader.FlushHeldBits();
}

void BlockPayload::read(util::BitReader& reader, AnnotDesc descriptorID, uint8_t numChrs) {
    descriptor_ID = descriptorID;
    num_chrs = numChrs;
    read(reader);
}

// DEPRECATED: Use write(util::BitWriter&) instead
// void BlockPayload::write(core::Writer& writer) const {
//     if (generic_payload_stream.str().size() > 0)
//       writer.Write(const_cast<std::stringstream*>(&generic_payload_stream));
//     else
//     for (const auto& byte : generic_payload) writer.Write(byte, 8, true);
//     writer.Flush();
// }

void BlockPayload::write(util::BitWriter& writer) const {
  if (generic_payload_stream.str().size() > 0) {
    auto* ss = const_cast<std::stringstream*>(&generic_payload_stream);
    ss->clear();
    ss->seekg(0, std::ios::beg);
    if (writer.IsByteAligned()) {
      writer.WriteAlignedStream(*ss);
    } else {
      char byte;
      while (ss->read(&byte, 1)) {
        writer.WriteBits(static_cast<uint8_t>(byte), 8);
      }
    }
  } else {
    for (const auto& byte : generic_payload) writer.WriteBits(byte, 8);
  }
  writer.FlushBits();
}

size_t BlockPayload::getSize(util::BitWriter& writesize) const {
    write(writesize);
    return writesize.GetTotalBitsWritten();
}

// DEPRECATED: Use getSize(util::BitWriter&) instead
// size_t BlockPayload::getSize(core::Writer& writesize) const {
//     write(writesize);
//     return writesize.GetBitsWritten();
// }

}  // namespace annotation
}  // namespace access_unit
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
