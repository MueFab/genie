/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef GENIE_REFERENCE_H
#define GENIE_REFERENCE_H

// -----------------------------------------------------------------------------

#include "genie/core/reference.h"

#include <string>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------
const std::string& Reference::GetName() const { return name_; }

// -----------------------------------------------------------------------------
uint64_t Reference::GetStart() const { return start_; }

// -----------------------------------------------------------------------------
uint64_t Reference::GetEnd() const { return end_; }

// -----------------------------------------------------------------------------
Reference::Reference(std::string name, const uint64_t start, const uint64_t end)
    : name_(std::move(name)), start_(start), end_(end) {}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // GENIE_REFERENCE_H

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
