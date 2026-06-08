/**
 * Copyright 2018-2024 The Genie Authors.
 * @file endianness.impl.h
 * @brief Implementation of endianness utility functions for byte swapping.
 *
 * This file contains the implementation of utility functions for swapping
 * the endianness of data types. It includes a template function to swap
 * the byte order of a given data type.
 *
 * @details The SwapEndianness function template swaps the byte order of
 * the provided data type, ensuring compatibility with different endianness
 * formats. It uses static assertions to ensure the correct byte Size and
 * performs the byte swapping in place.
 *
 * @tparam T The type of the data whose endianness is being swapped.
 * @tparam Size The Size of the data type in bytes.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_ENDIANNESS_IMPL_H_
#define SRC_GENIE_UTIL_ENDIANNESS_IMPL_H_

// -----------------------------------------------------------------------------

#include <climits>

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

template <typename T, size_t Size>
void SwapEndianness(T& u) {
  static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");

  for (size_t k = 0; k < Size / 2; k++) {
    const char tmp = reinterpret_cast<char*>(&u)[k];
    reinterpret_cast<char*>(&u)[k] = reinterpret_cast<char*>(&u)[Size - k - 1];
    reinterpret_cast<char*>(&u)[Size - k - 1] = tmp;
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_ENDIANNESS_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
