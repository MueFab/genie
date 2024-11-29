/**
 * Copyright 2018-2024 The Genie Authors.
 * @file exceptions.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Defines custom exception classes for handling errors in the CALQ
 * module.
 *
 * This file contains the declaration of custom exception classes for handling
 * various types of errors in the CALQ module. It provides a base `Exception`
 * class and a specialized `ErrorException` class that inherits from the base
 * class.
 *
 * @details The `Exception` class serves as the base class for all custom
 * exceptions in the CALQ module, providing a standard mechanism for error
 * reporting and message handling. The `ErrorException` class extends this
 * functionality and is used for reporting specific errors encountered during
 * quality value encoding and decoding.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_EXCEPTIONS_H_
#define SRC_GENIE_QUALITY_CALQ_EXCEPTIONS_H_

// -----------------------------------------------------------------------------
#include <exception>
#include <string>

// -----------------------------------------------------------------------------
namespace genie::quality::calq {

/**
 * @brief Base exception class for handling errors in the CALQ module.
 *
 * The `Exception` class is used as a base class for all custom exceptions in
 * the CALQ module. It extends the standard `std::exception` class and provides
 * additional functionalities for storing and retrieving error messages.
 */
class Exception : public std::exception {
 public:
  /**
   * @brief Constructs a new `Exception` with the specified message.
   *
   * This constructor initializes the `Exception` with the provided error
   * message. The message is stored internally and can be retrieved using the
   * `getMessage()` method.
   *
   * @param msg The error message to be associated with this exception.
   */
  explicit Exception(std::string msg);

  /**
   * @brief Copy constructor for creating a new `Exception` from an existing
   * one.
   *
   * This copy constructor creates a new `Exception` object by copying the error
   * message and other internal states from the given `Exception` object.
   *
   * @param e The `Exception` object to copy.
   */
  Exception(const Exception& e) noexcept;

  /**
   * @brief Destructor for the `Exception` class.
   *
   * This destructor ensures that any resources held by the exception object are
   * released. It is marked as `noexcept` to comply with the `std::exception`
   * interface.
   */
  ~Exception() noexcept override;

  /**
   * @brief Retrieves the error message associated with this exception.
   *
   * This method returns the internal error message stored in the exception
   * object. It can be used for logging or displaying error messages to the
   * user.
   *
   * @return A string containing the error message.
   */
  [[nodiscard]] virtual std::string GetMessage() const;

  /**
   * @brief Retrieves the error message in C-style string format.
   *
   * This method overrides the `what()` function from the `std::exception` class
   * to return the error message as a C-style string (`const char*`). It is
   * useful for compatibility with standard error handling mechanisms.
   *
   * @return A C-style string containing the error message.
   */
  [[nodiscard]] const char* what() const noexcept override;

 protected:
  std::string
      msg_;  //!< @brief The error message associated with this exception.
};

/**
 * @brief Specialized exception class for reporting critical errors.
 *
 * The `ErrorException` class extends the base `Exception` class and is used to
 * report critical errors encountered during the execution of the CALQ module.
 * It does not add any new functionality but serves as a distinct type for error
 * reporting.
 */
class ErrorException final : public Exception {
 public:
  /**
   * @brief Constructs a new `ErrorException` with the specified message.
   *
   * This constructor initializes the `ErrorException` with the given error
   * message, which can be retrieved using the `getMessage()` or `what()`
   * methods.
   *
   * @param msg The error message to be associated with this exception.
   */
  explicit ErrorException(const std::string& msg) : Exception(msg) {}
};

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_EXCEPTIONS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
