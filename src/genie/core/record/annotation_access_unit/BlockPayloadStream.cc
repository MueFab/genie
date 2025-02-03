/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <sstream>
#include <string>
#include <utility>
#include "BlockPayload.h"

#include "genie/core/record/annotation_access_unit/BlockPayloadStream.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

BlockPayloadStream::BlockPayloadStream()
    : descriptor_ID(genie::core::record::annotation_parameter_set::DescriptorID::GENOTYPE),
      block_payload_size(0),
      generic_payload{} {}

BlockPayloadStream::BlockPayloadStream(genie::core::record::annotation_parameter_set::DescriptorID _descriptorID,
                                       uint32_t _block_payload_size, const std::stringstream& _generic_payload)
    : descriptor_ID(_descriptorID),
      block_payload_size(_block_payload_size),
      generic_payload(){

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
    writer.write(&generic_payload);
    writer.flush();
}

BlockPayloadStream& BlockPayloadStream::operator=(const BlockPayloadStream& bp) {
    generic_payload << bp.generic_payload.str();
//    generic_payload << bp.generic_payload.rdbuf();
    descriptor_ID = bp.descriptor_ID;
    block_payload_size = bp.block_payload_size;
//    return *this;
}

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
