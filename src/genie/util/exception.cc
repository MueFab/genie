/**
 * Copyright 2018-2024 The Genie Authors.
 * @file exception.cc
 * @brief Implementation of the Exception class for handling generic exceptions.
 *
 * This file contains the implementation of the Exception class, which provides
 * a base class for handling generic exceptions. It captures an error message
 * and provides methods to retrieve the message and a C-style string
 * representation of the message.
 *
 * @details The Exception class constructor takes an error message as a
 * parameter and stores it. It also provides a copy constructor for creating a
 * copy of an existing Exception object. The Msg() method returns the stored
 * error message, and the what() method returns a C-style string representation
 * of the message.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#include "genie/util/exception.h"

#include <string>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

Exception::Exception(std::string msg) noexcept : msg_(std::move(msg)) {}

// -----------------------------------------------------------------------------

Exception::Exception(const Exception& e) noexcept : msg_(e.Msg()) {}

// -----------------------------------------------------------------------------

std::string Exception::Msg() const { return msg_; }

// -----------------------------------------------------------------------------

const char* Exception::what() const noexcept { return msg_.c_str(); }

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
