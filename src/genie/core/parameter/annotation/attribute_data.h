/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_ANNOTATION_ATTRIBUTE_DATA_H_
#define SRC_GENIE_CORE_PARAMETER_ANNOTATION_ATTRIBUTE_DATA_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>

#include "genie/core/constants.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::parameter::annotation {

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

}  // namespace genie::core::parameter::annotation

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_ANNOTATION_ATTRIBUTE_DATA_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
