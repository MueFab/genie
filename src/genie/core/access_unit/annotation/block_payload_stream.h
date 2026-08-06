/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_BLOCK_PAYLOAD_STREAM_H_
#define SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_BLOCK_PAYLOAD_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <sstream>

#include "genie/core/parameter/annotation/descriptor_configuration.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::access_unit::annotation {

//----------------------------------------------------------------------------------//

class BlockPayloadStream {
 private:
    genie::core::AnnotDesc descriptor_ID;
    uint32_t block_payload_size;
    std::stringstream generic_payload;

 public:
    BlockPayloadStream();

    BlockPayloadStream(genie::core::AnnotDesc _descriptorID,
                       uint32_t _block_payload_size, const std::stringstream& _generic_payload);

    BlockPayloadStream(const BlockPayloadStream& bp);
    BlockPayloadStream& operator=(const BlockPayloadStream& bp);

    std::stringstream& getPayload();

    void write(util::BitWriter& writer);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::access_unit::annotation

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_BLOCK_PAYLOAD_STREAM_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
