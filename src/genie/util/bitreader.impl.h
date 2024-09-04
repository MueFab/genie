/**
 * @file bitreader.impl.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/mitogen/genie for more details.
 *
 * @brief Implementation details of the BitReader utility class for reading bits from a stream.
 *
 * This file contains the template implementations for the member functions of the BitReader class.
 * These functions support reading different integral types and enforcing endianness.
 *
 * @details The file provides template specializations for reading various integral types, including
 * handling different bit sizes and enforcing endianness conversion when reading multibyte values.
 * It uses static asserts to ensure that only integral types can be read, ensuring type safety.
 *
 * @warning This file includes workarounds for MSVC-specific warnings and ensures compatibility across different
 * compilers.
 */

#ifndef SRC_GENIE_UTIL_BITREADER_IMPL_H_
#define SRC_GENIE_UTIL_BITREADER_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

// Suppress MSVC warning about constants in template if-conditions
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4127)
#endif

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/endianness.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

// Extract static assertions to a separate function to reduce redundancy
template <typename T>
void staticAssertIntegralOrEnum() {
    static_assert(std::is_integral<T>::value || std::is_enum<T>::value, "T must be an integral type or enum");
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, typename>
T BitReader::read() {
    staticAssertIntegralOrEnum<T>();
    return static_cast<T>(readBits(sizeof(T) * 8));
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, typename>
T BitReader::read(uint8_t numBits) {
    staticAssertIntegralOrEnum<T>();
    return static_cast<T>(readBits(numBits));
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, size_t NUM_BYTES, typename>
T BitReader::readAlignedInt() {
    staticAssertIntegralOrEnum<T>();
    static_assert(NUM_BYTES > 0, "NUM_BYTES should be greater than 0.");
    static_assert(NUM_BYTES <= sizeof(T), "UM_BYTES should be less than or equal to the size of type T.");

    T result = static_cast<T>(0);
    inputStream.read(reinterpret_cast<char*>(&result), NUM_BYTES);

    // Swap Endianness if necessary
    if (NUM_BYTES > 1) {
        swap_endianness<T, NUM_BYTES>(result);
    }

    // Extend sign bit if necessary for signed types
    if constexpr (std::is_signed<T>::value) {
        if (NUM_BYTES < sizeof(T) && (reinterpret_cast<unsigned char*>(&result)[NUM_BYTES - 1] & 0x80)) {
            for (size_t i = NUM_BYTES; i < sizeof(T); ++i) {
                reinterpret_cast<unsigned char*>(&result)[i] = static_cast<unsigned char>(0xFF);
            }
        }
    }

    return result;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma warning(pop)
#endif

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_BITREADER_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
