/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/meta/dataset.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "block_header/disabled.h"
#include "block_header/enabled.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::core::meta {

// -----------------------------------------------------------------------------
Dataset::Dataset(const uint32_t version,
                 std::unique_ptr<BlockHeader> header_cfg, std::string meta,
                 std::string protection)
    : version_(version),
      header_cfg_(std::move(header_cfg)),
      dt_metadata_value_(std::move(meta)),
      dt_protection_value_(std::move(protection)) {}

// -----------------------------------------------------------------------------
Dataset::Dataset(const nlohmann::json& json)
    : version_(json["version"]),
      dt_metadata_value_(json["DT_metadata_value"]),
      dt_protection_value_(json["DT_protection_value"]) {
  // Optional: dataset group metadata
  if (json.contains("dataset_group")) {
    dataset_group_ = DatasetGroup(json["dataset_group"]);
  }
  // Optional: reference metadata (aligned data only)
  if (json.contains("reference")) {
    reference_ = Reference(json["reference"]);
  }
  // Optional: labels
  if (json.contains("label_list")) {
    for (const auto& l : json["label_list"]) {
      label_list_.emplace_back(l);
    }
  }
  // Presence of MIT and CC_mode flags
  // signals blockheader_flag == true  // NOLINT
  if (json.contains("MIT_flag") && json.contains("CC_mode_flag") &&
      !json.contains("ordered_blocks_flag")) {
    header_cfg_ = std::make_unique<block_header::Enabled>(json);
  } else if (!json.contains("MIT_flag") && !json.contains("CC_mode_flag") &&
             json.contains("ordered_blocks_flag")) {
    header_cfg_ = std::make_unique<block_header::Disabled>(json);
  } else {
    UTILS_DIE("Could not infer block header mode.");
  }
  // Optional: Access unit specific metadata
  if (json.contains("access_units")) {
    for (const auto& au : json["access_units"]) {
      access_units_.emplace_back(au);
    }
  }
  // Optional: descriptor stream metadata
  if (json.contains("descriptor_streams")) {
    for (const auto& ds : json["descriptor_streams"]) {
      descriptor_streams_.emplace_back(ds);
    }
  }
}

// -----------------------------------------------------------------------------
nlohmann::json Dataset::ToJson() const {
  nlohmann::json ret;
  if (dataset_group_) {
    ret["dataset_group"] = dataset_group_->ToJson();
  }
  if (reference_) {
    ret["reference"] = reference_->ToJson();
  }
  for (const auto& l : label_list_) {
    ret["label_list"].push_back(l.ToJson());
  }
  ret["version"] = version_;
  auto tmp = header_cfg_->ToJson();
  for (const auto& i : tmp.items()) {
    ret[i.key()] = i.value();
  }
  ret["DT_metadata_value"] = dt_metadata_value_;
  ret["DT_protection_value"] = dt_protection_value_;
  for (const auto& au : access_units_) {
    ret["access_units"].push_back(au.ToJson());
  }
  for (const auto& ds : descriptor_streams_) {
    ret["descriptor_streams"].push_back(ds.ToJson());
  }
  return ret;
}

// -----------------------------------------------------------------------------
const std::optional<DatasetGroup>& Dataset::GetDataGroup() const {
  return dataset_group_;
}

// -----------------------------------------------------------------------------
const std::optional<Reference>& Dataset::GetReference() const {
  return reference_;
}

// -----------------------------------------------------------------------------
const std::vector<Label>& Dataset::GetLabels() const { return label_list_; }

// -----------------------------------------------------------------------------
uint32_t Dataset::GetVersion() const { return version_; }

// -----------------------------------------------------------------------------
const BlockHeader& Dataset::GetHeader() const { return *header_cfg_; }

// -----------------------------------------------------------------------------
const std::string& Dataset::GetInformation() const {
  return dt_metadata_value_;
}

// -----------------------------------------------------------------------------
const std::string& Dataset::GetProtection() const {
  return dt_protection_value_;
}

// -----------------------------------------------------------------------------
const std::vector<AccessUnit>& Dataset::GetAUs() const { return access_units_; }

// -----------------------------------------------------------------------------
const std::vector<DescriptorStream>& Dataset::GetDSs() const {
  return descriptor_streams_;
}

// -----------------------------------------------------------------------------
void Dataset::SetDataGroup(DatasetGroup dg) { dataset_group_ = std::move(dg); }

// -----------------------------------------------------------------------------
void Dataset::SetReference(Reference ref) { reference_ = std::move(ref); }

// -----------------------------------------------------------------------------
void Dataset::AddLabel(Label l) { label_list_.emplace_back(std::move(l)); }

// -----------------------------------------------------------------------------
void Dataset::AddAccessUnit(AccessUnit au) {
  access_units_.emplace_back(std::move(au));
}

// -----------------------------------------------------------------------------
void Dataset::AddDescriptorStream(DescriptorStream ds) {
  descriptor_streams_.emplace_back(std::move(ds));
}

// -----------------------------------------------------------------------------
void Dataset::SetMetadata(std::string meta) {
  dt_metadata_value_ = std::move(meta);
}

// -----------------------------------------------------------------------------
void Dataset::SetProtection(std::string protection) {
  dt_protection_value_ = std::move(protection);
}

// -----------------------------------------------------------------------------
Dataset::Dataset() {
  version_ = 0;
  header_cfg_ = std::make_unique<block_header::Enabled>(false, false);
}

// -----------------------------------------------------------------------------
void Dataset::SetHeader(std::unique_ptr<BlockHeader> hdr) {
  header_cfg_ = std::move(hdr);
}

// -----------------------------------------------------------------------------
std::optional<DatasetGroup>& Dataset::GetDataGroup() { return dataset_group_; }

// -----------------------------------------------------------------------------
std::optional<Reference>& Dataset::GetReference() { return reference_; }

// -----------------------------------------------------------------------------
std::vector<Label>& Dataset::GetLabels() { return label_list_; }

// -----------------------------------------------------------------------------
std::string& Dataset::GetInformation() { return dt_metadata_value_; }

// -----------------------------------------------------------------------------
std::string& Dataset::GetProtection() { return dt_protection_value_; }

// -----------------------------------------------------------------------------
std::vector<AccessUnit>& Dataset::GetAUs() { return access_units_; }

// -----------------------------------------------------------------------------
std::vector<DescriptorStream>& Dataset::GetDSs() { return descriptor_streams_; }

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
