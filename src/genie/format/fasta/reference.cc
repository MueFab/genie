/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/fasta/reference.h"

#include <string>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::format::fasta {

// -----------------------------------------------------------------------------
Reference::Reference(std::string name, const size_t length, Manager* m)
    : core::Reference(std::move(name), 0, length), mgr_(m) {}

// -----------------------------------------------------------------------------
std::string Reference::GetSequence(const uint64_t start, const uint64_t end) {
  return mgr_->GetRef(GetName(), start, end);
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::fasta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
