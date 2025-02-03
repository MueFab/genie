/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_ATTRIBUTEDATA_H_
#define SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_ATTRIBUTEDATA_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/arrayType.h"
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

class AttributeData {
 public:
    AttributeData();
    AttributeData(uint8_t length, std::string name, uint16_t attributeID);
    AttributeData(uint8_t length, std::string name, genie::core::DataType type, uint8_t arrayLength,
                  uint16_t attributeID);

    AttributeData& operator=(const AttributeData& other);

    AttributeData(const AttributeData& other);

    void setAttributeType(genie::core::DataType value) { attributeType = value; }
    void setArrayLength(uint8_t value) { attributeArrayDims = value; }

    uint8_t getAttributeNameLength() const { return attributeNameLength; }
    std::string getAttributeName() const { return attributeName; }
    genie::core::DataType getAttributeType() const { return attributeType; }
    uint8_t getArrayLength() const { return attributeArrayDims; }
    uint16_t getAttributeID() const { return attributeID; }

 private:
    uint16_t attributeID;
    uint8_t attributeNameLength;
    std::string attributeName;
    genie::core::DataType attributeType;
    uint8_t attributeArrayDims;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ANNOTATION_PARAMETER_SET_ATTRIBUTEDATA_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
