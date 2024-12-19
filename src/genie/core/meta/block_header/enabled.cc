/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/meta/block_header/enabled.h"

// -----------------------------------------------------------------------------

namespace genie::core::meta::block_header {

// -----------------------------------------------------------------------------
Enabled::Enabled(const bool mit, const bool cc)
    : BlockHeader(HeaderType::kEnabled), mit_flag_(mit), cc_mode_flag_(cc) {}

// -----------------------------------------------------------------------------
Enabled::Enabled(const nlohmann::json& json)
    : BlockHeader(HeaderType::kEnabled),
      mit_flag_(json["MIT_flag"]),
      cc_mode_flag_(json["CC_mode_flag"]) {}

// -----------------------------------------------------------------------------
nlohmann::json Enabled::ToJson() const {
  nlohmann::json ret;
  ret["MIT_flag"] = mit_flag_;
  ret["CC_mode_flag"] = cc_mode_flag_;
  return ret;
}

// -----------------------------------------------------------------------------
bool Enabled::GetMitFlag() const { return mit_flag_; }

// -----------------------------------------------------------------------------
bool Enabled::GetCcFlag() const { return cc_mode_flag_; }

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta::block_header

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
