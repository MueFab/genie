/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/reference_manager.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------

const uint64_t ReferenceManager::chunk_size_ = 1 * 1024 * 1024;  //!

// -----------------------------------------------------------------------------

void ReferenceManager::Touch(const std::string& name, size_t num) {
  if (cache_info_.size() < cache_size_) {
    cache_info_.emplace_front(name, num);
    return;
  }

  for (auto it = cache_info_.begin(); it != cache_info_.end(); ++it) {
    if (it->first == name && it->second == num) {
      cache_info_.erase(it);
      cache_info_.emplace_front(name, num);
      return;
    }
  }

  cache_info_.emplace_front(name, num);
  const auto& line = data_[cache_info_.back().first][cache_info_.back().second];
  cache_info_.pop_back();

  line->chunk.reset();
  line->memory.reset();
}

// -----------------------------------------------------------------------------

void ReferenceManager::ValidateRefId(const size_t id) {
  std::unique_lock lock2(cache_info_lock_);
  for (size_t i = 0; i <= id; ++i) {
    auto s = std::to_string(i);
    data_[std::string(s.size() < 3 ? 3 - s.size() : 0, '0') + s];
  }
}

// -----------------------------------------------------------------------------

size_t ReferenceManager::Ref2Id(const std::string& ref) {
  std::unique_lock lock2(cache_info_lock_);
  for (const auto& [kFst, kSnd] : indices_) {
    if (kSnd == ref) {
      return kFst;
    }
  }
  UTILS_DIE("Unknown reference");
}

// -----------------------------------------------------------------------------

std::string ReferenceManager::Id2Ref(const size_t id) {
  std::unique_lock lock2(cache_info_lock_);
  const auto it = indices_.find(id);
  UTILS_DIE_IF(it == indices_.end(),
               "Unknown reference ID. Forgot to specify external reference?");
  return it->second;
}

// -----------------------------------------------------------------------------

bool ReferenceManager::RefKnown(const size_t id) {
  std::unique_lock lock2(cache_info_lock_);
  const auto it = indices_.find(id);
  return it != indices_.end();
}

// -----------------------------------------------------------------------------

size_t ReferenceManager::GetChunkSize() { return chunk_size_; }

// -----------------------------------------------------------------------------

bool ReferenceManager::ReferenceExcerpt::IsEmpty() const {
  return std::none_of(data_.begin(), data_.end(),
                      [&](const auto& p) { return IsMapped(p); });
}

// -----------------------------------------------------------------------------

void ReferenceManager::ReferenceExcerpt::Merge(ReferenceExcerpt& e) {
  if (this->ref_name_.empty()) {
    *this = std::move(e);
    return;
  }
  Extend(e.global_end_);
  for (size_t i = e.global_start_ / chunk_size_;
       i < (e.global_end_ - 1) / chunk_size_ + 1; i++) {
    if (e.IsMapped(i * chunk_size_)) {
      MapChunkAt(i * chunk_size_, e.GetChunkAt(i * chunk_size_));
    }
  }
}

// -----------------------------------------------------------------------------

size_t ReferenceManager::ReferenceExcerpt::GetGlobalStart() const {
  return global_start_;
}

// -----------------------------------------------------------------------------

size_t ReferenceManager::ReferenceExcerpt::GetGlobalEnd() const {
  return global_end_;
}

// -----------------------------------------------------------------------------

const std::string& ReferenceManager::ReferenceExcerpt::GetRefName() const {
  return ref_name_;
}

// -----------------------------------------------------------------------------

std::shared_ptr<const std::string>
ReferenceManager::ReferenceExcerpt::GetChunkAt(const size_t pos) const {
  const int id =
      static_cast<int>(pos / chunk_size_ - global_start_ / chunk_size_);
  UTILS_DIE_IF(id < 0 || id >= static_cast<int>(data_.size()), "Invalid index");
  return data_[id];
}

// -----------------------------------------------------------------------------

void ReferenceManager::ReferenceExcerpt::MapChunkAt(
    const size_t pos, std::shared_ptr<const std::string> dat) {
  const int id =
      static_cast<int>(pos / chunk_size_ - global_start_ / chunk_size_);
  UTILS_DIE_IF(id < 0 || id >= static_cast<int>(data_.size()), "Invalid index");
  data_[id] = std::move(dat);
}

// -----------------------------------------------------------------------------

ReferenceManager::ReferenceExcerpt::ReferenceExcerpt()
    : ReferenceExcerpt("", 0, 1) {}

// -----------------------------------------------------------------------------

ReferenceManager::ReferenceExcerpt::ReferenceExcerpt(std::string name,
                                                     const size_t start,
                                                     const size_t end)
    : ref_name_(std::move(name)),
      global_start_(start),
      global_end_(end),
      data_((global_end_ - 1) / chunk_size_ - global_start_ / chunk_size_ + 1,
            UndefPage()) {}

// -----------------------------------------------------------------------------

const std::shared_ptr<const std::string>&
ReferenceManager::ReferenceExcerpt::UndefPage() {
  static auto ret = std::make_shared<const std::string>(chunk_size_, 'N');
  return ret;
}

// -----------------------------------------------------------------------------

void ReferenceManager::ReferenceExcerpt::Extend(const size_t new_end) {
  if (new_end < global_end_) {
    return;
  }
  const size_t id = (new_end - 1) / chunk_size_;
  for (size_t i = data_.size() - 1; i < id; ++i) {
    data_.push_back(UndefPage());
  }
  global_end_ = new_end;
}

// -----------------------------------------------------------------------------

bool ReferenceManager::ReferenceExcerpt::IsMapped(
    const std::shared_ptr<const std::string>& page) {
  return page.get() != UndefPage().get();
}

// -----------------------------------------------------------------------------

