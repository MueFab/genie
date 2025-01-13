/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/access_unit.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "genie/core/mismatch_decoder.h"

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------

AccessUnit::Subsequence& AccessUnit::Subsequence::operator=(
    const Subsequence& sub) {
  data_ = sub.data_;
  position_ = sub.position_;
  id_ = sub.id_;
  num_symbols_ = sub.num_symbols_;
  dependency_ = sub.dependency_;
  if (sub.mm_decoder_ != nullptr) mm_decoder_ = sub.mm_decoder_->Copy();
  return *this;
}

// -----------------------------------------------------------------------------

AccessUnit::Subsequence& AccessUnit::Subsequence::operator=(
    Subsequence&& sub) noexcept {
  data_ = std::move(sub.data_);
  position_ = sub.position_;
  id_ = sub.id_;
  num_symbols_ = sub.num_symbols_;
  dependency_ = sub.dependency_;
  mm_decoder_ = std::move(sub.mm_decoder_);
  return *this;
}

// -----------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(const Subsequence& sub) { *this = sub; }

// -----------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(Subsequence&& sub) noexcept {
  *this = std::move(sub);
}

// -----------------------------------------------------------------------------

util::DataBlock* AccessUnit::Subsequence::GetDependency() {
  if (id_ == gen_sub::kMismatchTypeSubstBase ||
      id_ == gen_sub::kRefTransTransform)
    return &dependency_;
  return nullptr;
}

// -----------------------------------------------------------------------------

AccessUnit::Subsequence AccessUnit::Subsequence::AttachMismatchDecoder(
    std::unique_ptr<MismatchDecoder> mm) {
  mm_decoder_ = std::move(mm);
  return *this;
}

// -----------------------------------------------------------------------------

MismatchDecoder* AccessUnit::Subsequence::GetMismatchDecoder() const {
  return mm_decoder_.get();
}

// -----------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(const uint8_t word_size, GenSubIndex id)
    : data_(0, word_size), id_(std::move(id)), dependency_(0, word_size) {}

// -----------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(util::DataBlock d, GenSubIndex id)
    : id_(std::move(id)), dependency_(0, d.GetWordSize()) {
  data_ = std::move(d);
}

// -----------------------------------------------------------------------------

void AccessUnit::Subsequence::Push(const uint64_t val) { data_.PushBack(val); }

// -----------------------------------------------------------------------------

void AccessUnit::Subsequence::PushDependency(const uint64_t val) {
  dependency_.PushBack(val);
}

// -----------------------------------------------------------------------------

void AccessUnit::Subsequence::Inc() { position_++; }

// -----------------------------------------------------------------------------

uint64_t AccessUnit::Subsequence::Get(const size_t lookahead) const {
  return data_.Get(position_ + lookahead);
}

// -----------------------------------------------------------------------------

bool AccessUnit::Subsequence::end() const { return data_.Size() <= position_; }

// -----------------------------------------------------------------------------

util::DataBlock&& AccessUnit::Subsequence::Move() { return std::move(data_); }

// -----------------------------------------------------------------------------

GenSubIndex AccessUnit::Subsequence::GetId() const { return id_; }

// -----------------------------------------------------------------------------

size_t AccessUnit::Subsequence::GetNumSymbols() const { return data_.Size(); }

// -----------------------------------------------------------------------------

uint64_t AccessUnit::Subsequence::Pull() {
  if (end()) {
    UTILS_DIE("Tried to read descriptor that has already ended: " +
              std::to_string(static_cast<uint8_t>(this->GetId().first)) + ", " +
              std::to_string(this->GetId().second));
  }
  return data_.Get(position_++);
}

// -----------------------------------------------------------------------------

AccessUnit::Subsequence& AccessUnit::Descriptor::Get(const uint16_t sub) {
  return subdesc_[sub];
}

// -----------------------------------------------------------------------------

const AccessUnit::Subsequence& AccessUnit::Descriptor::Get(
    const uint16_t sub) const {
  return subdesc_[sub];
}

// -----------------------------------------------------------------------------

