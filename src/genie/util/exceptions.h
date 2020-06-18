/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef UTIL_EXCEPTIONS_H_
#define UTIL_EXCEPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------

#define DIE(msg) UTILS_DIE(msg)
#define UTILS_DIE(msg) UTILS_THROW_RUNTIME_EXCEPTION(msg)
#define UTILS_DIE_IF(cond, msg)                 \
    do {                                        \
        if (cond) {                             \
            UTILS_THROW_RUNTIME_EXCEPTION(msg); \
        }                                       \
    } while (false)
#define UTILS_THROW_RUNTIME_EXCEPTION(msg) throw genie::util::RuntimeException(__FILE__, __FUNCTION__, __LINE__, msg)

// ---------------------------------------------------------------------------------------------------------------------

#include <exception>
#include <iostream>
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 *
 */
class Exception : public std::exception {
   public:
    /**
     *
     * @param msg
     */
    explicit Exception(std::string msg);

    /**
     *
     */
    ~Exception() noexcept override;

    /**
     *
     * @return
     */
    virtual std::string msg() const;

    /**
     *
     * @return
     */
    const char *what() const noexcept override;

   protected:
    std::string msg_;  //!<
};

/**
 *
 */
class RuntimeException : public Exception {
   public:
    /**
     *
     * @param file
     * @param function
     * @param line
     * @param msg
     */
    explicit RuntimeException(const std::string &file, const std::string &function, int line,
                              const std::string &msg) noexcept;

    /**
     *
     * @param e
     */
    RuntimeException(const RuntimeException &e) noexcept;

    /**
     *
     */
    ~RuntimeException() noexcept override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // UTIL_EXCEPTIONS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------