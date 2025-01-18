/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_protection.h"

#include <string>
#include <utility>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------

const std::string& DatasetProtection::GetKey() const {
  static const std::string key = "dtpr";
  return key;
}

// -----------------------------------------------------------------------------

DatasetProtection::DatasetProtection(util::BitReader& bitreader,
                                     const core::MpegMinorVersion version)
    : version_(version) {
  const auto start_pos = bitreader.GetStreamPosition();
  const auto length = bitreader.ReadAlignedInt<uint64_t>();
  auto protection_length = length - GetHeaderLength();
  if (version_ != core::MpegMinorVersion::kV1900) {
    dataset_group_id_ = bitreader.ReadAlignedInt<uint8_t>();
    dataset_id_ = bitreader.ReadAlignedInt<uint16_t>();
    protection_length -= sizeof(uint8_t);
    protection_length -= sizeof(uint16_t);
  }
  dg_protection_value_.resize(protection_length);
  bitreader.ReadAlignedBytes(dg_protection_value_.data(),
                             dg_protection_value_.length());
  UTILS_DIE_IF(start_pos + length != bitreader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------

DatasetProtection::DatasetProtection(const uint8_t dataset_group_id,
                                     const uint16_t dataset_id,
                                     std::string dg_protection_value,
                                     const core::MpegMinorVersion version)
    : version_(version),
      dataset_group_id_(dataset_group_id),
      dataset_id_(dataset_id),
      dg_protection_value_(std::move(dg_protection_value)) {}

// -----------------------------------------------------------------------------

void DatasetProtection::BoxWrite(util::BitWriter& bit_writer) const {
  if (version_ != core::MpegMinorVersion::kV1900) {
    bit_writer.WriteAlignedInt(dataset_group_id_);
    bit_writer.WriteAlignedInt(dataset_id_);
  }
  bit_writer.WriteAlignedBytes(dg_protection_value_.data(),
                              dg_protection_value_.length());
}

// -----------------------------------------------------------------------------

uint8_t DatasetProtection::GetDatasetGroupId() const {
  return dataset_group_id_;
}

// -----------------------------------------------------------------------------

uint16_t DatasetProtection::GetDatasetId() const { return dataset_id_; }

// -----------------------------------------------------------------------------

const std::string& DatasetProtection::GetProtection() const {
  return dg_protection_value_;
}

// -----------------------------------------------------------------------------

bool DatasetProtection::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const DatasetProtection&>(info);
  return version_ == other.version_ &&
         dataset_group_id_ == other.dataset_group_id_ &&
         dataset_id_ == other.dataset_id_ &&
         dg_protection_value_ == other.dg_protection_value_;
}

// -----------------------------------------------------------------------------

std::string DatasetProtection::Decapsulate() {
  return std::move(dg_protection_value_);
}

// -----------------------------------------------------------------------------

void DatasetProtection::PatchId(const uint8_t group_id, const uint16_t set_id) {
  dataset_group_id_ = group_id;
  dataset_id_ = set_id;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
