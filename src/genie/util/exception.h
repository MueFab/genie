/**
 * Copyright 2018-2024 The Genie Authors.
 * @file exception.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the Exception class for handling error messages.
 *
 * This file contains the declaration of the Exception class, which serves as a
 * general-purpose exception type for the GENIE framework. It provides
 * constructors for initializing error messages and methods for retrieving those
 * messages in different formats. This class is used as a base for creating more
 * specific exception types within the framework.
 *
 * @details The Exception class inherits from `std::exception` and provides
 * additional functionality for storing and retrieving error messages. It
 * includes methods to obtain the error message as a standard C++ string or as a
 * C-style string for compatibility with legacy code.
 */

#ifndef SRC_GENIE_UTIL_EXCEPTION_H_
#define SRC_GENIE_UTIL_EXCEPTION_H_

// -----------------------------------------------------------------------------

#include <string>

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief A general exception class for error handling.
 *
 * The `Exception` class is a custom exception type that provides mechanisms for
 * storing and retrieving error messages. It extends the standard
 * `std::exception` and includes methods to access the stored message in various
 * formats. This class is designed to be inherited by other exception types to
 * provide specific error handling capabilities.
 */
class Exception : public std::exception {
 public:
  /**
   * @brief Constructs an Exception object with a given message.
   *
   * This constructor initializes the exception with a custom message, which
   * can later be retrieved using the `msg()` or `what()` methods.
   *
   * @param msg The error message to be stored in the exception.
   */
  explicit Exception(std::string msg) noexcept;

  /**
   * @brief Copy constructor for the Exception class.
   *
   * This constructor performs a no-throw copy of the given Exception object,
   * preserving the error message.
   *
   * @param e The Exception object to copy.
   */
  Exception(const Exception& e) noexcept;

  /**
   * @brief Virtual destructor for safe inheritance.
   *
   * Ensures that derived classes can be safely destructed through a pointer
   * to the base Exception class.
   */
  ~Exception() noexcept override = default;

  /**
   * @brief Retrieves the internal message as a C++ string.
   *
   * This method returns the stored error message as a standard C++ string.
   *
   * @return The internal error message.
   */
  [[nodiscard]] virtual std::string Msg() const;

  /**
   * @brief Retrieves the internal message as a C-style string.
   *
   * This method overrides `std::exception::what()` to return the stored error
   * message as a null-terminated C-style string. It is useful for
   * compatibility with legacy error handling mechanisms that expect C-style
   * strings.
   *
   * @return The error message as a C-style string.
   */
  [[nodiscard]] const char* what() const noexcept override;

 protected:
  /// The exception message stored as a standard string.
  std::string msg_;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_EXCEPTION_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
