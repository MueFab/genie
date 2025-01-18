/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_metadata.h"

#include <string>
#include <utility>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------

const std::string& DatasetMetadata::GetKey() const {
  static const std::string key = "dtmd";
  return key;
}

// -----------------------------------------------------------------------------

DatasetMetadata::DatasetMetadata(util::BitReader& bitreader,
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
  dg_metatdata_value_.resize(metadata_length);
  bitreader.ReadAlignedBytes(dg_metatdata_value_.data(),
                             dg_metatdata_value_.length());
  UTILS_DIE_IF(start_pos + length != bitreader.GetStreamPosition(),
               "Invalid length");
  UTILS_DIE_IF(!bitreader.IsStreamGood(), "Invalid length");
}

// -----------------------------------------------------------------------------

DatasetMetadata::DatasetMetadata(const uint8_t dataset_group_id,
                                 const uint16_t dataset_id,
                                 std::string dg_metatdata_value,
                                 const core::MpegMinorVersion version)
    : version_(version),
      dataset_group_id_(dataset_group_id),
      dataset_id_(dataset_id),
      dg_metatdata_value_(std::move(dg_metatdata_value)) {}

// -----------------------------------------------------------------------------

void DatasetMetadata::BoxWrite(util::BitWriter& bit_writer) const {
  if (version_ != core::MpegMinorVersion::kV1900) {
    bit_writer.WriteAlignedInt(dataset_group_id_);
    bit_writer.WriteAlignedInt(dataset_id_);
  }
  bit_writer.WriteAlignedBytes(dg_metatdata_value_.data(),
                              dg_metatdata_value_.length());
}

// -----------------------------------------------------------------------------

uint8_t DatasetMetadata::GetDatasetGroupId() const { return dataset_group_id_; }

// -----------------------------------------------------------------------------

uint16_t DatasetMetadata::GetDatasetId() const { return dataset_id_; }

// -----------------------------------------------------------------------------

const std::string& DatasetMetadata::GetMetadata() const {
  return dg_metatdata_value_;
}

// -----------------------------------------------------------------------------

bool DatasetMetadata::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const DatasetMetadata&>(info);
  return version_ == other.version_ &&
         dataset_group_id_ == other.dataset_group_id_ &&
         dataset_id_ == other.dataset_id_ &&
         dg_metatdata_value_ == other.dg_metatdata_value_;
}

// -----------------------------------------------------------------------------

std::string DatasetMetadata::decapsulate() {
  return std::move(dg_metatdata_value_);
}

// -----------------------------------------------------------------------------

void DatasetMetadata::PatchId(const uint8_t group_id, const uint16_t set_id) {
  dataset_group_id_ = group_id;
  dataset_id_ = set_id;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