bool ReferenceManager::ReferenceExcerpt::IsMapped(const size_t pos) const {
  return IsMapped(data_[(pos - (global_start_ - global_start_ % chunk_size_)) /
                        chunk_size_]);
}

// -----------------------------------------------------------------------------

ReferenceManager::ReferenceExcerpt::Stepper::Stepper(
    const ReferenceExcerpt& e) {
  start_vec_it_ = e.data_.begin();
  vec_it_ = e.data_.begin();
  end_vec_it_ = e.data_.end();
  string_pos_ = e.global_start_ % chunk_size_;
  cur_string_ = (*vec_it_)->data();
}

// -----------------------------------------------------------------------------

void ReferenceManager::ReferenceExcerpt::Stepper::Inc(const size_t off) {
  string_pos_ += off;
  while (string_pos_ >= chunk_size_) {
    string_pos_ -= chunk_size_;
    ++vec_it_;
  }
  if (vec_it_ != end_vec_it_) {
    cur_string_ = vec_it_->get()->data();
  }
}

// -----------------------------------------------------------------------------

bool ReferenceManager::ReferenceExcerpt::Stepper::end() const {
  return vec_it_ >= end_vec_it_;
}

// -----------------------------------------------------------------------------

char ReferenceManager::ReferenceExcerpt::Stepper::Get() const {
  return cur_string_[string_pos_];
}

// -----------------------------------------------------------------------------

ReferenceManager::ReferenceExcerpt::Stepper
ReferenceManager::ReferenceExcerpt::GetStepper() const {
  return Stepper(*this);
}

// -----------------------------------------------------------------------------

std::string ReferenceManager::ReferenceExcerpt::GetString(
    const size_t start, const size_t end) const {
  if (start < global_start_ || end > global_end_) {
    UTILS_DIE("String can't be bigger than reference excerpt");
  }
  auto stepper = GetStepper();
  std::string ret;
  stepper.Inc(start - global_start_);
  for (size_t i = start; i < end; ++i) {
    ret += stepper.Get();
    stepper.Inc();
  }
  return ret;
}

// -----------------------------------------------------------------------------

ReferenceManager::ReferenceManager(const size_t cache_size)
    : cache_size_(cache_size) {}

// -----------------------------------------------------------------------------

void ReferenceManager::AddRef(size_t index, std::unique_ptr<Reference> ref) {
  std::unique_lock lock2(cache_info_lock_);
  UTILS_DIE_IF(indices_.find(index) != indices_.end(),
               "Ref index already taken");
  indices_.insert(std::make_pair(index, ref->GetName()));
  const auto sequence = data_.find(ref->GetName());
  const size_t cur_chunks =
      sequence == data_.end() ? 0 : sequence->second.size();
  for (size_t i = cur_chunks; i < (ref->GetEnd() - 1) / chunk_size_ + 1; i++) {
    data_[ref->GetName()].push_back(std::make_unique<CacheLine>());
  }
  mgr_.RegisterRef(std::move(ref));
}

// -----------------------------------------------------------------------------

std::shared_ptr<const std::string> ReferenceManager::LoadAt(
    const std::string& name, const size_t pos) {
  std::unique_lock lock2(cache_info_lock_);
  const size_t id = pos / chunk_size_;
  const auto it = data_.find(name);

  // Invalid chunk
  if (it == data_.end()) {
    return ReferenceExcerpt::UndefPage();
  }

  auto& [chunk, memory, load_mutex] = *it->second[id];
  lock2.unlock();

  // Very important that lock2 is released, and we lock again in that specific
  // order, to avoid deadlocks.
  std::lock_guard lock1(load_mutex);
  lock2.lock();

  // Chunk already loaded
  auto ret = chunk;
  if (ret) {
    Touch(name, id);
    return ret;
  }

  // Try quick load. Maybe another thread unloaded this reference, but it is
  // still in memory, reachable via weak ptr.
  ret = memory.lock();
  if (ret) {
    chunk = ret;
    memory = ret;
    Touch(name, id);
    return ret;
  }

  // Reference is not in memory. We have to do a slow read from disc...
  // Loading mutex keeps locked for this chunk, but other chunks can be
  // accessed, main lock is opened.
  lock2.unlock();
  ret = std::make_shared<const std::string>(
      mgr_.GetSequence(name, id * chunk_size_, (id + 1) * chunk_size_));
  lock2.lock();

  chunk = ret;
  memory = ret;
  Touch(name, id);
  return ret;
}

// -----------------------------------------------------------------------------

ReferenceManager::ReferenceExcerpt ReferenceManager::Load(
    const std::string& name, const size_t start, const size_t end) {
  ReferenceExcerpt ret(name, start, end);
  for (size_t i = start / chunk_size_; i <= (end - 1) / chunk_size_; i++) {
    ret.MapChunkAt(i * chunk_size_, LoadAt(name, i * chunk_size_));
  }
  return ret;
}

// -----------------------------------------------------------------------------

std::vector<std::pair<size_t, size_t>> ReferenceManager::GetCoverage(
    const std::string& name) const {
  return mgr_.GetCoverage(name);
}

// -----------------------------------------------------------------------------

std::vector<std::string> ReferenceManager::GetSequences() const {
  std::vector<std::string> ret;
  ret.reserve(indices_.size());
  for (const auto& [kFst, kSnd] : indices_) {
    (void) kFst;
    ret.push_back(kSnd);
  }
  return ret;
}

// -----------------------------------------------------------------------------

size_t ReferenceManager::GetLength(const std::string& name) const {
  auto cov = GetCoverage(name);

  size_t ret = 0;
  for (const auto& [kFst, kSnd] : cov) {
    (void) kFst;
    ret = std::max(kSnd, ret);
  }
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
