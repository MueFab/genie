/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/meta/internal_ref.h"

#include <memory>
#include <string>

// -----------------------------------------------------------------------------

namespace genie::core::meta {

// -----------------------------------------------------------------------------
uint16_t InternalRef::GetGroupId() const { return internal_dataset_group_id_; }

// -----------------------------------------------------------------------------
uint16_t InternalRef::GetId() const { return internal_dataset_id_; }

// -----------------------------------------------------------------------------
InternalRef::InternalRef(const nlohmann::json& json)
    : internal_dataset_group_id_(json["internal_dataset_group_id"]),
      internal_dataset_id_(json["internal_dataset_ID"]) {}

// -----------------------------------------------------------------------------
InternalRef::InternalRef(const uint16_t group_id, const uint16_t id)
    : internal_dataset_group_id_(group_id), internal_dataset_id_(id) {}

// -----------------------------------------------------------------------------
nlohmann::json InternalRef::ToJson() const {
  nlohmann::json ret;
  ret["internal_dataset_group_id"] = internal_dataset_group_id_;
  ret["internal_dataset_ID"] = internal_dataset_id_;
  return ret;
}

// -----------------------------------------------------------------------------
const std::string& InternalRef::GetKeyName() const {
  static const std::string ret = "internal_ref";
  return ret;
}

// -----------------------------------------------------------------------------
std::unique_ptr<RefBase> InternalRef::Clone() const {
  auto ret = std::make_unique<InternalRef>(internal_dataset_group_id_,
                                           internal_dataset_id_);
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
