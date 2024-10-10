/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/util/runtime-exception.h"
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

RuntimeException::RuntimeException(const std::string &file, const std::string &function, int line,
                                   const std::string &msg) noexcept
    : Exception(file + ":" + function + ":" + std::to_string(line) + ": " + msg) {
    // These dummy casts just avoid compiler warnings due to unused variables
    static_cast<void>(file);
    static_cast<void>(function);
    static_cast<void>(line);
}

// ---------------------------------------------------------------------------------------------------------------------

RuntimeException::RuntimeException(const RuntimeException &e) noexcept : Exception(e.what()) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
