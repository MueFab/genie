/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "format_field.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

uint8_t FormatField::getFormatLen() const { return format_name.size(); }

// ---------------------------------------------------------------------------------------------------------------------

const std::string& FormatField::getFormatName() const { return format_name; }

// ---------------------------------------------------------------------------------------------------------------------

DataType FormatField::getFormatType() const { return format_type; }

// ---------------------------------------------------------------------------------------------------------------------

uint32_t FormatField::getSampleCount() const { return format_value.size(); }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t FormatField::getFormatArrayLen() const { return format_value[0].size(); }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::vector<DynamicDataType>>& FormatField::getFormatValue() const { return format_value; }

// ---------------------------------------------------------------------------------------------------------------------


}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------