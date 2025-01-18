/**
 * Copyright 2018-2024 The Genie Authors.
 * @file bit_reader.impl.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Implementation details of the BitReader utility class for reading bits
 * from a stream.
 *
 * This file contains the template implementations for the member functions of
 * the BitReader class. These functions support reading different integral types
 * and enforcing endianness.
 *
 * @details The file provides template specializations for reading various
 * integral types, including handling different bit sizes and enforcing
 * endianness conversion when reading multibyte values. It uses static asserts
 * to ensure that only integral types can be read, ensuring type safety.
 *
 * @warning This file includes workarounds for MSVC-specific warnings and
 * ensures compatibility across different compilers.
 */

#ifndef SRC_GENIE_UTIL_BIT_READER_IMPL_H_
#define SRC_GENIE_UTIL_BIT_READER_IMPL_H_

// -----------------------------------------------------------------------------

// Suppress MSVC warning about constants in template if-conditions
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4127)
#endif

// -----------------------------------------------------------------------------

#include "genie/util/endianness.h"

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

// Extract static assertions to a separate function to reduce redundancy

template <typename T>
void StaticAssertIntegralOrEnum() {
  static_assert(std::is_integral_v<T> || std::is_enum_v<T>,
                "T must be an integral type or enum");
}

// -----------------------------------------------------------------------------

template <typename T, typename>
T BitReader::Read() {
  StaticAssertIntegralOrEnum<T>();
  return static_cast<T>(ReadBits(sizeof(T) * 8));
}

// -----------------------------------------------------------------------------

template <typename T, typename>
T BitReader::Read(const uint8_t num_bits) {
  StaticAssertIntegralOrEnum<T>();
  return static_cast<T>(ReadBits(num_bits));
}

// -----------------------------------------------------------------------------

template <typename T, size_t NumBytes, typename>
T BitReader::ReadAlignedInt() {
  StaticAssertIntegralOrEnum<T>();
  static_assert(NumBytes > 0, "NUM_BYTES should be greater than 0.");
  static_assert(NumBytes <= sizeof(T),
                "UM_BYTES should be less than or equal to the Size of type T.");

  T result = static_cast<T>(0);
  input_stream_.read(reinterpret_cast<char*>(&result), NumBytes);

  // Swap Endianness if necessary
  if (NumBytes > 1) {
    SwapEndianness<T, NumBytes>(result);
  }

  // Extend sign bit if necessary for signed types
  if constexpr (std::is_signed_v<T>) {
    if (NumBytes < sizeof(T) &&
        (reinterpret_cast<unsigned char*>(&result)[NumBytes - 1] &
         0x80)) {  // NOLINT
      for (size_t i = NumBytes; i < sizeof(T); ++i) {
        reinterpret_cast<unsigned char*>(&result)[i] =
            static_cast<unsigned char>(0xFF);
      }
    }
  }

  return result;
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma warning(pop)
#endif

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_BIT_READER_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
