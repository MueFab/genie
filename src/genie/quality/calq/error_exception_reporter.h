/**
 * Copyright 2018-2024 The Genie Authors.
 * @file error_exception_reporter.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Provides mechanisms for detailed error reporting and exception
 * handling in the CALQ module.
 *
 * This file contains the declaration of the `ErrorExceptionReporter` class,
 * which is used to generate detailed error messages, including the file name,
 * function name, and line number where the exception occurred. It also defines
 * the `throwErrorException` function to throw custom exceptions with detailed
 * context.
 *
 * @details The `ErrorExceptionReporter` class captures detailed contextual
 * information, such as the source file, function name, and line number, when an
 * exception is thrown. The provided macro `throwErrorException` redefines the
 * original function to include this context. This functionality is used to
 * improve debugging and error tracing within the CALQ module.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_ERROR_EXCEPTION_REPORTER_H_
#define SRC_GENIE_QUALITY_CALQ_ERROR_EXCEPTION_REPORTER_H_

// -----------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <utility>

#include "genie/quality/calq/exceptions.h"

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

/**
 * @brief Throws a custom `ErrorException` with the specified message.
 *
 * This function throws an `ErrorException` with the provided error message. It
 * is intended to be used as a default error handling mechanism within the CALQ
 * module.
 *
 * @param msg The error message to include in the exception.
 * @throw ErrorException Thrown with the provided error message.
 */
inline void ThrowErrorException(const std::string& msg) {
  std::cout.flush();
  throw ErrorException(msg);
}

/**
 * @brief Utility class for generating detailed error reports and throwing
 * exceptions.
 *
 * The `ErrorExceptionReporter` class is used to generate detailed error
 * messages that include the file name, function name, and line number where the
 * exception occurred. This additional context helps developers quickly identify
 * and debug the source of errors.
 *
 * @details The class is typically instantiated using the `throwErrorException`
 * macro, which automatically captures the current file name, function name, and
 * line number.
 */
class ErrorExceptionReporter {
 public:
  /**
   * @brief Constructs an `ErrorExceptionReporter` with contextual information.
   *
   * This constructor captures the file name, function name, and line number
   * where the error occurred. These values are used to generate a detailed
   * error message when the `operator()` method is called.
   *
   * @param file The name of the source file where the error occurred.
   * @param function The name of the function where the error occurred.
   * @param line The line number in the source file where the error occurred.
   */
  ErrorExceptionReporter(std::string file, std::string function,
                         const int& line)
      : file_(std::move(file)), function_(std::move(function)), line_(line) {}

  /**
   * @brief Throws an `ErrorException` with a detailed error message.
   *
   * This operator function constructs a detailed error message using the stored
   * file name, function name, and line number, along with the provided error
   * message. It then throws an `ErrorException` with the constructed message.
   *
   * @param msg The specific error message to include in the exception.
   * @throw ErrorException Thrown with a detailed error message including
   * contextual information.
   */
  void operator()(const std::string& msg) const {
    const std::string tmp = file_.substr(file_.find_last_of("/\\") + 1) + ":" +
                            function_ + ":" + std::to_string(line_) + ": " +
                            msg;
    ThrowErrorException(tmp);
  }

 private:
  std::string
      file_;  //!< @brief The name of the file where the exception occurred.
  std::string function_;  //!< @brief The name of the function where the
                          //!< exception occurred.
  int line_;  //!< @brief The line number where the exception occurred.
};

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
// Remove the original `throwErrorException` symbol to redefine it with the new
// version.
#undef throwErrorException

/**
 * @brief Macro for throwing exceptions with detailed contextual information.
 *
 * This macro redefines the `throwErrorException` function to automatically
 * capture the current file name, function name, and line number where the macro
 * is called. It instantiates an `ErrorExceptionReporter` with the captured
 * context and then throws an `ErrorException` using the provided message.
 *
 * @param msg The error message to include in the exception.
 */
#define THROW_ERROR_EXCEPTION \
  calq::ErrorExceptionReporter(__FILE__, __FUNCTION__, __LINE__)

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_ERROR_EXCEPTION_REPORTER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
