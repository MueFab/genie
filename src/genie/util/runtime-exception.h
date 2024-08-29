/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_RUNTIME_EXCEPTION_H_
#define SRC_GENIE_UTIL_RUNTIME_EXCEPTION_H_

// ---------------------------------------------------------------------------------------------------------------------

#define UTILS_DIE(msg) UTILS_THROW_RUNTIME_EXCEPTION(msg)
#define UTILS_DIE_IF(cond, msg)                 \
    do {                                        \
        if (cond) {                             \
            UTILS_THROW_RUNTIME_EXCEPTION(msg); \
        }                                       \
    } while (false)
#define UTILS_THROW_RUNTIME_EXCEPTION(msg) throw genie::util::RuntimeException(__FILE__, __FUNCTION__, __LINE__, msg)

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/util/exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Genie runtime exception for all the bad stuff that can happen.
 */
class RuntimeException : public Exception {
 public:
    /**
     * @brief
     * @param file
     * @param function
     * @param line
     * @param msg
     */
    explicit RuntimeException(const std::string &file, const std::string &function, int line,
                              const std::string &msg) noexcept;

    /**
     * @brief
     * @param e
     */
    RuntimeException(const RuntimeException &e) noexcept;

    /**
     * @brief Needed for inheritance.
     */
    ~RuntimeException() noexcept override = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_RUNTIME_EXCEPTION_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