AccessUnit::Subsequence& AccessUnit::Descriptor::GetTokenType(
    const uint16_t pos, const uint8_t type) {
  const uint16_t s_id = (pos << 4u) | (type & 0xfu);  // NOLINT
  while (subdesc_.size() <= s_id) {                   // NOLINT
    subdesc_.emplace_back(
        4_u8, GenSubIndex(GetId(), static_cast<uint16_t>(subdesc_.size())));
  }
  return Get(s_id);
}

// -----------------------------------------------------------------------------

GenDesc AccessUnit::Descriptor::GetId() const { return id_; }

// -----------------------------------------------------------------------------

void AccessUnit::Descriptor::Add(Subsequence&& sub) {
  subdesc_.push_back(std::move(sub));
}

// -----------------------------------------------------------------------------

void AccessUnit::Descriptor::Set(const uint16_t id, Subsequence&& sub) {
  subdesc_[id] = std::move(sub);
}

// -----------------------------------------------------------------------------

AccessUnit::Descriptor::Descriptor(const GenDesc id) : id_(id) {}

// -----------------------------------------------------------------------------

AccessUnit::Subsequence* AccessUnit::Descriptor::begin() {
  return &subdesc_.front();
}

// -----------------------------------------------------------------------------

AccessUnit::Subsequence* AccessUnit::Descriptor::end() {
  return &subdesc_.back() + 1;
}

// -----------------------------------------------------------------------------

const AccessUnit::Subsequence* AccessUnit::Descriptor::begin() const {
  return &subdesc_.front();
}

// -----------------------------------------------------------------------------

const AccessUnit::Subsequence* AccessUnit::Descriptor::end() const {
  return &subdesc_.back() + 1;
}

// -----------------------------------------------------------------------------

size_t AccessUnit::Descriptor::GetSize() const { return subdesc_.size(); }

// -----------------------------------------------------------------------------

AccessUnit::Subsequence& AccessUnit::Get(GenSubIndex sub) {
  return descriptors_[static_cast<uint8_t>(sub.first)].Get(sub.second);
}

// -----------------------------------------------------------------------------

AccessUnit::Descriptor& AccessUnit::Get(GenDesc desc) {
  return descriptors_[static_cast<uint8_t>(desc)];
}

// -----------------------------------------------------------------------------

const AccessUnit::Descriptor& AccessUnit::Get(GenDesc desc) const {
  return descriptors_[static_cast<uint8_t>(desc)];
}

// -----------------------------------------------------------------------------

const AccessUnit::Subsequence& AccessUnit::Get(GenSubIndex sub) const {
  return descriptors_[static_cast<uint8_t>(sub.first)].Get(sub.second);
}

// -----------------------------------------------------------------------------

void AccessUnit::Set(GenSubIndex sub, Subsequence&& data) {
  descriptors_[static_cast<uint8_t>(sub.first)].Set(sub.second,
                                                    std::move(data));
}

// -----------------------------------------------------------------------------

void AccessUnit::Set(GenDesc sub, Descriptor&& data) {
  descriptors_[static_cast<uint8_t>(sub)] = std::move(data);
}

// -----------------------------------------------------------------------------

void AccessUnit::Push(const GenSubIndex& sub, const uint64_t value) {
  Get(sub).Push(value);
}

// -----------------------------------------------------------------------------

void AccessUnit::PushDependency(const GenSubIndex& sub, const uint64_t value) {
  Get(sub).PushDependency(value);
}

// -----------------------------------------------------------------------------

bool AccessUnit::IsEnd(const GenSubIndex& sub) { return Get(sub).end(); }

// -----------------------------------------------------------------------------

uint64_t AccessUnit::Peek(const GenSubIndex& sub, const size_t lookahead) {
  return Get(sub).Get(lookahead);
}

// -----------------------------------------------------------------------------

uint64_t AccessUnit::Pull(const GenSubIndex& sub) { return Get(sub).Pull(); }

// -----------------------------------------------------------------------------

void AccessUnit::Subsequence::AnnotateNumSymbols(const size_t num) {
  num_symbols_ = num;
}

// -----------------------------------------------------------------------------

bool AccessUnit::Subsequence::IsEmpty() const { return !GetNumSymbols(); }

// -----------------------------------------------------------------------------

