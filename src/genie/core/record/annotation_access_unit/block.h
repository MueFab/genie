/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCK_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCK_H_

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
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/core/record/annotation_parameter_set/GenotypeParameters.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"

#include "BlockHeader.h"
#include "BlockPayload.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

class BlockData {
 public:
    BlockData(annotation_parameter_set::DescriptorID descriptorID, std::stringstream data)
        : descriptorID(descriptorID), data(data), thisIsAttribute(false), attributeID(0) {}

    BlockData(annotation_parameter_set::DescriptorID descriptorID, uint16_t attributeID, std::stringstream& data)
        : descriptorID(descriptorID), attributeID(attributeID), data(data), thisIsAttribute(true) {}

    annotation_parameter_set::DescriptorID getDescriptorID() const { return descriptorID; }
    uint16_t getAttributeID() const { return attributeID; }
    bool isAttribute() const { return thisIsAttribute; }
    std::stringstream& getData() { return data; }

 private:
    annotation_parameter_set::DescriptorID descriptorID;
    uint16_t attributeID;
    bool thisIsAttribute;
    std::stringstream& data;
};


class BlockVectorData {
 public:
    BlockVectorData(annotation_parameter_set::DescriptorID descriptorID, std::vector<uint8_t> data)
        : descriptorID(descriptorID), data(data), thisIsAttribute(false), attributeID(0) {}

    BlockVectorData(annotation_parameter_set::DescriptorID descriptorID, uint16_t attributeID, std::vector<uint8_t>& data)
        : descriptorID(descriptorID), attributeID(attributeID), data(data), thisIsAttribute(true) {}

    annotation_parameter_set::DescriptorID getDescriptorID() const { return descriptorID; }
    uint16_t getAttributeID() const { return attributeID; }
    bool isAttribute() const { return thisIsAttribute; }
    std::vector<uint8_t>& getData() { return data; }

 private:
    annotation_parameter_set::DescriptorID descriptorID;
    uint16_t attributeID;
    bool thisIsAttribute;
    std::vector<uint8_t>& data;
};



class Block {
 private:
    BlockHeader block_header;
    BlockPayload block_payload;
    uint8_t numChrs;

 public:
    Block();
    Block(BlockHeader block_header, BlockPayload block_payload, uint8_t numChrs);
    Block(util::BitReader& reader, uint8_t numChrs);

    Block(const Block& b) {
        block_header = b.block_header;
        block_payload = b.block_payload;
        numChrs = b.numChrs;
    }

    Block& operator=(const Block& b) {
        block_header = b.block_header;
        block_payload = b.block_payload;
        numChrs = b.numChrs;
        return *this;
    }

    void read(util::BitReader& reader);
    void read(util::BitReader& reader, uint8_t numChrs);
    void write(core::Writer& writer) ;
    void set(BlockVectorData blockData);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_access_unit
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
