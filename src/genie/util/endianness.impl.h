/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_ENDIANNESS_IMPL_H_
#define SRC_GENIE_UTIL_ENDIANNESS_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <climits>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, size_t SIZE>
inline void SwapEndianness(T& u) {
    static_assert(CHAR_BIT == 8, "CHAR_BIT != 8");

    for (size_t k = 0; k < SIZE / 2; k++) {
        char tmp = reinterpret_cast<char*>(&u)[k];
        reinterpret_cast<char*>(&u)[k] = reinterpret_cast<char*>(&u)[SIZE - k - 1];
        reinterpret_cast<char*>(&u)[SIZE - k - 1] = tmp;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_ENDIANNESS_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
