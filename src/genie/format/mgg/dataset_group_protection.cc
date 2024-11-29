/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_group_protection.h"

#include <string>
#include <utility>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------
bool DatasetGroupProtection::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const DatasetGroupProtection&>(info);
  return version_ == other.version_ &&
         dataset_group_id_ == other.dataset_group_id_ &&
         dg_protection_value_ == other.dg_protection_value_;
}

// -----------------------------------------------------------------------------
const std::string& DatasetGroupProtection::GetKey() const {
  static const std::string key = "dgpr";
  return key;
}

// -----------------------------------------------------------------------------
DatasetGroupProtection::DatasetGroupProtection(
    util::BitReader& bitreader, const core::MpegMinorVersion version)
    : version_(version) {
  const auto start_pos = bitreader.GetStreamPosition() - 4;
  const auto length = bitreader.ReadAlignedInt<uint64_t>();
  auto protection_length = length - GetHeaderLength();
  if (version_ != core::MpegMinorVersion::kV1900) {
    dataset_group_id_ = bitreader.ReadAlignedInt<uint8_t>();
    protection_length -= sizeof(uint8_t);
  }
  dg_protection_value_.resize(protection_length);
  bitreader.ReadAlignedBytes(dg_protection_value_.data(),
                             dg_protection_value_.length());
  UTILS_DIE_IF(start_pos + length != bitreader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------
DatasetGroupProtection::DatasetGroupProtection(
    const uint8_t dataset_group_id, std::string dg_protection_value,
    const core::MpegMinorVersion version)
    : version_(version),
      dataset_group_id_(dataset_group_id),
      dg_protection_value_(std::move(dg_protection_value)) {}

// -----------------------------------------------------------------------------
void DatasetGroupProtection::BoxWrite(util::BitWriter& bit_writer) const {
  if (version_ != core::MpegMinorVersion::kV1900) {
    bit_writer.WriteAlignedInt(dataset_group_id_);
  }
  bit_writer.WriteAlignedBytes(dg_protection_value_.data(),
                              dg_protection_value_.length());
}

// -----------------------------------------------------------------------------
uint8_t DatasetGroupProtection::GetDatasetGroupId() const {
  return dataset_group_id_;
}

// -----------------------------------------------------------------------------
const std::string& DatasetGroupProtection::GetProtection() const {
  return dg_protection_value_;
}

// -----------------------------------------------------------------------------
std::string DatasetGroupProtection::decapsulate() {
  return std::move(dg_protection_value_);
}

// -----------------------------------------------------------------------------
void DatasetGroupProtection::PatchId(const uint8_t group_id) {
  dataset_group_id_ = group_id;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
