/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MAKE_UNIQUE_H
#define GENIE_MAKE_UNIQUE_H

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 *
 * @tparam T
 */
template <class T>
struct Unique_if {
    typedef std::unique_ptr<T> Single_object;
};

/**
 *
 * @tparam T
 */
template <class T>
struct Unique_if<T[]> {
    typedef std::unique_ptr<T[]> Unknown_bound;
};

/**
 *
 * @tparam T
 * @tparam N
 */
template <class T, size_t N>
struct Unique_if<T[N]> {
    typedef void Known_bound;  // NOLINT
};

/**
 *
 * @tparam T
 * @tparam Args
 * @param args
 * @return
 */
template <class T, class... Args>
typename Unique_if<T>::Single_object make_unique(Args &&... args);

/**
 *
 * @tparam T
 * @param n
 * @return
 */
template <class T>
typename Unique_if<T>::Unknown_bound make_unique(size_t n);

/**
 *
 * @tparam T
 * @tparam Args
 * @return
 */
template <class T, class... Args>                                     // NOLINT
typename Unique_if<T>::Known_bound make_unique(Args &&...) = delete;  // NOLINT

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#include "make-unique.impl.h"

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MAKE_UNIQUE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
