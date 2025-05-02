/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCKPAYLOAD_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCKPAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
// #include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/core/writer.h"
// #include "genie/genotype/genotype_parameters.h"
#include "genie/util/bit_reader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

class BlockPayload {
 private:
    AnnotDesc descriptor_ID;
    uint8_t num_chrs;
    uint32_t block_payload_size;
    std::vector<uint8_t> generic_payload;
    std::stringstream generic_payload_stream;

 public:
    BlockPayload();
    BlockPayload(util::BitReader& reader, AnnotDesc descriptorID, uint8_t numChrs);

    BlockPayload(AnnotDesc descriptorID, uint32_t block_payload_size, const std::vector<uint8_t>& generic_payload);

    BlockPayload(AnnotDesc descriptorID, uint8_t numChrs, uint32_t block_payload_size,
                 const std::vector<uint8_t>& generic_payload);

    BlockPayload(AnnotDesc descriptorID, uint8_t numChrs, uint32_t block_payload_size,
        std::stringstream& generic_payload);

    void read(util::BitReader& reader);
    void read(util::BitReader& reader, AnnotDesc descriptorID, uint8_t numChrs);
    void write(core::Writer& writer) const;
    void write(util::BitWriter& writer) const;
    size_t getSize(core::Writer& writesize) const;


    BlockPayload& operator=(const BlockPayload& other) {
        descriptor_ID = other.descriptor_ID;
        num_chrs = other.num_chrs;
        block_payload_size = other.block_payload_size;
        generic_payload = other.generic_payload;
        generic_payload_stream << other.generic_payload_stream.rdbuf();
        return *this;
    }

    BlockPayload(BlockPayload& other) {
        descriptor_ID = other.descriptor_ID;
        num_chrs = other.num_chrs;
        block_payload_size = other.block_payload_size;
        generic_payload = other.generic_payload;
        generic_payload_stream << other.generic_payload_stream.rdbuf();
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCKPAYLOAD_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
