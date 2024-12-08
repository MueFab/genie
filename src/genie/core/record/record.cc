/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/record.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/core/record/alignment_box.h"
#include "genie/core/record/alignment_external/none.h"
#include "genie/core/record/alignment_shared_data.h"
#include "genie/core/record/alignment_split/same_rec.h"
#include "genie/core/record/segment.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::core::record {

// -----------------------------------------------------------------------------
void Record::PatchRefId(const size_t ref_id) {
  shared_alignment_info_ = AlignmentSharedData(
      static_cast<uint16_t>(ref_id), shared_alignment_info_.GetAsDepth());
}

// -----------------------------------------------------------------------------
Record::Record() : alignment_info_(0), more_alignment_info_(nullptr) {}

// -----------------------------------------------------------------------------
Record::Record(const uint8_t number_of_template_segments,
               const ClassType au_type_cfg, std::string&& read_name,
               std::string&& read_group, const uint8_t flags,
               const bool is_read_1_first)
    : number_of_template_segments_(number_of_template_segments),
      alignment_info_(0),
      class_id_(au_type_cfg),
      read_group_(std::move(read_group)),
      read_1_first_(is_read_1_first),
      read_name_(std::move(read_name)),
      flags_(flags),
      more_alignment_info_(std::make_unique<alignment_external::None>()) {}

// -----------------------------------------------------------------------------
Record::Record(util::BitReader& reader)
    : number_of_template_segments_(reader.ReadAlignedInt<uint8_t>()),
      reads_(reader.ReadAlignedInt<uint8_t>()),
      alignment_info_(reader.ReadAlignedInt<uint16_t>()),
      class_id_(reader.ReadAlignedInt<ClassType>()),
      read_group_(reader.ReadAlignedInt<uint8_t>(), 0),
      read_1_first_(reader.ReadAlignedInt<uint8_t>()),
      shared_alignment_info_(!alignment_info_.empty()
                                 ? AlignmentSharedData(reader)
                                 : AlignmentSharedData()) {
  std::vector<uint32_t> read_sizes(reads_.size());
  for (auto& s : read_sizes) {
    s = reader.ReadAlignedInt<uint32_t, 3>();
  }
  qv_depth_ = reader.ReadAlignedInt<uint8_t>();
  read_name_.resize(reader.ReadAlignedInt<uint8_t>());
  reader.ReadAlignedBytes(&read_name_[0], read_name_.size());
  reader.ReadAlignedBytes(&read_group_[0], read_group_.size());

  size_t index = 0;
  for (auto& r : reads_) {
    r = Segment(read_sizes[index], qv_depth_, reader);
    ++index;
  }
  for (auto& a : alignment_info_) {
    a = AlignmentBox(class_id_, shared_alignment_info_.GetAsDepth(),
                     number_of_template_segments_, reader);
  }
  flags_ = reader.ReadAlignedInt<uint8_t>();
  more_alignment_info_ = AlignmentExternal::Factory(reader);
}

// -----------------------------------------------------------------------------
Record::Record(const Record& rec) { *this = rec; }

// -----------------------------------------------------------------------------
Record::Record(Record&& rec) noexcept { *this = std::move(rec); }

// -----------------------------------------------------------------------------
Record& Record::operator=(const Record& rec) {
  if (this == &rec) {
    return *this;
  }
  this->number_of_template_segments_ = rec.number_of_template_segments_;
  this->class_id_ = rec.class_id_;
  this->read_1_first_ = rec.read_1_first_;
  this->shared_alignment_info_ = rec.shared_alignment_info_;
  this->qv_depth_ = rec.qv_depth_;
  this->read_name_ = rec.read_name_;
  this->read_group_ = rec.read_group_;
  this->reads_ = rec.reads_;
  this->alignment_info_ = rec.alignment_info_;
  this->flags_ = rec.flags_;
  this->more_alignment_info_ = rec.more_alignment_info_->Clone();
  return *this;
}

