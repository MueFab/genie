/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/classifier_bypass.h"

#include <utility>

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------

record::Chunk ClassifierBypass::GetChunk() {
  flushing_ = false;
  record::Chunk ret;
  if (vec_.empty()) {
    return ret;
  }
  ret = std::move(vec_.front());
  vec_.erase(vec_.begin());
  return ret;
}

// -----------------------------------------------------------------------------

void ClassifierBypass::Add(record::Chunk&& c) {
  vec_.emplace_back(std::move(c));
}

// -----------------------------------------------------------------------------

void ClassifierBypass::Flush() { flushing_ = true; }

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
