/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_ENDIANNESS_H_
#define SRC_GENIE_UTIL_ENDIANNESS_H_

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 * @brief Swap endianness of value
 * @tparam T Data type
 * @tparam SIZE Size in bytes of value
 * @param u Value
 */
template <typename T, size_t SIZE = sizeof(T)>
inline void swap_endianness(T& u);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/util/endianness.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_ENDIANNESS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
