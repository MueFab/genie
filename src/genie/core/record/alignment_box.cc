/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/alignment_box.h"

#include <memory>
#include <utility>
#include <vector>

#include "genie/core/record/alignment_split/same_rec.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::core::record {

// -----------------------------------------------------------------------------

void AlignmentBox::Write(util::BitWriter& writer) const {
  writer.WriteAlignedInt<uint64_t, 5>(mapping_pos_);
  alignment_.Write(writer);
  for (const auto& a : split_alignment_info_) {
    a->Write(writer);
  }
}

// -----------------------------------------------------------------------------

AlignmentBox::AlignmentBox(const ClassType type, const uint8_t as_depth,
                           const bool extended_alignment,
                           const uint8_t number_of_template_segments,
                           util::BitReader& reader)
    : split_alignment_info_(
          type == ClassType::kClassHm ? 0 : number_of_template_segments - 1) {
  mapping_pos_ = reader.ReadAlignedInt<uint64_t, 5>();
  alignment_ = Alignment(as_depth, extended_alignment, reader);

  if (type == ClassType::kClassHm) {
    return;
  }
  for (size_t i_segment = 1; i_segment < number_of_template_segments;
       i_segment++) {
    split_alignment_info_[i_segment - 1] =
        AlignmentSplit::Factory(as_depth, extended_alignment, reader);
  }
}

//------------------------------------------------------------------------------

AlignmentBox::AlignmentBox() : split_alignment_info_(0) {}

// -----------------------------------------------------------------------------

void AlignmentBox::AddAlignmentSplit(
    std::unique_ptr<AlignmentSplit> alignment) {
  const auto status = alignment->IsExtendedAlignment();
  UTILS_DIE_IF(
      status.has_value() && alignment->IsExtendedAlignment() != status.value(),
      "Split alignment type incompatible: extended alignment status differs");
  if (alignment->GetType() == AlignmentSplit::Type::kSameRec) {
    UTILS_DIE_IF(dynamic_cast<alignment_split::SameRec&>(*alignment)
                         .GetAlignment()
                         .GetMappingScores()
                         .size() != alignment_.GetMappingScores().size(),
                 "AS depth incompatible");
  }
  split_alignment_info_.push_back(std::move(alignment));
}

// -----------------------------------------------------------------------------

uint8_t AlignmentBox::GetNumberOfTemplateSegments() const {
  return static_cast<uint8_t>(split_alignment_info_.size() + 1);
}

// -----------------------------------------------------------------------------

uint64_t AlignmentBox::GetPosition() const { return mapping_pos_; }

// -----------------------------------------------------------------------------

const Alignment& AlignmentBox::GetAlignment() const { return alignment_; }

// -----------------------------------------------------------------------------

const std::vector<std::unique_ptr<AlignmentSplit>>&
AlignmentBox::GetAlignmentSplits() const {
  return split_alignment_info_;
}

// -----------------------------------------------------------------------------

AlignmentBox::AlignmentBox(const uint64_t mapping_pos, Alignment&& alignment)
    : mapping_pos_(mapping_pos), alignment_(std::move(alignment)) {}

// -----------------------------------------------------------------------------

AlignmentBox::AlignmentBox(const AlignmentBox& container) { *this = container; }

// -----------------------------------------------------------------------------

AlignmentBox::AlignmentBox(AlignmentBox&& container) noexcept {
  *this = container;
}

// -----------------------------------------------------------------------------

AlignmentBox& AlignmentBox::operator=(const AlignmentBox& container) {
  if (this == &container) {
    return *this;
  }
  this->mapping_pos_ = container.mapping_pos_;
  this->alignment_ = container.alignment_;
  this->split_alignment_info_.clear();
  for (const auto& a : container.split_alignment_info_) {
    this->split_alignment_info_.emplace_back(a->clone());
  }
  return *this;
}

// -----------------------------------------------------------------------------

AlignmentBox& AlignmentBox::operator=(AlignmentBox&& container) noexcept {
  this->mapping_pos_ = container.mapping_pos_;
  this->alignment_ = container.alignment_;
  this->split_alignment_info_ = std::move(container.split_alignment_info_);
  return *this;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
