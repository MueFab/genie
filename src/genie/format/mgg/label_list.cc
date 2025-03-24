/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/label_list.h"

#include <string>
#include <utility>
#include <vector>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------

LabelList::LabelList(const uint8_t ds_group_id)
    : dataset_group_id_(ds_group_id) {}

// -----------------------------------------------------------------------------

LabelList::LabelList(util::BitReader& reader) {
  const auto start_pos = reader.GetStreamPosition() - 4;
  const auto length = reader.ReadAlignedInt<uint64_t>();
  // ID u(8)
  dataset_group_id_ = reader.ReadAlignedInt<uint8_t>();
  // num_labels u(16)
  const auto num_labels = reader.ReadAlignedInt<uint16_t>();

  for (size_t i = 0; i < num_labels; ++i) {
    ReadBox(reader, false);
  }
  UTILS_DIE_IF(start_pos + length != reader.GetStreamPosition(),
               "Invalid length");
}

// -----------------------------------------------------------------------------

uint8_t LabelList::GetDatasetGroupId() const { return dataset_group_id_; }

// -----------------------------------------------------------------------------

const std::vector<Label>& LabelList::GetLabels() const { return labels_; }

// -----------------------------------------------------------------------------

void LabelList::BoxWrite(util::BitWriter& bit_writer) const {
  bit_writer.WriteAlignedInt<uint8_t>(dataset_group_id_);
  bit_writer.WriteAlignedInt<uint16_t>(static_cast<uint16_t>(labels_.size()));
  // data encapsulated in Class Label
  for (auto& label : labels_) {
    label.Write(bit_writer);
  }
}

// -----------------------------------------------------------------------------

const std::string& LabelList::GetKey() const {
  static const std::string key = "labl";  // NOLINT
  return key;
}

// -----------------------------------------------------------------------------

bool LabelList::operator==(const GenInfo& info) const {
  if (!GenInfo::operator==(info)) {
    return false;
  }
  const auto& other = dynamic_cast<const LabelList&>(info);
  return dataset_group_id_ == other.dataset_group_id_ &&
         labels_ == other.labels_;
}

// -----------------------------------------------------------------------------

void LabelList::AddLabel(Label l) { labels_.emplace_back(std::move(l)); }

// -----------------------------------------------------------------------------

void LabelList::PatchId(const uint8_t group_id) {
  dataset_group_id_ = group_id;
}

// -----------------------------------------------------------------------------

std::vector<core::meta::Label> LabelList::Decapsulate(const uint16_t dataset) {
  std::vector<core::meta::Label> ret;
  ret.reserve(labels_.size());
  for (auto& l : labels_) {
    ret.emplace_back(l.Decapsulate(dataset));
  }
  labels_.clear();
  return ret;
}

// -----------------------------------------------------------------------------

void LabelList::PrintDebug(std::ostream& output, const uint8_t depth,
                            const uint8_t max_depth) const {
  print_offset(output, depth, max_depth, "* Label list");
  for (const auto& l : labels_) {
    l.PrintDebug(output, depth + 1, max_depth);
  }
}

// -----------------------------------------------------------------------------

void LabelList::ReadBox(util::BitReader& reader,  // NOLINT
                        const bool in_offset) {
  std::string tmp_str(4, '\0');
  reader.ReadAlignedBytes(tmp_str.data(), tmp_str.length());
  if (tmp_str == "lbll") {  // NOLINT
    labels_.emplace_back(reader);
  } else if (tmp_str == "offs") {
    UTILS_DIE_IF(in_offset, "Recursive offset not permitted");
    reader.ReadAlignedBytes(tmp_str.data(), tmp_str.length());
    const auto offset = reader.ReadAlignedInt<uint64_t>();
    if (offset == ~static_cast<uint64_t>(0)) {
      ReadBox(reader, in_offset);
      return;
    }
    const auto pos_save = reader.GetStreamPosition();
    reader.SetStreamPosition(static_cast<int64_t>(offset));
    ReadBox(reader, true);
    reader.SetStreamPosition(pos_save);
  } else {
    UTILS_DIE("Unknown box");
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
