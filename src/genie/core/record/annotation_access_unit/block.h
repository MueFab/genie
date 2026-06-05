/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCK_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_ACCESS_UNIT_BLOCK_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <sstream>
#include <vector>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

#include "BlockHeader.h"
#include "BlockPayload.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_access_unit {

class BlockData {
 public:
    BlockData(AnnotDesc _descriptorID, std::stringstream _data)
        : descriptorID(_descriptorID), attributeID(0), thisIsAttribute(false), data(_data) {}

    BlockData(AnnotDesc _descriptorID, uint16_t _attributeID, std::stringstream& _data)
        : descriptorID(_descriptorID), attributeID(_attributeID), thisIsAttribute(true), data(_data) {}

    AnnotDesc getDescriptorID() const { return descriptorID; }
    uint16_t getAttributeID() const { return attributeID; }
    bool isAttribute() const { return thisIsAttribute; }
    size_t getDataSize() const { return data.str().size(); }
    std::stringstream& getData() { return data; }

 private:
    AnnotDesc descriptorID;
    uint16_t attributeID;
    bool thisIsAttribute;
    std::stringstream& data;
};

class BlockVectorData {
 public:
    BlockVectorData(AnnotDesc _descriptorID, std::vector<uint8_t>& _data)
        : descriptorID(_descriptorID), attributeID(0), thisIsAttribute(false), data(_data) {}

    BlockVectorData(AnnotDesc _descriptorID, uint16_t _attributeID, std::vector<uint8_t>& _data)
        : descriptorID(_descriptorID), attributeID(_attributeID), thisIsAttribute(true), data(_data) {}

    AnnotDesc getDescriptorID() const { return descriptorID; }
    uint16_t getAttributeID() const { return attributeID; }
    bool isAttribute() const { return thisIsAttribute; }
    size_t getDataSize() const { return data.size(); }
    std::vector<uint8_t>& getData() { return data; }

 private:
    AnnotDesc descriptorID;
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

    void Read(util::BitReader& reader);
    void Read(util::BitReader& reader, uint8_t numChrs);
    void Write(util::BitWriter& writer) const;

    void set(BlockVectorData blockData);
    void set(BlockData& blockData);
    size_t GetSize(util::BitWriter& writesize) const;
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
