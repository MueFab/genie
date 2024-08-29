/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_EXCEPTIONS_H_
#define SRC_GENIE_QUALITY_CALQ_EXCEPTIONS_H_

// -----------------------------------------------------------------------------

#include <exception>
#include <iostream>
#include <string>

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

/**
 * Default exception
 */
class Exception : public std::exception {
 public:
    /**
     * Create new exception
     */
    explicit Exception(std::string  msg);

    /**
     * @param e Copy
     */
    Exception(const Exception& e) noexcept;

    /**
     * Destructor
     */
    ~Exception() noexcept override;

    /**
     * @return Get message
     */
    [[nodiscard]] virtual std::string getMessage() const;

    /**
     * Get message
     */
    [[nodiscard]] const char* what() const noexcept override;

 protected:
    std::string msg_;
};

// -----------------------------------------------------------------------------

/**
 * Error exception
 */
class ErrorException : public Exception {
 public:
    /**
     * Constructor
     * @param msg
     */
    explicit ErrorException(const std::string& msg) : Exception(msg) {}
};

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_EXCEPTIONS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
