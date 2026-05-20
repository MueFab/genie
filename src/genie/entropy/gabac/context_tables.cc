/**
* Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/context_tables.h"

#include <vector>

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac::contexttables {

// -----------------------------------------------------------------------------

std::vector<ContextModel> BuildContextTable(const uint64_t num_contexts) {
  return std::vector<ContextModel>(num_contexts);
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac::contexttables

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------