/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/label_region.h"

#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------
LabelRegion::LabelRegion(const uint16_t seq_id, const uint64_t start_pos,
                         const uint64_t end_pos)
    : seq_id_(seq_id), start_pos_(start_pos), end_pos_(end_pos) {}

// -----------------------------------------------------------------------------
LabelRegion::LabelRegion(util::BitReader& reader) {
  // seq_ID u(16)
  seq_id_ = reader.Read<uint16_t>();

  // num_classes u(8)
  const auto num_classes = reader.Read<uint8_t>(4);
  class_i_ds_.resize(num_classes);
  // for class_IDs[] u(4)
  for (auto& class_id : class_i_ds_) {
    class_id = reader.Read<core::record::ClassType>(4);
  }

  // start_pos u(40)
  start_pos_ = reader.Read<uint64_t>(40);

  // end_pos u(40)
  end_pos_ = reader.Read<uint64_t>(40);
}

// -----------------------------------------------------------------------------
void LabelRegion::AddClassId(const core::record::ClassType class_id) {
  class_i_ds_.push_back(class_id);
}

// -----------------------------------------------------------------------------
uint16_t LabelRegion::GetSeqId() const { return seq_id_; }

// -----------------------------------------------------------------------------
const std::vector<core::record::ClassType>& LabelRegion::GetClassIDs() const {
  return class_i_ds_;
}

// -----------------------------------------------------------------------------
uint64_t LabelRegion::GetStartPos() const { return start_pos_; }

// -----------------------------------------------------------------------------
uint64_t LabelRegion::GetEndPos() const { return end_pos_; }

// -----------------------------------------------------------------------------
void LabelRegion::Write(util::BitWriter& bit_writer) const {
  // seq_ID u(16)
  bit_writer.WriteBits(seq_id_, 16);

  // num_classes u(8)
  bit_writer.WriteBits(class_i_ds_.size(), 4);

  // for class_IDs[] u(4)
  for (auto& class_id : class_i_ds_) {
    bit_writer.WriteBits(static_cast<uint8_t>(class_id), 4);
  }

  // start_pos u(40)
  bit_writer.WriteBits(start_pos_, 40);

  // end_pos u(40)
  bit_writer.WriteBits(end_pos_, 40);
}

// -----------------------------------------------------------------------------
bool LabelRegion::operator==(const LabelRegion& other) const {
  return seq_id_ == other.seq_id_ && class_i_ds_ == other.class_i_ds_ &&
         start_pos_ == other.start_pos_ && end_pos_ == other.end_pos_;
}

// -----------------------------------------------------------------------------
uint64_t LabelRegion::SizeInBits() const {
  return sizeof(uint16_t) * 8 + 4 + 4 * class_i_ds_.size() + 40 + 40;
}

// -----------------------------------------------------------------------------
core::meta::Region LabelRegion::Decapsulate() {
  core::meta::Region ret(seq_id_, start_pos_, end_pos_, std::move(class_i_ds_));
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
