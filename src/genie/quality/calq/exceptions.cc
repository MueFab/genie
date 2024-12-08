/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/exceptions.h"

#include <string>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

Exception::Exception(std::string msg) : msg_(std::move(msg)) {}

// -----------------------------------------------------------------------------

Exception::Exception(const Exception& e) noexcept : msg_(e.msg_) {}

// -----------------------------------------------------------------------------

Exception::~Exception() noexcept = default;

// -----------------------------------------------------------------------------

std::string Exception::GetMessage() const { return msg_; }

// -----------------------------------------------------------------------------

const char* Exception::what() const noexcept { return msg_.c_str(); }

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
