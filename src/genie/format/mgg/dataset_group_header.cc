/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/dataset_group_header.h"

#include <string>
#include <vector>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------

bool DatasetGroupHeader::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const DatasetGroupHeader&>(info);
  return id_ == other.id_ && version_ == other.version_ &&
         dataset_i_ds_ == other.dataset_i_ds_;
}

// -----------------------------------------------------------------------------

DatasetGroupHeader::DatasetGroupHeader() : DatasetGroupHeader(0, 0) {}

// -----------------------------------------------------------------------------

DatasetGroupHeader::DatasetGroupHeader(const uint8_t id,
                                       const uint8_t version)
    : id_(id), version_(version) {}

// -----------------------------------------------------------------------------

const std::string& DatasetGroupHeader::GetKey() const {
  static const std::string key = "dghd";
  return key;
}

// -----------------------------------------------------------------------------

DatasetGroupHeader::DatasetGroupHeader(util::BitReader& reader) {
  const auto start_pos = reader.GetStreamPosition() - 4;
  const auto length = reader.ReadAlignedInt<uint64_t>();
  const auto num_datasets = (length - 14) / 2;
  dataset_i_ds_.resize(num_datasets);
  id_ = reader.ReadAlignedInt<uint8_t>();
  version_ = reader.ReadAlignedInt<uint8_t>();
  for (auto& d : dataset_i_ds_) {
    d = reader.ReadAlignedInt<uint16_t>();
  }

  UTILS_DIE_IF(start_pos + length != reader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------

uint8_t DatasetGroupHeader::GetId() const { return id_; }

// -----------------------------------------------------------------------------

void DatasetGroupHeader::SetId(const uint8_t id) { id_ = id; }

// -----------------------------------------------------------------------------

uint8_t DatasetGroupHeader::GetVersion() const { return version_; }

// -----------------------------------------------------------------------------

const std::vector<uint16_t>& DatasetGroupHeader::GetDatasetIDs() const {
  return dataset_i_ds_;
}

// -----------------------------------------------------------------------------

void DatasetGroupHeader::AddDatasetId(uint8_t id) {
  dataset_i_ds_.emplace_back(id);
}

// -----------------------------------------------------------------------------

void DatasetGroupHeader::BoxWrite(util::BitWriter& writer) const {
  writer.WriteAlignedInt<uint8_t>(id_);
  writer.WriteAlignedInt<uint8_t>(version_);
  for (auto& d : dataset_i_ds_) {
    writer.WriteAlignedInt<uint16_t>(d);
  }
}

// -----------------------------------------------------------------------------

void DatasetGroupHeader::PatchId(const uint8_t group_id) { id_ = group_id; }

// -----------------------------------------------------------------------------

void DatasetGroupHeader::PrintDebug(std::ostream& output, const uint8_t depth,
                                     const uint8_t max_depth) const {
  print_offset(output, depth, max_depth, "* Dataset Group Header");
  print_offset(output, depth + 1, max_depth, "ID: " + std::to_string(id_));
  print_offset(output, depth + 1, max_depth,
               "Version: " + std::to_string(version_));
  for (const auto& id : dataset_i_ds_) {
    print_offset(output, depth + 1, max_depth,
                 "Dataset ID: " + std::to_string(id));
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
