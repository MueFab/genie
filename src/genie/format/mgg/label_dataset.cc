/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/label_dataset.h"

#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------

bool LabelDataset::operator==(const LabelDataset& other) const {
  return dataset_id_ == other.dataset_id_ &&
         dataset_regions_ == other.dataset_regions_;
}

// -----------------------------------------------------------------------------

LabelDataset::LabelDataset(const uint16_t ds_id) : dataset_id_(ds_id) {}

// -----------------------------------------------------------------------------

LabelDataset::LabelDataset(util::BitReader& reader) {
  // dataset_IDs u(16)
  dataset_id_ = reader.Read<uint16_t>();
  // num_regions u(8)
  const auto num_regions = reader.Read<uint8_t>();

  /// data encapsulated in Class dataset_region
  for (uint8_t i = 0; i < num_regions; ++i) {
    dataset_regions_.emplace_back(reader);
  }
}

// -----------------------------------------------------------------------------

void LabelDataset::AddDatasetRegion(LabelRegion ds_region) {
  dataset_regions_.emplace_back(std::move(ds_region));
}

// -----------------------------------------------------------------------------

uint16_t LabelDataset::GetDatasetId() const { return dataset_id_; }

// -----------------------------------------------------------------------------

uint64_t LabelDataset::GetBitLength() const {
  // dataset_IDs u(16)
  uint64_t bit_length = 16;

  // num_regions u(8)
  bit_length += 8;

  /// data encapsulated in Class dataset_region
  for (auto& ds_reg : dataset_regions_) {
    bit_length += ds_reg.SizeInBits();
  }

  return bit_length;
}

// -----------------------------------------------------------------------------

void LabelDataset::Write(util::BitWriter& bit_writer) const {
  // dataset_IDs u(16)
  bit_writer.WriteBits(dataset_id_, 16);

  // num_regions u(8)
  bit_writer.WriteBits(dataset_regions_.size(), 8);

  /// data encapsulated in Class dataset_region
  for (auto& ds_reg : dataset_regions_) {
    ds_reg.Write(bit_writer);
  }
}

// -----------------------------------------------------------------------------

std::vector<core::meta::Region> LabelDataset::Decapsulate(
    const uint16_t dataset) {
  std::vector<core::meta::Region> ret;
  if (dataset != dataset_id_) {
    return ret;
  }
  for (auto& r : dataset_regions_) {
    ret.emplace_back(r.Decapsulate());
  }
  dataset_regions_.clear();
  return ret;
}

// -----------------------------------------------------------------------------

LabelDataset::LabelDataset(const uint16_t dataset_id,
                           const core::meta::Label& labels)
    : dataset_id_(dataset_id) {
  for (auto& l : labels.GetRegions()) {
    dataset_regions_.emplace_back(l.GetSeqId(), l.GetStartPos(), l.GetEndPos());
    for (auto& c : l.GetClasses()) {
      dataset_regions_.back().AddClassId(c);
    }
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
