/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/au_protection.h"

#include <string>
#include <utility>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------

const std::string& AuProtection::GetKey() const {
  static const std::string key = "aupr";
  return key;
}

// -----------------------------------------------------------------------------

AuProtection::AuProtection(util::BitReader& bitreader,
                           const core::MpegMinorVersion version)
    : version_(version) {
  const auto start_pos = bitreader.GetStreamPosition() - 4;
  const auto length = bitreader.ReadAlignedInt<uint64_t>();
  auto metadata_length = length - GetHeaderLength();
  if (version_ != core::MpegMinorVersion::kV1900) {
    dataset_group_id_ = bitreader.ReadAlignedInt<uint8_t>();
    dataset_id_ = bitreader.ReadAlignedInt<uint16_t>();
    metadata_length -= sizeof(uint8_t);
    metadata_length -= sizeof(uint16_t);
  }
  au_protection_value_.resize(metadata_length);
  bitreader.ReadAlignedBytes(au_protection_value_.data(),
                             au_protection_value_.length());
  UTILS_DIE_IF(start_pos + length != bitreader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------

AuProtection::AuProtection(const uint8_t dataset_group_id,
                           const uint16_t dataset_id,
                           std::string au_protection_value,
                           const core::MpegMinorVersion version)
    : version_(version),
      dataset_group_id_(dataset_group_id),
      dataset_id_(dataset_id),
      au_protection_value_(std::move(au_protection_value)) {}

// -----------------------------------------------------------------------------

void AuProtection::BoxWrite(util::BitWriter& bit_writer) const {
  if (version_ != core::MpegMinorVersion::kV1900) {
    bit_writer.WriteAlignedInt(dataset_group_id_);
    bit_writer.WriteAlignedInt(dataset_id_);
  }
  bit_writer.WriteAlignedBytes(au_protection_value_.data(),
                              au_protection_value_.length());
}

// -----------------------------------------------------------------------------

uint8_t AuProtection::GetDatasetGroupId() const { return dataset_group_id_; }

// -----------------------------------------------------------------------------

uint16_t AuProtection::GetDatasetId() const { return dataset_id_; }

// -----------------------------------------------------------------------------

const std::string& AuProtection::GetInformation() const {
  return au_protection_value_;
}

// -----------------------------------------------------------------------------

bool AuProtection::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const AuProtection&>(info);
  return version_ == other.version_ &&
         dataset_group_id_ == other.dataset_group_id_ &&
         dataset_group_id_ == other.dataset_group_id_ &&
         dataset_id_ == other.dataset_id_ &&
         au_protection_value_ == other.au_protection_value_;
}

// -----------------------------------------------------------------------------

std::string AuProtection::decapsulate() {
  return std::move(au_protection_value_);
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