// -----------------------------------------------------------------------------
Record& Record::operator=(Record&& rec) noexcept {
  this->number_of_template_segments_ = rec.number_of_template_segments_;
  this->class_id_ = rec.class_id_;
  this->read_1_first_ = rec.read_1_first_;
  this->shared_alignment_info_ = rec.shared_alignment_info_;
  this->qv_depth_ = rec.qv_depth_;
  this->read_name_ = std::move(rec.read_name_);
  this->read_group_ = std::move(rec.read_group_);
  this->reads_ = std::move(rec.reads_);
  this->alignment_info_ = std::move(rec.alignment_info_);
  this->flags_ = rec.flags_;
  this->more_alignment_info_ = std::move(rec.more_alignment_info_);
  return *this;
}

// -----------------------------------------------------------------------------
void Record::AddSegment(Segment&& rec) {
  UTILS_DIE_IF(reads_.size() == number_of_template_segments_,
               "Record already full");
  if (reads_.empty()) {
    qv_depth_ = static_cast<uint8_t>(rec.GetQualities().size());
  }
  UTILS_DIE_IF(!reads_.empty() && rec.GetQualities().size() != qv_depth_,
               "Incompatible qv depth");
  reads_.push_back(std::move(rec));
}

// -----------------------------------------------------------------------------
void Record::AddAlignment(const uint16_t seq_id, AlignmentBox&& rec) {
  if (alignment_info_.empty()) {
    shared_alignment_info_ = AlignmentSharedData(
        seq_id,
        static_cast<uint8_t>(rec.GetAlignment().GetMappingScores().size()));
  } else {
    UTILS_DIE_IF(rec.GetAlignment().GetMappingScores().size() !=
                     shared_alignment_info_.GetAsDepth(),
                 "Incompatible AS depth");
    UTILS_DIE_IF(
        rec.GetNumberOfTemplateSegments() != number_of_template_segments_,
        "Incompatible number_of_template_segments");
    UTILS_DIE_IF(seq_id != shared_alignment_info_.GetSeqId(),
                 "Incompatible seq id");
  }
  alignment_info_.push_back(std::move(rec));
}

// -----------------------------------------------------------------------------
const std::vector<Segment>& Record::GetSegments() const { return reads_; }

// -----------------------------------------------------------------------------
std::vector<Segment>& Record::GetSegments() { return reads_; }

// -----------------------------------------------------------------------------
void Record::SwapSegmentOrder() {
  if (reads_.size() != 2) {
    return;
  }
  auto s_tmp = std::move(reads_[0]);
  reads_[0] = std::move(reads_[1]);
  reads_[1] = std::move(s_tmp);
}

// -----------------------------------------------------------------------------
size_t Record::GetNumberOfTemplateSegments() const {
  return number_of_template_segments_;
}

// -----------------------------------------------------------------------------
const std::vector<AlignmentBox>& Record::GetAlignments() const {
  return alignment_info_;
}

// -----------------------------------------------------------------------------

void Record::Write(util::BitWriter& writer) const {
  writer.WriteAlignedInt(number_of_template_segments_);
  writer.WriteAlignedInt<uint8_t>(static_cast<uint8_t>(reads_.size()));
  writer.WriteAlignedInt<uint16_t>(
      static_cast<uint16_t>(alignment_info_.size()));
  writer.WriteAlignedInt(class_id_);
  writer.WriteAlignedInt<uint8_t>(static_cast<uint8_t>(read_group_.length()));
  writer.WriteAlignedInt(read_1_first_);
  if (!alignment_info_.empty()) {
    shared_alignment_info_.Write(writer);
  }
  for (const auto& a : reads_) {
    writer.WriteAlignedInt<uint32_t, 3>(
        static_cast<uint32_t>(a.GetSequence().length()));
  }
  writer.WriteAlignedInt(qv_depth_);
  writer.WriteAlignedInt<uint8_t>(static_cast<uint8_t>(read_name_.length()));
  writer.WriteAlignedBytes(read_name_.data(), read_name_.length());
  writer.WriteAlignedBytes(read_group_.data(), read_group_.length());
  for (const auto& r : reads_) {
    r.Write(writer);
  }
  for (const auto& a : alignment_info_) {
    a.Write(writer);
  }
  writer.WriteAlignedInt(flags_);
  more_alignment_info_->Write(writer);
}

