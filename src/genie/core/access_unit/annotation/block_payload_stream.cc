/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include "block_payload.h"

#include "genie/core/access_unit/annotation/block_payload_stream.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace access_unit {
namespace annotation {

BlockPayloadStream::BlockPayloadStream()
    : descriptor_ID(genie::core::AnnotDesc::GENOTYPE),
      block_payload_size(0),
      generic_payload{} {}

BlockPayloadStream::BlockPayloadStream(genie::core::AnnotDesc _descriptorID,
                                       uint32_t _block_payload_size, const std::stringstream& _generic_payload)
    : descriptor_ID(_descriptorID),
      block_payload_size(_block_payload_size),
      generic_payload() {

    generic_payload << _generic_payload.rdbuf();
}

BlockPayloadStream::BlockPayloadStream(const BlockPayloadStream& bp) {
    generic_payload << bp.generic_payload.str();
//    generic_payload << bp.generic_payload.rdbuf();
    descriptor_ID = bp.descriptor_ID;
    block_payload_size = bp.block_payload_size;
}

std::stringstream& BlockPayloadStream::getPayload() { return generic_payload; }

void BlockPayloadStream::write(util::BitWriter& writer) {
    writer.Write(&generic_payload);
    writer.FlushBits();
}

BlockPayloadStream& BlockPayloadStream::operator=(const BlockPayloadStream& bp) {
    generic_payload << bp.generic_payload.str();
//    generic_payload << bp.generic_payload.rdbuf();
    descriptor_ID = bp.descriptor_ID;
    block_payload_size = bp.block_payload_size;
    return *this;
}

}  // namespace annotation
}  // namespace access_unit
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
