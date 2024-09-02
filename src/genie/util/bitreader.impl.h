/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
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

template <typename T, typename>
T BitReader::read() {
    return static_cast<T>(read_b(sizeof(T) * 8));
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, typename>
T BitReader::read(uint8_t s) {
    return static_cast<T>(read_b(s));
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, size_t SIZE, typename>
T BitReader::readBypassBE() {
    static_assert(SIZE > 0, "SIZE == 0");
    static_assert(SIZE <= sizeof(T), "SIZE > sizeof(T)");
    T ret = static_cast<T>(0);
    istream.read(reinterpret_cast<char*>(&ret), SIZE);

    // Swap Endianness if necessary
    if (SIZE > 1) {
        swap_endianness<T, SIZE>(ret);
    }

    // Extend sign bit if necessary
    if (std::is_signed<T>::value && SIZE < sizeof(T) && reinterpret_cast<char*>(&ret)[SIZE - 1] < 0) {
        for (size_t i = SIZE; i < sizeof(T); ++i) {
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

#endif  // SRC_GENIE_UTIL_BITREADER_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
