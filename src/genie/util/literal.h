/**
 * Copyright 2018-2024 The Genie Authors.
 * @file literal.h
 * @brief Header file for user-defined literal operators.
 *
 * This file provides user-defined literal operators for converting
 * unsigned long long values to various fixed-width integer types.
 *
 * @details The user-defined literals allow for easy and explicit
 * conversion of integer literals to specific fixed-width integer types
 * such as std::uint8_t, std::uint16_t, std::uint32_t, and std::uint64_t.
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_LITERAL_H_
#define SRC_GENIE_UTIL_LITERAL_H_

// -----------------------------------------------------------------------------

#include <cstdint>

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Converts an unsigned long long value to std::uint8_t.
 *
 * @param value The value to convert.
 * @return The converted value as std::uint8_t.
 */
constexpr std::uint8_t operator""_u8(
    const unsigned long long value) {  // NOLINT
  return static_cast<std::uint8_t>(value);
}

/**
 * @brief Converts an unsigned long long value to std::uint16_t.
 *
 * @param value The value to convert.
 * @return The converted value as std::uint16_t.
 */
constexpr std::uint16_t operator""_u16(
    const unsigned long long value) {  // NOLINT
  return static_cast<std::uint16_t>(value);
}

/**
 * @brief Converts an unsigned long long value to std::uint32_t.
 *
 * @param value The value to convert.
 * @return The converted value as std::uint32_t.
 */
constexpr std::uint32_t operator""_u32(
    const unsigned long long value) {  // NOLINT
  return static_cast<std::uint32_t>(value);
}

/**
 * @brief Converts an unsigned long long value to std::uint64_t.
 *
 * @param value The value to convert.
 * @return The converted value as std::uint64_t.
 */
constexpr std::uint64_t operator""_u64(
    const unsigned long long value) {  // NOLINT
  return value;
}
}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_LITERAL_H_

// -----------------------------------------------------------------------------
