/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/alignment_split/unpaired.h"

#include <memory>

// -----------------------------------------------------------------------------

namespace genie::core::record::alignment_split {

// -----------------------------------------------------------------------------
Unpaired::Unpaired() : AlignmentSplit(Type::kUnpaired) {}

// -----------------------------------------------------------------------------
void Unpaired::Write(util::BitWriter& writer) const {
  AlignmentSplit::Write(writer);
}

// -----------------------------------------------------------------------------
std::unique_ptr<AlignmentSplit> Unpaired::clone() const {
  auto ret = std::make_unique<Unpaired>();
  ret->split_alignment_ = this->split_alignment_;
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_split

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
