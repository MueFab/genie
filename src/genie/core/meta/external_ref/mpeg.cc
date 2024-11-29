/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/meta/external_ref/mpeg.h"

#include <memory>
#include <string>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::core::meta::external_ref {

// -----------------------------------------------------------------------------
Mpeg::Mpeg(std::string ref_uri, const ChecksumAlgorithm check,
           const uint16_t group_id, const uint16_t id, std::string checksum)
    : ExternalRef(std::move(ref_uri), check, ReferenceType::kMpeggRef),
      external_dataset_group_id_(group_id),
      external_dataset_id_(id),
      ref_checksum_(std::move(checksum)) {}

// -----------------------------------------------------------------------------
Mpeg::Mpeg(const nlohmann::json& json) : ExternalRef(json) {
  external_dataset_group_id_ =
      json["ref_type_mpegg_ref"]["external_dataset_group_id"];
  external_dataset_id_ = json["ref_type_mpegg_ref"]["external_dataset_id"];
  ref_checksum_ = json["ref_type_mpegg_ref"]["ref_checksum"];
}

// -----------------------------------------------------------------------------
nlohmann::json Mpeg::ToJson() const {
  auto ret = ExternalRef::ToJson();
  ret["ref_type_mpegg_ref"]["external_dataset_group_id"] =
      external_dataset_group_id_;
  ret["ref_type_mpegg_ref"]["external_dataset_id"] = external_dataset_id_;
  ret["ref_type_mpegg_ref"]["ref_checksum"] = ref_checksum_;
  return ret;
}

// -----------------------------------------------------------------------------
uint16_t Mpeg::GetGroupId() const { return external_dataset_group_id_; }

// -----------------------------------------------------------------------------
uint16_t Mpeg::GetId() const { return external_dataset_id_; }

// -----------------------------------------------------------------------------
const std::string& Mpeg::GetChecksum() const { return ref_checksum_; }

// -----------------------------------------------------------------------------
std::string& Mpeg::GetChecksum() { return ref_checksum_; }

// -----------------------------------------------------------------------------
std::unique_ptr<RefBase> Mpeg::Clone() const {
  auto ret = std::make_unique<Mpeg>(GetUri(), GetChecksumAlgo(),
                                    external_dataset_group_id_,
                                    external_dataset_id_, ref_checksum_);
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::meta::external_ref

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
