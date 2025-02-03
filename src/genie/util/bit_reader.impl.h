/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_BIT_READER_IMPL_H_
#define SRC_GENIE_UTIL_BIT_READER_IMPL_H_

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

template <typename T, typename>
T BitReader::Read() {
    return static_cast<T>(ReadBits(sizeof(T) * 8));
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, typename>
T BitReader::Read(uint8_t s) {
    return static_cast<T>(ReadBits(s));
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, size_t NumBytes, typename>
T BitReader::ReadAlignedInt() {
    static_assert(NumBytes > 0, "NumBytes == 0");
    static_assert(NumBytes <= sizeof(T), "NumBytes > sizeof(T)");
    T ret = static_cast<T>(0);
    istream.read(reinterpret_cast<char*>(&ret), NumBytes);

    // Swap Endianness if necessary
    if (NumBytes > 1) {
        SwapEndianness<T, NumBytes>(ret);
    }

    // Extend sign bit if necessary
    if (std::is_signed<T>::value && NumBytes < sizeof(T) && reinterpret_cast<char*>(&ret)[NumBytes - 1] < 0) {
        for (size_t i = NumBytes; i < sizeof(T); ++i) {
            reinterpret_cast<unsigned char*>(&ret)[i] = static_cast<unsigned char>(0xff);
        }
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma warning(pop)
#endif

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_BIT_READER_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
