/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/label.h"

#include <string>
#include <utility>
#include <vector>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------

Label::Label() = default;

// -----------------------------------------------------------------------------

Label::Label(std::string label_id) : label_id_(std::move(label_id)) {}

// -----------------------------------------------------------------------------

Label::Label(util::BitReader& reader) {
  const auto start_pos = reader.GetStreamPosition() - 4;
  const auto length = reader.ReadAlignedInt<uint64_t>();
  label_id_ = reader.ReadAlignedStringTerminated();
  const auto num_datasets = reader.Read<uint16_t>();

  for (size_t i = 0; i < num_datasets; ++i) {
    dataset_infos_.emplace_back(reader);
  }
  reader.FlushHeldBits();
  UTILS_DIE_IF(start_pos + length != reader.GetStreamPosition(),
               "Invalid length: start_pos " + std::to_string(start_pos) +
                   "; length " + std::to_string(length) +
                   "; position should be " +
                   std::to_string(start_pos + length) + "; position is " +
                   std::to_string(reader.GetStreamPosition()));
}

// -----------------------------------------------------------------------------

void Label::AddDataset(LabelDataset ds_info) {
  dataset_infos_.push_back(std::move(ds_info));
}

// -----------------------------------------------------------------------------

const std::string& Label::GetLabelId() const { return label_id_; }

// -----------------------------------------------------------------------------

void Label::BoxWrite(util::BitWriter& bit_writer) const {
  bit_writer.WriteAlignedBytes(label_id_.data(), label_id_.length());
  bit_writer.WriteAlignedInt('\0');

  // num_datasets u(16)
  bit_writer.WriteAlignedInt<uint16_t>(
      static_cast<uint16_t>(dataset_infos_.size()));

  // data encapsulated in Class dataset_info
  for (auto& ds_info : dataset_infos_) {
    ds_info.Write(bit_writer);
  }

  // aligned to byte
  bit_writer.FlushBits();
}

// -----------------------------------------------------------------------------

const std::string& Label::GetKey() const {
  static const std::string key = "lbll";  // NOLINT
  return key;
}

// -----------------------------------------------------------------------------

bool Label::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const Label&>(info);
  return label_id_ == other.label_id_ && dataset_infos_ == other.dataset_infos_;
}

// -----------------------------------------------------------------------------

const std::vector<LabelDataset>& Label::GetDatasets() const {
  return dataset_infos_;
}

// -----------------------------------------------------------------------------

core::meta::Label Label::Decapsulate(const uint16_t dataset) {  // NOLINT
  core::meta::Label ret(label_id_);
  for (auto& d : dataset_infos_) {
    auto regions = d.Decapsulate(dataset);
    for (auto& r : regions) {
      ret.AddRegion(std::move(r));
    }
  }
  return ret;
}

// -----------------------------------------------------------------------------

void Label::PrintDebug(std::ostream& output, const uint8_t depth,
                       const uint8_t max_depth) const {
  print_offset(output, depth, max_depth, "* Label");
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
