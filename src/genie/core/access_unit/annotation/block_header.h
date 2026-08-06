/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_BLOCK_HEADER_H_
#define SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_BLOCK_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::access_unit::annotation {

class BlockHeader {
 private:
    bool attribute_contiguity;
    AnnotDesc descriptor_ID;
    uint16_t attribute_ID;
    bool indexed;
    uint32_t block_payload_size;

 public:
    BlockHeader();
    BlockHeader(bool attribute_contiguity, AnnotDesc descriptor_ID,
                uint16_t attribute_ID, bool indexed, uint32_t block_payload_size);

    void read(genie::util::BitReader& reader);
    void write(util::BitWriter& writer) const;

    AnnotDesc getDescriptorID() const { return descriptor_ID; }
    uint16_t getAttributeID() const { return attribute_ID; }
    bool isIndexed() const { return indexed; }
    uint32_t getBlockPayloadSize() const { return block_payload_size; }
    size_t getSize(util::BitWriter& writer) const;
};



// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::access_unit::annotation

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_ACCESS_UNIT_ANNOTATION_BLOCK_HEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