// -----------------------------------------------------------------------------

uint8_t Record::GetFlags() const { return flags_; }

// -----------------------------------------------------------------------------

ClassType Record::GetClassId() const { return class_id_; }

// -----------------------------------------------------------------------------

const std::string& Record::GetName() const { return read_name_; }

// -----------------------------------------------------------------------------

bool Record::GetRead1First() const { return read_1_first_; }

// -----------------------------------------------------------------------------

const std::string& Record::GetGroup() const { return read_group_; }

// -----------------------------------------------------------------------------

const AlignmentSharedData& Record::GetAlignmentSharedData() const {
  return shared_alignment_info_;
}

// -----------------------------------------------------------------------------

const AlignmentExternal& Record::GetAlignmentExternal() const {
  return *more_alignment_info_;
}

// -----------------------------------------------------------------------------

void Record::SetQvDepth(const uint8_t depth) { qv_depth_ = depth; }

// -----------------------------------------------------------------------------

void Record::SetRead1First(const bool val) { this->read_1_first_ = val; }

// -----------------------------------------------------------------------------

void Record::SetName(const std::string& name) { read_name_ = name; }

// -----------------------------------------------------------------------------

void Record::SetClassType(const ClassType type) { this->class_id_ = type; }

// -----------------------------------------------------------------------------

bool Record::IsRead1First() const { return read_1_first_; }

// -----------------------------------------------------------------------------

uint64_t Record::GetLengthOfCigar(const std::string& cigar) {
  std::string digits;
  size_t length = 0;
  for (const auto& c : cigar) {
    if (isdigit(c)) {
      digits += c;
      continue;
    }
    if (GetAlphabetProperties(AlphabetId::kAcgtn).IsIncluded(c)) {
      length++;
      digits.clear();
      continue;
    }
    if (c == '=' || c == '-' || c == '*' || c == '/' || c == '%') {
      length += std::stoi(digits);
      digits.clear();
    } else {
      digits.clear();
    }
  }
  return length;
}

// -----------------------------------------------------------------------------

size_t Record::GetMappedLength(const size_t alignment,
                               const size_t split) const {
  if (split == 0) {
    return GetLengthOfCigar(
        GetAlignments()[alignment].GetAlignment().GetECigar());
  }
  const auto& s2 = dynamic_cast<alignment_split::SameRec&>(
      *GetAlignments()[alignment].GetAlignmentSplits()[split - 1]);
  return GetLengthOfCigar(s2.GetAlignment().GetECigar());
}

// -----------------------------------------------------------------------------

void Record::SetAlignment(const size_t id, AlignmentBox&& b) {
  this->alignment_info_[id] = std::move(b);
}

// -----------------------------------------------------------------------------

std::pair<size_t, size_t> Record::GetTemplatePosition() const {
  std::pair ret = {GetPosition(0, 0),
                   GetPosition(0, 0) + GetMappedLength(0, 0)};
  for (size_t i = 0; i < GetAlignments().front().GetAlignmentSplits().size();
       ++i) {
    auto pos = GetPosition(0, i);
    ret.first = std::min(ret.first, pos);
    ret.second = std::max(ret.second, pos + GetMappedLength(0, i));
  }
  return ret;
}

// -----------------------------------------------------------------------------

size_t Record::GetPosition(const size_t alignment, const size_t split) const {
  if (split == 0) {
    return GetAlignments()[alignment].GetPosition();
  }
  const auto& s2 = dynamic_cast<alignment_split::SameRec&>(
      *GetAlignments()[alignment].GetAlignmentSplits()[split - 1]);
  return GetAlignments()[alignment].GetPosition() + s2.GetDelta();
}

// -----------------------------------------------------------------------------

void Record::SetMoreAlignmentInfo(
    std::unique_ptr<AlignmentExternal> more_alignment_info) {
  more_alignment_info_ = std::move(more_alignment_info);
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
