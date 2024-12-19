/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/entropy/gabac/context_selector.h"

#include <vector>

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------
ContextSelector::ContextSelector(const paramcabac::StateVars& state_vars)
    : state_vars_(state_vars) {}

// -----------------------------------------------------------------------------
ContextSelector::ContextSelector(const ContextSelector& src) = default;

// -----------------------------------------------------------------------------
unsigned int ContextSelector::GetContextIdxOrder0(
    const uint8_t subsym_idx) const {
  return static_cast<unsigned int>(subsym_idx *
                                   state_vars_.GetCodingSizeCtxOffset());
}

// -----------------------------------------------------------------------------
unsigned int ContextSelector::GetContextIdxOrderGt0(
    const uint8_t subsym_idx, const uint8_t prv_idx,
    const std::vector<Subsymbol>& subsymbols,
    const uint8_t coding_order) const {
  unsigned int ctx_idx = 0;
  ctx_idx += state_vars_.GetNumCtxLuts();
  ctx_idx += static_cast<unsigned int>(subsym_idx *
                                       state_vars_.GetCodingSizeCtxOffset());
  for (uint8_t i = 1; i <= coding_order; i++) {
    ctx_idx +=
        static_cast<unsigned int>(subsymbols[prv_idx].prv_values[i - 1] *
                                  state_vars_.GetCodingOrderCtxOffset(i));
  }

  return ctx_idx;
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
