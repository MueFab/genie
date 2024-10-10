/**
 * @file endianness.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of utility functions for handling endianness.
 *
 * This file contains the declaration of functions used for swapping endianness of different data types.
 * It provides a template-based approach to handle various data sizes and types. The functions are
 * designed to be used with both integral and user-defined types, enabling byte-order conversions.
 *
 * @details The endianness utility is essential for systems that need to convert between little-endian
 * and big-endian formats. These conversions are required when handling data across platforms with
 * different native endianness representations, ensuring consistent interpretation of binary data.
 */

#ifndef SRC_GENIE_UTIL_ENDIANNESS_H_
#define SRC_GENIE_UTIL_ENDIANNESS_H_

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Swaps the endianness of the given value.
 *
 * This template function reverses the byte order of the provided value, changing its
 * endianness. It uses the `SIZE` parameter, which defaults to the size of the type `T`,
 * to determine the number of bytes to swap. This function is typically used for converting
 * between little-endian and big-endian formats.
 *
 * @tparam T The data type of the value to be swapped.
 * @tparam SIZE The size of the value in bytes (defaults to `sizeof(T)`).
 * @param u The value whose endianness will be swapped.
 *
 * @note The function performs an in-place modification of the value `u`.
 */
template <typename T, size_t SIZE = sizeof(T)>
inline void swap_endianness(T& u);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/endianness.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_ENDIANNESS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
