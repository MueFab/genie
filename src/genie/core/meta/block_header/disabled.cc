/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/meta/block_header/disabled.h"

// -----------------------------------------------------------------------------

namespace genie::core::meta::block_header {

// -----------------------------------------------------------------------------
Disabled::Disabled(const bool ordered)
    : BlockHeader(HeaderType::kDisabled), ordered_blocks_flag_(ordered) {}

// -----------------------------------------------------------------------------
Disabled::Disabled(const nlohmann::json& json)
    : BlockHeader(HeaderType::kDisabled),
      ordered_blocks_flag_(json["ordered_blocks_flag"]) {}

// -----------------------------------------------------------------------------
bool Disabled::GetOrderedFlag() const { return ordered_blocks_flag_; }

// -----------------------------------------------------------------------------
nlohmann::json Disabled::ToJson() const {
  nlohmann::json ret;
  ret["ordered_blocks_flag"] = ordered_blocks_flag_;
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta::block_header

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
