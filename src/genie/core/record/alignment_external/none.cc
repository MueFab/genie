/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/alignment_external/none.h"

#include <memory>

// -----------------------------------------------------------------------------

namespace genie::core::record::alignment_external {

// -----------------------------------------------------------------------------

None::None() : AlignmentExternal(Type::kNone) {}

// -----------------------------------------------------------------------------

void None::Write(util::BitWriter& writer) const {
  AlignmentExternal::Write(writer);
}

// -----------------------------------------------------------------------------

std::unique_ptr<AlignmentExternal> None::Clone() const {
  auto ret = std::make_unique<None>();
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_external

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
