/**
 * @file bitwriter.impl.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/mitogen/genie for more details.
 *
 * @brief Implementation of BitWriter utility class template methods for specific tasks.
 *
 * This file contains the implementation of the BitWriter class's template methods,
 * specifically specialized functions like writing aligned integers to the output stream
 * while managing endianness.
 *
 * @details The BitWriter class template method writeAlignedInt is implemented here
 * to handle writing of integral types to the output stream in a byte-aligned manner.
 * This method ensures data integrity by handling endianness swapping when necessary.
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

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

template <typename T, size_t NUM_BYTES, typename>
void BitWriter::writeAlignedInt(T val) {
    static_assert(NUM_BYTES > 0, "NUM_BYTES should be greater than 0.");
    static_assert(NUM_BYTES <= sizeof(T), "NUM_BYTES should be less than or equal to the size of type T.");

    // Swap Endianness if necessary
    if (NUM_BYTES > 1) {
        swap_endianness<T, NUM_BYTES>(val);
    }

    stream.write(reinterpret_cast<char*>(&val), NUM_BYTES);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma warning(pop)
#endif

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_BITWRITER_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
