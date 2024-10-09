/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_REFERENCE_H
#define GENIE_REFERENCE_H

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/reference.h"
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

// ---------------------------------------------------------------------------------------------------------------------

const std::string& Reference::getName() const { return name; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Reference::getStart() const { return start; }

// ---------------------------------------------------------------------------------------------------------------------

uint64_t Reference::getEnd() const { return end; }

// ---------------------------------------------------------------------------------------------------------------------

Reference::Reference(std::string _name, uint64_t _start, uint64_t _end)
    : name(std::move(_name)), start(_start), end(_end) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_REFERENCE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
