/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "BlockPayload.h"
#include <algorithm>
#include <sstream>
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

void BlockPayload::read(util::BitReader& reader) {
    if (descriptor_ID == AnnotDesc::GENOTYPE) {
    } else if (descriptor_ID == AnnotDesc::LIKELIHOOD) {
    } else if (descriptor_ID == AnnotDesc::CONTACT) {
    } else {
        generic_payload.push_back(static_cast<uint8_t>(reader.read_b(8)));
    }
    reader.flush();
}

void BlockPayload::read(util::BitReader& reader, AnnotDesc descriptorID, uint8_t numChrs) {
    descriptor_ID = descriptorID;
    num_chrs = numChrs;
    read(reader);
}

void BlockPayload::write(core::Writer& writer) const {
    for (const auto& byte : generic_payload) writer.write(byte, 8, true);
    writer.flush();
}

size_t BlockPayload::getSize(core::Writer& writesize) const {
    write(writesize);
    return writesize.getBitsWritten();
}

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
