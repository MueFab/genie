/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/global_cfg.h"

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------
util::IndustrialPark& GlobalCfg::GetIndustrialPark() { return fpark_; }

// -----------------------------------------------------------------------------
GlobalCfg& GlobalCfg::GetSingleton() {
  static GlobalCfg cfg;
  return cfg;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
