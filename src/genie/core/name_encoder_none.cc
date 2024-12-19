/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/name_encoder_none.h"

#include <tuple>

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------
std::tuple<AccessUnit::Descriptor, stats::PerfStats> NameEncoderNone::Process(
    const record::Chunk&) {
  return std::make_tuple(AccessUnit::Descriptor(GenDesc::kReadName),
                         stats::PerfStats());
}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
