/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/reference_source.h"

#include <memory>

#include "genie/core/meta/internal_ref.h"
#include "genie/util/literal.h"

// -----------------------------------------------------------------------------

namespace genie::core {

using util::operator""_u16;

// -----------------------------------------------------------------------------

ReferenceSource::ReferenceSource(ReferenceManager* mgr) : ref_mgr_(mgr) {}

// -----------------------------------------------------------------------------

meta::Reference ReferenceSource::GetMeta() const {
  return {
      "",           0, 0, 0, std::make_unique<meta::InternalRef>(0_u16, 0_u16),
      "Placeholder"};
}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
