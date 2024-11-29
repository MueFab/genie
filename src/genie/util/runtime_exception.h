/**
 * Copyright 2018-2024 The Genie Authors.
 * @file runtime_exception.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the RuntimeException class for handling runtime errors.
 *
 * This file contains the declaration of the `RuntimeException` class, which
 * extends the `Exception` class to provide more context for runtime errors. It
 * includes additional information such as the file name, function name, and
 * line number where the error occurred. The file also defines utility macros
 * for throwing these exceptions in a standardized manner.
 *
 * @details The `RuntimeException` class is used throughout the GENIE framework
 * to signal runtime errors with detailed diagnostic information. This aids in
 * debugging by providing information about the source of the exception. The
 * accompanying macros (`UTILS_DIE`, `UTILS_DIE_IF`, and
 * `UTILS_THROW_RUNTIME_EXCEPTION`) simplify the process of generating
 * exceptions with the relevant context.
 */

#ifndef SRC_GENIE_UTIL_RUNTIME_EXCEPTION_H_
#define SRC_GENIE_UTIL_RUNTIME_EXCEPTION_H_

// -----------------------------------------------------------------------------
#define UTILS_DIE(msg) UTILS_THROW_RUNTIME_EXCEPTION(msg)
#define UTILS_DIE_IF(cond, msg)           \
  do {                                    \
    if (cond) {                           \
      UTILS_THROW_RUNTIME_EXCEPTION(msg); \
    }                                     \
  } while (false)
#define UTILS_THROW_RUNTIME_EXCEPTION(msg) \
  throw genie::util::RuntimeException(__FILE__, "", __LINE__, msg)

// -----------------------------------------------------------------------------

#include <string>

#include "genie/util/exception.h"

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Custom runtime exception class for GENIE framework.
 *
 * The `RuntimeException` class extends the base `Exception` class and provides
 * additional context for runtime errors. It includes information such as the
 * file name, function name, and line number where the exception occurred,
 * making it easier to diagnose and debug issues.
 */
class RuntimeException : public Exception {
 public:
  /**
   * @brief Constructs a `RuntimeException` with detailed context information.
   *
   * This constructor initializes the `RuntimeException` with the file name,
   * function name, line number, and a custom message describing the error. It
   * is primarily used through the `UTILS_THROW_RUNTIME_EXCEPTION` macro to
   * automatically capture the context.
   *
   * @param file The name of the file where the exception occurred.
   * @param function The name of the function where the exception was thrown.
   * @param line The line number where the exception was generated.
   * @param msg A custom error message describing the exception.
   */
  explicit RuntimeException(const std::string& file,
                            const std::string& function, int line,
                            const std::string& msg) noexcept;

  /**
   * @brief Copy constructor for `RuntimeException`.
   *
   * This copy constructor creates a new `RuntimeException` object from an
   * existing one, preserving all context information and the error message.
   *
   * @param e The `RuntimeException` object to copy.
   */
  RuntimeException(const RuntimeException& e) noexcept;

  /**
   * @brief Virtual destructor for safe polymorphic deletion.
   *
   * Ensures that derived classes are properly destructed when handled through
   * a pointer to the `RuntimeException` interface.
   */
  ~RuntimeException() noexcept override = default;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_RUNTIME_EXCEPTION_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
