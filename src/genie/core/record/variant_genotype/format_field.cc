/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "format_field.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

// ---------------------------------------------------------------------------------------------------------------------

uint8_t FormatField::GetFormatLen() const {
  return static_cast<uint8_t>(format_name_.size());
}

// ---------------------------------------------------------------------------------------------------------------------

const std::string& FormatField::GetFormatName() const {
  return format_name_;
}

// ---------------------------------------------------------------------------------------------------------------------

DataType FormatField::GetFormatType() const {
  return format_type_;
}

// ---------------------------------------------------------------------------------------------------------------------

uint32_t FormatField::GetSampleCount() const {
  return static_cast<uint32_t>(format_value_.size());
}

// ---------------------------------------------------------------------------------------------------------------------

uint8_t FormatField::GetFormatArrayLen() const {
  return static_cast<uint8_t>(format_value_[0].size());
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<std::vector<DynamicDataType>>& FormatField::GetFormatValue() const {
  return format_value_;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------