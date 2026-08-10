/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/parameter/annotation/attribute_data.h"

#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::parameter::annotation {

AttributeData::AttributeData()
    : attributeID(0),
      attributeNameLength(0),
      attributeName(""),
      attributeType(genie::core::DataType::STRING),
      attributeArrayDims(0) {}

AttributeData::AttributeData(uint8_t length, std::string name, uint16_t attributeID)
    : attributeID(attributeID),
      attributeNameLength(length),
      attributeName(name),
      attributeType(genie::core::DataType::STRING),
      attributeArrayDims(0) {}

AttributeData::AttributeData(uint8_t length, std::string name, genie::core::DataType type, uint8_t arrayLength,
                             uint16_t attributeID)
    : attributeID(attributeID),
      attributeNameLength(length),
      attributeName(name),
      attributeType(type),
      attributeArrayDims(arrayLength) {}

AttributeData& AttributeData::operator=(const AttributeData& other) {
    attributeNameLength = other.attributeNameLength;
    attributeName = other.attributeName;
    attributeType = other.attributeType;
    attributeArrayDims = other.attributeArrayDims;
    attributeID = other.attributeID;
    return *this;
}

AttributeData::AttributeData(const AttributeData& other) {
    attributeNameLength = other.attributeNameLength;
    attributeName = other.attributeName;
    attributeType = other.attributeType;
    attributeArrayDims = other.attributeArrayDims;
    attributeID = other.attributeID;
}

}  // namespace genie::core::parameter::annotation

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
