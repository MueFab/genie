/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_LITERAL_H_
#define SRC_GENIE_UTIL_LITERAL_H_

// -----------------------------------------------------------------------------

#include <cstdint>

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 *
 * @param value
 * @return
 */
constexpr std::uint8_t operator""_u8(
    const unsigned long long value) {  // NOLINT
  return static_cast<std::uint8_t>(value);
}

/**
 *
 * @param value
 * @return
 */
constexpr std::uint16_t operator""_u16(
    const unsigned long long value) {  // NOLINT
  return static_cast<std::uint16_t>(value);
}

/**
 *
 * @param value
 * @return
 */
constexpr std::uint32_t operator""_u32(
    const unsigned long long value) {  // NOLINT
  return static_cast<std::uint32_t>(value);
}

/**
 *
 * @param value
 * @return
 */
constexpr std::uint64_t operator""_u64(
    const unsigned long long value) {  // NOLINT
  return value;
}
}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_LITERAL_H_

// -----------------------------------------------------------------------------
