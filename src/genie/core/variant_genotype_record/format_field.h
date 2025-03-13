/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef GENIE_CORE_RECORD_FORMAT_FIELD_FORMAT_FIELD_H
#define GENIE_CORE_RECORD_FORMAT_FIELD_FORMAT_FIELD_H

#include <boost/variant/variant.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Class representing a format field in a variant genotype record
 *
 * Stores information about format data, including name, type, and values for each sample.
 */
class FormatField {
 private:
  std::string format_name_;      //!< @brief Name of the format field (e.g., GT, DP, GQ)
  DataType format_type_;         //!< @brief Data type of the format field
  // sample_count, array_len
  std::vector<std::vector<DynamicDataType>> format_value_;  //!< @brief Values for each sample and array index

 public:
  /**
   * @brief Gets the length of the format name
   * @return The length of the format name
   */
  [[nodiscard]] uint8_t GetFormatLen() const;

  /**
   * @brief Gets the format field name
   * @return Constant reference to the format name
   */
  [[nodiscard]] const std::string& GetFormatName() const;

  /**
   * @brief Gets the data type of the format field
   * @return The format field data type
   */
  [[nodiscard]] DataType GetFormatType() const;

  /**
   * @brief Gets the number of samples this format field applies to
   * @return The sample count
   */
  [[nodiscard]] uint32_t GetSampleCount() const;

  /**
   * @brief Gets the array length of the format field
   * @return The array length
   */
  [[nodiscard]] uint8_t GetFormatArrayLen() const;

  /**
   * @brief Gets the format field values for all samples
   * @return Constant reference to the format values
   */
  [[nodiscard]] const std::vector<std::vector<DynamicDataType>>& GetFormatValue() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_CORE_RECORD_FORMAT_FIELD_FORMAT_FIELD_H

// ---------------------------------------------------------------------------------------------------------------------