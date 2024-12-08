/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_group_metadata.h"

#include <string>
#include <utility>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------
bool DatasetGroupMetadata::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const DatasetGroupMetadata&>(info);
  return version_ == other.version_ &&
         dataset_group_id_ == other.dataset_group_id_ &&
         dg_metadata_value_ == other.dg_metadata_value_;
}

// -----------------------------------------------------------------------------
const std::string& DatasetGroupMetadata::GetKey() const {
  static const std::string key = "dgmd";
  return key;
}

// -----------------------------------------------------------------------------
DatasetGroupMetadata::DatasetGroupMetadata(
    util::BitReader& bitreader, const core::MpegMinorVersion version)
    : version_(version) {
  const auto start_pos = bitreader.GetStreamPosition() - 4;
  const auto length = bitreader.ReadAlignedInt<uint64_t>();
  auto metadata_length = length - GetHeaderLength();
  if (version_ != core::MpegMinorVersion::kV1900) {
    dataset_group_id_ = bitreader.ReadAlignedInt<uint8_t>();
    metadata_length -= sizeof(uint8_t);
  }
  dg_metadata_value_.resize(metadata_length);
  bitreader.ReadAlignedBytes(dg_metadata_value_.data(),
                             dg_metadata_value_.length());
  UTILS_DIE_IF(start_pos + length != bitreader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------
DatasetGroupMetadata::DatasetGroupMetadata(
    const uint8_t dataset_group_id, std::string dg_metatdata_value,
    const core::MpegMinorVersion version)
    : version_(version),
      dataset_group_id_(dataset_group_id),
      dg_metadata_value_(std::move(dg_metatdata_value)) {}

// -----------------------------------------------------------------------------
void DatasetGroupMetadata::BoxWrite(util::BitWriter& bit_writer) const {
  if (version_ != core::MpegMinorVersion::kV1900) {
    bit_writer.WriteAlignedInt(dataset_group_id_);
  }
  bit_writer.WriteAlignedBytes(dg_metadata_value_.data(),
                              dg_metadata_value_.length());
}

// -----------------------------------------------------------------------------
uint8_t DatasetGroupMetadata::GetDatasetGroupId() const {
  return dataset_group_id_;
}

// -----------------------------------------------------------------------------
const std::string& DatasetGroupMetadata::GetMetadata() const {
  return dg_metadata_value_;
}

// -----------------------------------------------------------------------------
std::string DatasetGroupMetadata::decapsulate() {
  return std::move(dg_metadata_value_);
}

// -----------------------------------------------------------------------------
void DatasetGroupMetadata::PatchId(const uint8_t group_id) {
  dataset_group_id_ = group_id;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
