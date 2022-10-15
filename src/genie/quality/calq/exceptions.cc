/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/quality/calq/exceptions.h"

// -----------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace calq {

// -----------------------------------------------------------------------------

Exception::Exception(const std::string& msg) : msg_(msg) {}

// -----------------------------------------------------------------------------

Exception::Exception(const Exception& e) noexcept : msg_(e.msg_) {}

// -----------------------------------------------------------------------------

Exception::~Exception() noexcept = default;

// -----------------------------------------------------------------------------

std::string Exception::getMessage() const { return msg_; }

// -----------------------------------------------------------------------------

const char* Exception::what() const noexcept { return msg_.c_str(); }

// -----------------------------------------------------------------------------

}  // namespace calq
}  // namespace quality
}  // namespace genie

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
