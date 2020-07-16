/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_MAKE_UNIQUE_IMPL_H
#define GENIE_MAKE_UNIQUE_IMPL_H

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

// ---------------------------------------------------------------------------------------------------------------------

template <class T, class... Args>
typename _Unique_if<T>::_Single_object make_unique(Args &&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// ---------------------------------------------------------------------------------------------------------------------

template <class T>
typename _Unique_if<T>::_Unknown_bound make_unique(size_t n) {
    typedef typename std::remove_extent<T>::type U;
    return std::unique_ptr<T>(new U[n]());
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_MAKE_UNIQUE_IMPL_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------