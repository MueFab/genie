/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_BITWRITER_IMPL_H_
#define SRC_GENIE_UTIL_BITWRITER_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

// Suppress MSVC warning about constants in template if-conditions
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4127)
#endif

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/endianness.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, size_t SIZE, typename>
void BitWriter::writeBypassBE(T val) {
    static_assert(SIZE > 0, "SIZE == 0");
    static_assert(SIZE <= sizeof(T), "SIZE > sizeof(T)");

    // Swap Endianness if necessary
    if (SIZE > 1) {
        swap_endianness<T, SIZE>(val);
    }

    stream->write(reinterpret_cast<char*>(&val), SIZE);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma warning(pop)
#endif

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_BITWRITER_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
