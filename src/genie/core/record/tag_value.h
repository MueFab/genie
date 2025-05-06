/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_TAG_VALUE_H_
#define SRC_GENIE_CORE_RECORD_TAG_VALUE_H_

// -----------------------------------------------------------------------------

#include <string>
#include <variant>
#include <vector>

// -----------------------------------------------------------------------------

#include "genie/util/hex_string.h"

// -----------------------------------------------------------------------------

namespace genie::core::record {

/**
 * @brief Type alias for tag data, which can be of various types.
 */
using TagData = std::variant<
    int32_t, std::string, uint8_t, int8_t, uint16_t, int16_t, uint32_t, float,
    double, util::HexString, std::vector<int32_t>, std::vector<uint8_t>,
    std::vector<int8_t>, std::vector<uint16_t>, std::vector<int16_t>,
    std::vector<uint32_t>, std::vector<float>, std::vector<double>>;

struct TagDataLengthVisitor {
  template <typename T>
  std::enable_if_t<std::is_arithmetic_v<T>, size_t> operator()(const T&) const {
    return 1;
  }
  template <typename T>
  size_t operator()(const std::vector<T>& v) const {
    return v.size();
  }

  size_t operator()(const std::string& v) const { return v.length(); }

  size_t operator()(const util::HexString& v) const { return v.str().length(); }
};

// -----------------------------------------------------------------------------

enum class TagDataType : uint8_t {
  kInt32 = 0,
  kChar = 1,
  kUInt8 = 2,
  kInt8 = 3,
  kUInt16 = 4,
  kInt16 = 5,
  kUInt32 = 6,
  kHexChar = 7,
  kFloat = 8,
  kDouble = 9
};

struct TagDataTypeVisitor {
  template <typename T>
  TagDataType operator()(const T&) const {
    return type_index<T>();
  }

  template <typename T>
  TagDataType operator()(const std::vector<T>&) const {
    return type_index<T>();
  }

 private:
  template <typename T>
  static constexpr TagDataType type_index() {
    if constexpr (std::is_same_v<T, int32_t>)
      return TagDataType::kInt32;
    else if constexpr (std::is_same_v<T, std::string>)
      return TagDataType::kChar;
    else if constexpr (std::is_same_v<T, uint8_t>)
      return TagDataType::kUInt8;
    else if constexpr (std::is_same_v<T, int8_t>)
      return TagDataType::kInt8;
    else if constexpr (std::is_same_v<T, uint16_t>)
      return TagDataType::kUInt16;
    else if constexpr (std::is_same_v<T, int16_t>)
      return TagDataType::kInt16;
    else if constexpr (std::is_same_v<T, uint32_t>)
      return TagDataType::kUInt32;
    else if constexpr (std::is_same_v<T, util::HexString>)
      return TagDataType::kHexChar;
    else if constexpr (std::is_same_v<T, float>)
      return TagDataType::kFloat;
    else if constexpr (std::is_same_v<T, double>)
      return TagDataType::kDouble;
    else
      static_assert(!sizeof(T), "Unsupported type in TagTypeIndex");
    return TagDataType::kInt32;
  }
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_TAG_VALUE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