size_t AccessUnit::Subsequence::GetRawSize() const {
  return data_.GetRawSize();
}

// -----------------------------------------------------------------------------

void AccessUnit::Subsequence::Write(util::BitWriter& writer) const {
  writer.WriteAlignedBytes(data_.GetData(), data_.GetRawSize());
}

// -----------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(GenSubIndex id, const size_t size,
                                     util::BitReader& reader)
    : data_(0, 1), id_(std::move(id)), dependency_(0, 1) {
  data_.Resize(size);
  // no need to resize 'dependency' as it's not used on decoder side
  reader.ReadAlignedBytes(data_.GetData(), size);
}

// -----------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(GenSubIndex id)
    : data_(0, 1), id_(std::move(id)), dependency_(0, 1) {}

// -----------------------------------------------------------------------------

AccessUnit::Subsequence::Subsequence(GenSubIndex id, util::DataBlock&& dat)
    : data_(std::move(dat)),
      id_(std::move(id)),
      dependency_(0, data_.GetWordSize()) {}

// -----------------------------------------------------------------------------

void AccessUnit::Subsequence::Set(util::DataBlock&& dat) {
  data_ = std::move(dat);
}

// -----------------------------------------------------------------------------

void AccessUnit::Subsequence::SetPosition(const size_t pos) { position_ = pos; }

// -----------------------------------------------------------------------------

util::DataBlock& AccessUnit::Subsequence::GetData() { return data_; }

// -----------------------------------------------------------------------------

size_t AccessUnit::Descriptor::GetWrittenSize() const {
  const size_t overhead = GetDescriptor(GetId()).token_type
                              ? 0
                              : (subdesc_.size() - 1) * sizeof(uint32_t);
  return std::accumulate(
      subdesc_.begin(), subdesc_.end(), overhead,
      [](const size_t sum, const Subsequence& payload) {
        return payload.IsEmpty() ? sum : sum + payload.GetRawSize();
      });
}

// -----------------------------------------------------------------------------

void AccessUnit::Descriptor::Write(util::BitWriter& writer) const {
  if (this->id_ == GenDesc::kReadName ||
      this->id_ == GenDesc::kMultiSegmentAlignment) {
    subdesc_.front().Write(writer);
    return;
  }
  for (size_t i = 0; i < subdesc_.size(); ++i) {
    if (i < subdesc_.size() - 1) {
      writer.WriteBits(subdesc_[i].GetRawSize(), 32);
    }
    subdesc_[i].Write(writer);
  }
}

// -----------------------------------------------------------------------------

AccessUnit::Descriptor::Descriptor(GenDesc id, const size_t count,
                                   size_t remaining_size,
                                   util::BitReader& reader)
    : id_(id) {
  if (this->id_ == GenDesc::kReadName ||
      this->id_ == GenDesc::kMultiSegmentAlignment) {
    subdesc_.emplace_back(GenSubIndex{id, 0_u16}, remaining_size, reader);
    return;
  }
  for (size_t i = 0; i < count; ++i) {
    size_t s = 0;
    if (i < count - 1) {
      s = reader.Read<size_t>(32);
      remaining_size -= s + 4;
    } else {
      s = remaining_size;
    }
    if (s) {
      subdesc_.emplace_back(GenSubIndex{id, static_cast<uint16_t>(i)}, s,
                            reader);
    } else {
      subdesc_.emplace_back(GenSubIndex{id, static_cast<uint16_t>(i)},
                            util::DataBlock(0, 4));
    }
  }
}

// -----------------------------------------------------------------------------

bool AccessUnit::Descriptor::IsEmpty() const {
  return std::all_of(subdesc_.begin(), subdesc_.end(),
                     [](const auto& d) { return d.IsEmpty(); });
}

// -----------------------------------------------------------------------------

AccessUnit::Descriptor::Descriptor() : id_(static_cast<GenDesc>(0)) {}

// -----------------------------------------------------------------------------

