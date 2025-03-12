/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCKHEADER_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCKHEADER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

#include "genie/core/constants.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/core/writer.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

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
    void write(core::Writer& writer) const;
    void write(util::BitWriter& writer) const;

    AnnotDesc getDescriptorID() const { return descriptor_ID; }
    uint16_t getAttributeID() const { return attribute_ID; }
    bool isIndexed() const { return indexed; }
    uint32_t getBlockPayloadSize() const { return block_payload_size; }
    size_t getSize(core::Writer& writesize) const;
};



// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCKHEADER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
