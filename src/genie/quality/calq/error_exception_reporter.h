/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_ERROR_EXCEPTION_REPORTER_H_
#define SRC_GENIE_QUALITY_CALQ_ERROR_EXCEPTION_REPORTER_H_

// -----------------------------------------------------------------------------

#include <exception>
#include <iostream>
#include <string>
#include <utility>

// -----------------------------------------------------------------------------

#include "genie/quality/calq/exceptions.h"

// -----------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace calq {

// -----------------------------------------------------------------------------

inline void throwErrorException(const std::string& msg) {
    std::cout.flush();
    throw ErrorException(msg);
}

// -----------------------------------------------------------------------------

class ErrorExceptionReporter {
 public:
    ErrorExceptionReporter(std::string file, std::string function, const int& line)
        : file_(std::move(file)), function_(std::move(function)), line_(line) {}

    // -------------------------------------------------------------------------

    void operator()(const std::string& msg) {
        // std::cerr << file << ":" << function << ":" << line << ": ";
        std::string tmp =
            file_.substr(file_.find_last_of("/\\") + 1) + ":" + function_ + ":" + std::to_string(line_) + ": " + msg;
        // Can use the original name here, as it is still defined
        throwErrorException(tmp);
    }

    // -------------------------------------------------------------------------

 private:
    std::string file_;
    std::string function_;
    int line_;
};

// -----------------------------------------------------------------------------

}  // namespace calq
}  // namespace quality
}  // namespace genie

// -----------------------------------------------------------------------------

// Remove the symbol for the function, then define a new version that instead
// creates a stack temporary instance of ErrorExceptionReporter initialized
// with the caller.
#undef throwErrorException
#define throwErrorException calq::ErrorExceptionReporter(__FILE__, __FUNCTION__, __LINE__)

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_ERROR_EXCEPTION_REPORTER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