AccessUnit::AccessUnit(parameter::EncodingSet&& set, const size_t num_records)
    : parameters_(std::move(set)),
      reference_only_(false),
      num_reads_(num_records),
      type_(record::ClassType::kClassU),
      min_pos_(0),
      max_pos_(0),
      reference_sequence_(0) {
  for (const auto& desc : GetDescriptors()) {
    Descriptor desc_data(desc.id);
    for (const auto& sub_descriptor : desc.sub_seqs) {
      const auto bytes = Range2Bytes(GetSubsequence(sub_descriptor.id).range);
      desc_data.Add(Subsequence(bytes, sub_descriptor.id));
    }
    descriptors_.push_back(std::move(desc_data));
  }
}

// -----------------------------------------------------------------------------

void AccessUnit::SetParameters(parameter::EncodingSet&& parameters) {
  parameters_ = std::move(parameters);
}

// -----------------------------------------------------------------------------

const parameter::EncodingSet& AccessUnit::GetParameters() const {
  return parameters_;
}

// -----------------------------------------------------------------------------

parameter::EncodingSet& AccessUnit::GetParameters() { return parameters_; }

// -----------------------------------------------------------------------------

parameter::EncodingSet&& AccessUnit::MoveParameters() {
  return std::move(parameters_);
}

// -----------------------------------------------------------------------------

size_t AccessUnit::GetNumReads() const { return num_reads_; }

// -----------------------------------------------------------------------------

void AccessUnit::Clear() { *this = AccessUnit(parameter::EncodingSet(), 0); }

// -----------------------------------------------------------------------------

void AccessUnit::AddRecord() { num_reads_++; }

// -----------------------------------------------------------------------------

void AccessUnit::SetReference(const uint16_t ref) { reference_sequence_ = ref; }

// -----------------------------------------------------------------------------

uint16_t AccessUnit::GetReference() const { return reference_sequence_; }

// -----------------------------------------------------------------------------

void AccessUnit::SetMaxPos(const uint64_t pos) { max_pos_ = pos; }

// -----------------------------------------------------------------------------

void AccessUnit::SetMinPos(const uint64_t pos) { min_pos_ = pos; }

// -----------------------------------------------------------------------------

uint64_t AccessUnit::GetMaxPos() const { return max_pos_; }

// -----------------------------------------------------------------------------

uint64_t AccessUnit::GetMinPos() const { return min_pos_; }

// -----------------------------------------------------------------------------

void AccessUnit::SetNumReads(const size_t recs) { num_reads_ = recs; }

// -----------------------------------------------------------------------------

record::ClassType AccessUnit::GetClassType() const { return type_; }

// -----------------------------------------------------------------------------

void AccessUnit::SetClassType(const record::ClassType type) { type_ = type; }

// -----------------------------------------------------------------------------

AccessUnit::Descriptor* AccessUnit::begin() { return &descriptors_.front(); }

// -----------------------------------------------------------------------------

AccessUnit::Descriptor* AccessUnit::end() { return &descriptors_.back() + 1; }

// -----------------------------------------------------------------------------

const AccessUnit::Descriptor* AccessUnit::begin() const {
  return &descriptors_.front();
}

// -----------------------------------------------------------------------------

const AccessUnit::Descriptor* AccessUnit::end() const {
  return &descriptors_.back() + 1;
}

// -----------------------------------------------------------------------------

stats::PerfStats& AccessUnit::GetStats() { return stats_; }

// -----------------------------------------------------------------------------

void AccessUnit::SetStats(stats::PerfStats&& stats) {
  stats_ = std::move(stats);
}

// -----------------------------------------------------------------------------

void AccessUnit::SetReference(
    const ReferenceManager::ReferenceExcerpt& ex,
    const std::vector<std::pair<size_t, size_t>>& ref2_write) {
  reference_ = ex;
  ref_to_write_ = ref2_write;
}

// -----------------------------------------------------------------------------

const ReferenceManager::ReferenceExcerpt& AccessUnit::GetReferenceExcerpt()
    const {
  return reference_;
}

// -----------------------------------------------------------------------------

const std::vector<std::pair<size_t, size_t>>& AccessUnit::GetRefToWrite()
    const {
  return ref_to_write_;
}

// -----------------------------------------------------------------------------

bool AccessUnit::IsReferenceOnly() const { return reference_only_; }

// -----------------------------------------------------------------------------

void AccessUnit::SetReferenceOnly(const bool ref) { reference_only_ = ref; }

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
