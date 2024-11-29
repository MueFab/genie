/**
 * Copyright 2018-2024 The Genie Authors.
 * @file runtime_exception.cc
 * @brief Implementation of the RuntimeException class for handling runtime
 * errors.
 *
 * This file contains the implementation of the RuntimeException class, which
 * provides a mechanism for handling runtime errors by extending the base
 * Exception class. It captures the file name, function name, line number, and
 * error message where the exception occurred.
 *
 * @details The RuntimeException class constructor takes the file name, function
 * name, line number, and error message as parameters and constructs a detailed
 * error message. It also provides a copy constructor for creating a copy of an
 * existing RuntimeException object.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#include "genie/util/runtime_exception.h"

#include <string>

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------
RuntimeException::RuntimeException(const std::string& file,
                                   const std::string& function, const int line,
                                   const std::string& msg) noexcept
    : Exception(file + ":" + function + ":" + std::to_string(line) + ": " +
                msg) {
  // These dummy casts just avoid compiler warnings due to unused variables
  static_cast<void>(file);
  static_cast<void>(function);
  static_cast<void>(line);
}

// -----------------------------------------------------------------------------
RuntimeException::RuntimeException(const RuntimeException& e) noexcept
    : Exception(e.what()) {}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
