/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_SAM_SAM_TAG_H_
#define SRC_GENIE_FORMAT_SAM_SAM_TAG_H_

// -----------------------------------------------------------------------------

#include <string>

// -----------------------------------------------------------------------------

#include "genie/core/record/tag_record.h"
#include "genie/util/hex_string.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam {

/**
 * @brief Tag type visitor for SAM format.
 *
 * This struct is used to determine the SAM type code for a given data type.
 * It provides a mechanism to convert various data types into their
 * corresponding SAM representation.
 */
struct TagSamType {
  /**
   * @brief Overloaded function call operator to get the SAM type code.
   *
   * This operator is specialized for different data types to return the
   * corresponding SAM type code as a string.
   *
   * @tparam T The data type to be converted.
   * @return The SAM type code as a string.
   */
  template <typename T>
  std::string operator()(const T&) const {
    if constexpr (IsStdVector<T>::value) {
      return "B:" + sam_code<typename T::value_type>();
    } else {
      return sam_code<T>();
    }
  }

 private:
  /**
   * @brief Helper struct to check if a type is a std::vector.
   *
   * This struct is used to determine if a given type is a specialization of
   * std::vector.
   */
  template <typename>
  struct IsStdVector : std::false_type {};

  /**
   * @brief Specialization of IsStdVector for std::vector.
   *
   * This specialization checks if a given type is a std::vector.
   */
  template <typename T, typename A>
  struct IsStdVector<std::vector<T, A>> : std::true_type {};

  /**
   * @brief Helper function to get the SAM type code for a given data type.
   *
   * This function is specialized for different data types to return the
   * corresponding SAM type code as a string.
   *
   * @tparam T The data type to be converted.
   * @return The SAM type code as a string.
   */
  template <typename T>
  static std::string sam_code() {
    if constexpr (std::is_same_v<T, int32_t>)
      return "i";
    else if constexpr (std::is_same_v<T, uint8_t>)
      return "C";
    else if constexpr (std::is_same_v<T, int8_t>)
      return "c";
    else if constexpr (std::is_same_v<T, uint16_t>)
      return "S";
    else if constexpr (std::is_same_v<T, int16_t>)
      return "s";
    else if constexpr (std::is_same_v<T, uint32_t>)
      return "I";
    else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
      return "f";
    else if constexpr (std::is_same_v<T, std::string>)
      return "Z";
    else if constexpr (std::is_same_v<T, util::HexString>)
      return "H";
    else
      static_assert(!sizeof(T), "Unsupported type for SAM tag");
    return "";
  }
};

// -----------------------------------------------------------------------------

std::string SerializeTagRecord(const core::record::TagRecord& record);

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_SAM_SAM_TAG_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
