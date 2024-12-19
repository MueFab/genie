/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/meta/dataset_group.h"

#include <string>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::core::meta {

// -----------------------------------------------------------------------------
uint16_t DatasetGroup::GetId() const { return dataset_group_id_; }

// -----------------------------------------------------------------------------
uint8_t DatasetGroup::GetVersion() const { return version_number_; }

// -----------------------------------------------------------------------------
const std::string& DatasetGroup::GetInformation() const {
  return dg_metadata_value_;
}

// -----------------------------------------------------------------------------
const std::string& DatasetGroup::GetProtection() const {
  return dg_protection_value_;
}

// -----------------------------------------------------------------------------
DatasetGroup::DatasetGroup(const uint16_t id, const uint8_t version,
                           std::string meta, std::string protection)
    : dataset_group_id_(id),
      version_number_(version),
      dg_metadata_value_(std::move(meta)),
      dg_protection_value_(std::move(protection)) {}

// -----------------------------------------------------------------------------
DatasetGroup::DatasetGroup(const nlohmann::json& json)
    : dataset_group_id_(json["dataset_group_ID"]),
      version_number_(json["version_number"]),
      dg_metadata_value_(json["DG_metadata_value"]),
      dg_protection_value_(json["DG_protection_value"]) {}

// -----------------------------------------------------------------------------
nlohmann::json DatasetGroup::ToJson() const {
  nlohmann::json ret;
  ret["dataset_group_ID"] = dataset_group_id_;
  ret["version_number"] = version_number_;
  ret["DG_metadata_value"] = dg_metadata_value_;
  ret["DG_protection_value"] = dg_protection_value_;
  return ret;
}

// -----------------------------------------------------------------------------
void DatasetGroup::SetMetadata(std::string meta) {
  dg_metadata_value_ = std::move(meta);
}

// -----------------------------------------------------------------------------
void DatasetGroup::SetProtection(std::string prot) {
  dg_metadata_value_ = std::move(prot);
}

// -----------------------------------------------------------------------------
std::string& DatasetGroup::GetInformation() { return dg_metadata_value_; }

// -----------------------------------------------------------------------------
std::string& DatasetGroup::GetProtection() { return dg_protection_value_; }

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
