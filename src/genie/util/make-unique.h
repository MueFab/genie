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
struct _Unique_if {
    typedef std::unique_ptr<T> _Single_object;
};

/**
 *
 * @tparam T
 */
template <class T>
struct _Unique_if<T[]> {
    typedef std::unique_ptr<T[]> _Unknown_bound;
};

/**
 *
 * @tparam T
 * @tparam N
 */
template <class T, size_t N>
struct _Unique_if<T[N]> {
    typedef void _Known_bound;
};

/**
 *
 * @tparam T
 * @tparam Args
 * @param args
 * @return
 */
template <class T, class... Args>
typename _Unique_if<T>::_Single_object make_unique(Args &&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/**
 *
 * @tparam T
 * @param n
 * @return
 */
template <class T>
typename _Unique_if<T>::_Unknown_bound make_unique(size_t n) {
    typedef typename std::remove_extent<T>::type U;
    return std::unique_ptr<T>(new U[n]());
}

/**
 *
 * @tparam T
 * @tparam Args
 * @return
 */
template <class T, class... Args>
typename _Unique_if<T>::_Known_bound make_unique(Args &&...) = delete;

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MAKE_UNIQUE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
