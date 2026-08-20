/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_MAKE_UNIQUE_IMPL_H_
#define SRC_GENIE_UTIL_MAKE_UNIQUE_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

template <class T, class... Args>
typename Unique_if<T>::Single_object make_unique(Args &&...args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// ---------------------------------------------------------------------------------------------------------------------

template <class T>
typename Unique_if<T>::Unknown_bound make_unique(size_t n) {
    typedef typename std::remove_extent<T>::type U;
    return std::unique_ptr<T>(new U[n]());
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_MAKE_UNIQUE_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
