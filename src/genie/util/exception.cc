/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/util/exception.h"
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

Exception::Exception(std::string msg) noexcept : msg_(std::move(msg)) {}

// ---------------------------------------------------------------------------------------------------------------------

Exception::Exception(const Exception& e) noexcept : msg_(e.msg()) {}

// ---------------------------------------------------------------------------------------------------------------------

std::string Exception::msg() const { return msg_; }

// ---------------------------------------------------------------------------------------------------------------------

const char* Exception::what() const noexcept { return msg_.c_str(); }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
