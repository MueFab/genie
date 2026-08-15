/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "BlockPayload.h"

#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

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
    Read(reader);
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

void BlockPayload::Read(util::BitReader& reader) {
    if (descriptor_ID == AnnotDesc::GENOTYPE) {
    } else if (descriptor_ID == AnnotDesc::LIKELIHOOD) {
    } else if (descriptor_ID == AnnotDesc::CONTACT) {
    } else {
        generic_payload.push_back(static_cast<uint8_t>(reader.ReadBits(8)));
    }
    reader.FlushHeldBits();
}

void BlockPayload::Read(util::BitReader& reader, AnnotDesc descriptorID, uint8_t numChrs) {
    descriptor_ID = descriptorID;
    num_chrs = numChrs;
    Read(reader);
}

void BlockPayload::Write(util::BitWriter& writer) const {
    if (generic_payload_stream.str().size() > 0) {
        for (char c : generic_payload_stream.str()) {
            writer.WriteBits(static_cast<uint8_t>(c), 8);
        }
    } else {
        for (const auto& byte : generic_payload) writer.WriteBits(byte, 8);
    }
    writer.FlushBits();
}

size_t BlockPayload::GetSize(util::BitWriter& writesize) const {
    Write(writesize);
    return writesize.GetTotalBitsWritten();
}


}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
