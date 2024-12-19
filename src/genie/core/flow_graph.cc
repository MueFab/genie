/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/flow_graph.h"

#include <memory>

#include "genie/core/meta/block_header/enabled.h"

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------
meta::Dataset FlowGraph::GetMeta() const {
  return {0, std::make_unique<meta::block_header::Enabled>(false, false), "",
          ""};
}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
