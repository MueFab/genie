/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/meta/access_unit.h"

#include <string>
#include <utility>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::core::meta {

// -----------------------------------------------------------------------------

AccessUnit::AccessUnit(const size_t id, std::string information,
                       std::string protection)
    : access_unit_id_(id),
      au_information_value_(std::move(information)),
      au_protection_value_(std::move(protection)) {
  UTILS_DIE_IF(au_information_value_.empty() && au_protection_value_.empty(),
               "Empty AU metadata");
}

// -----------------------------------------------------------------------------

AccessUnit::AccessUnit(const nlohmann::json& obj)
    : access_unit_id_(obj["access_unit_ID"]) {
  au_information_value_ = obj.at("AU_information_value");
  au_protection_value_ = obj.at("AU_protection_value");
  UTILS_DIE_IF(au_information_value_.empty() && au_protection_value_.empty(),
               "Empty AU metadata");
}

// -----------------------------------------------------------------------------

nlohmann::json AccessUnit::ToJson() const {
  nlohmann::json ret;
  ret["access_unit_ID"] = access_unit_id_;
  ret["AU_information_value"] = au_information_value_;
  ret["AU_protection_value"] = au_protection_value_;
  return ret;
}

// -----------------------------------------------------------------------------

size_t AccessUnit::GetId() const { return access_unit_id_; }

// -----------------------------------------------------------------------------

const std::string& AccessUnit::GetInformation() const {
  return au_information_value_;
}

// -----------------------------------------------------------------------------

const std::string& AccessUnit::GetProtection() const {
  return au_protection_value_;
}

// -----------------------------------------------------------------------------

AccessUnit::AccessUnit(const size_t id) : access_unit_id_(id) {}

// -----------------------------------------------------------------------------

void AccessUnit::SetInformation(std::string information) {
  au_information_value_ = std::move(information);
}

// -----------------------------------------------------------------------------

void AccessUnit::SetProtection(std::string protection) {
  au_protection_value_ = std::move(protection);
}

// -----------------------------------------------------------------------------

std::string& AccessUnit::GetInformation() { return au_information_value_; }

// -----------------------------------------------------------------------------

std::string& AccessUnit::GetProtection() { return au_protection_value_; }

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
