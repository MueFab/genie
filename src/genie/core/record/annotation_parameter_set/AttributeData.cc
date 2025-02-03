/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <cstring>
#include <sstream>
#include <string>
#include <utility>

#include "genie/core/arrayType.h"
#include "genie/core/record/annotation_parameter_set/AttributeData.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {
namespace annotation_parameter_set {

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

}  // namespace annotation_parameter_set
}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
