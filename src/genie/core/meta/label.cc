/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/meta/label.h"

#include <string>
#include <utility>
#include <vector>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::core::meta {

// -----------------------------------------------------------------------------
Label::Label(std::string id) : label_id_(std::move(id)) {}

// -----------------------------------------------------------------------------
Label::Label(const nlohmann::json& json) : label_id_(json["label_ID"]) {
  for (const auto& r : json["regions"]) {
    regions_.emplace_back(r);
  }
  UTILS_DIE_IF(regions_.empty(), "Empty label region.");
}

// -----------------------------------------------------------------------------
nlohmann::json Label::ToJson() const {
  nlohmann::json ret;
  ret["label_ID"] = label_id_;
  for (const auto& r : regions_) {
    ret["regions"].push_back(r.ToJson());
  }
  return ret;
}

// -----------------------------------------------------------------------------
void Label::AddRegion(Region r) { regions_.emplace_back(std::move(r)); }

// -----------------------------------------------------------------------------
const std::string& Label::GetId() const { return label_id_; }

// -----------------------------------------------------------------------------
const std::vector<Region>& Label::GetRegions() const { return regions_; }

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
