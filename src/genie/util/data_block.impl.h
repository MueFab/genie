/**
 * Copyright 2018-2024 The Genie Authors.
 * @file data_block.impl.h
 * @brief Implementation of the DataBlock class for managing data storage.
 *
 * This file contains the implementation of the DataBlock class, which provides
 * mechanisms for managing and manipulating data storage. It includes methods
 * for accessing, modifying, and iterating over data elements with different
 * word sizes.
 *
 * @details The DataBlock class supports operations such as getting and setting
 * data elements, resizing the data storage, and iterating over the data with
 * custom iterators. It ensures compatibility with different word sizes and
 * provides utility functions for common data manipulations.
 *
 * @tparam T The type of the data elements being managed by the DataBlock.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_DATA_BLOCK_IMPL_H_
#define SRC_GENIE_UTIL_DATA_BLOCK_IMPL_H_

// -----------------------------------------------------------------------------

#include <cstring>
#include <vector>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------
inline uint8_t DataBlock::GetWordSize() const { return 1 << lg_word_size_; }

// -----------------------------------------------------------------------------
inline uint64_t DataBlock::MulByWordSize(const uint64_t val) const {
  return val << lg_word_size_;
}

// -----------------------------------------------------------------------------
inline uint64_t DataBlock::DivByWordSize(const uint64_t val) const {
  return val >> lg_word_size_;
}

// -----------------------------------------------------------------------------
inline uint64_t DataBlock::ModByWordSize(const uint64_t val) const {
  return val & ((1 << lg_word_size_) - 1);
}

// -----------------------------------------------------------------------------
template <typename T>
size_t DataBlock::IteratorCore<T>::operator-(const IteratorCore& offset) const {
  return position_ - offset.position_;
}

// -----------------------------------------------------------------------------
inline size_t DataBlock::GetRawSize() const { return GetWordSize() * Size(); }

// -----------------------------------------------------------------------------
inline uint64_t DataBlock::Get(const size_t index) const {
  switch (lg_word_size_) {
    case 0:
      return *(data_.data() + index);
    case 1:
      return *reinterpret_cast<const uint16_t*>(data_.data() + (index << 1u));
    case 2:
      return *reinterpret_cast<const uint32_t*>(data_.data() + (index << 2u));
    case 3:
      return *reinterpret_cast<const uint64_t*>(data_.data() + (index << 3u));
    default:
      return 0;
  }
}

// -----------------------------------------------------------------------------
inline void DataBlock::Set(const size_t index, const uint64_t val) {
  switch (lg_word_size_) {
    case 0:
      *(data_.data() + index) = static_cast<uint8_t>(val);
      return;
    case 1:
      *reinterpret_cast<uint16_t*>(data_.data() + (index << 1u)) =
          static_cast<uint16_t>(val);
      return;
    case 2:
      *reinterpret_cast<uint32_t*>(data_.data() + (index << 2u)) =
          static_cast<uint32_t>(val);
      return;
    case 3:
      *reinterpret_cast<uint64_t*>(data_.data() + (index << 3u)) =
          static_cast<uint64_t>(val);
    default:
      break;
  }
}

// -----------------------------------------------------------------------------
inline DataBlock::ConstIterator DataBlock::begin() const { return {this, 0}; }

// -----------------------------------------------------------------------------
inline DataBlock::Iterator DataBlock::begin() { return {this, 0}; }

// -----------------------------------------------------------------------------
inline DataBlock::ConstIterator DataBlock::end() const {
  return {this, DivByWordSize(data_.size())};
}

// -----------------------------------------------------------------------------
inline DataBlock::Iterator DataBlock::end() {
  return {this, DivByWordSize(data_.size())};
}

// -----------------------------------------------------------------------------
inline void DataBlock::PushBack(const uint64_t val) {
  data_.resize(data_.size() + GetWordSize());
  switch (lg_word_size_) {
    case 0:
      *(data_.end() - 1) = static_cast<uint8_t>(val);
      return;
    case 1:
      *reinterpret_cast<uint16_t*>(&*(data_.end() - 2)) =
          static_cast<uint16_t>(val);
      return;
    case 2:
      *reinterpret_cast<uint32_t*>(&*(data_.end() - 4)) =
          static_cast<uint32_t>(val);
      return;
    case 3:
      *reinterpret_cast<uint64_t*>(&*(data_.end() - 8)) = val;
    default:
      break;
  }
}

// -----------------------------------------------------------------------------
[[maybe_unused]] inline void DataBlock::EmplaceBack(const uint64_t val) {
  PushBack(val);
}

// -----------------------------------------------------------------------------
inline const void* DataBlock::GetData() const { return data_.data(); }

// -----------------------------------------------------------------------------
inline void* DataBlock::GetData() { return data_.data(); }

// -----------------------------------------------------------------------------
inline void DataBlock::SetWordSize(const uint8_t size) {
  switch (size) {
    case 1:
      lg_word_size_ = 0;
      break;
    case 2:
      lg_word_size_ = 1;
      break;
    case 4:
      lg_word_size_ = 2;
      break;
    case 8:
      lg_word_size_ = 3;
      break;
    default:
      UTILS_DIE("Bad DataBlock word Size");
  }
  if (ModByWordSize(data_.size())) {
    UTILS_DIE("Bad DataBlock word Size");
  }
}

// -----------------------------------------------------------------------------
template <typename T>
DataBlock::ProxyCore<T>::ProxyCore(T str, size_t pos)
    : stream_(str), position_(pos) {}

// -----------------------------------------------------------------------------
template <typename T>
DataBlock::ProxyCore<T>::operator uint64_t() const {
  return stream_->Get(position_);
}

// -----------------------------------------------------------------------------
template <typename T>
DataBlock::ProxyCore<T>& DataBlock::ProxyCore<T>::operator=(uint64_t val) {
  stream_->Set(position_, val);
  return *this;
}

// -----------------------------------------------------------------------------
template <typename T>
DataBlock::IteratorCore<T>::IteratorCore(T str, size_t pos)
    : stream_(str), position_(pos) {}

// -----------------------------------------------------------------------------
template <typename T>
DataBlock::IteratorCore<T> DataBlock::IteratorCore<T>::operator+(
    const size_t offset) const {
  return IteratorCore(stream_, position_ + offset);
}

// -----------------------------------------------------------------------------
template <typename T>
DataBlock::IteratorCore<T> DataBlock::IteratorCore<T>::operator-(
    const size_t offset) const {
  return IteratorCore(stream_, position_ - offset);
}

// -----------------------------------------------------------------------------
template <typename T>
DataBlock::IteratorCore<T>& DataBlock::IteratorCore<T>::operator++() {
  *this = *this + 1;
  return *this;
}

// -----------------------------------------------------------------------------
template <typename T>
DataBlock::IteratorCore<T>& DataBlock::IteratorCore<T>::operator--() {
  *this = *this + 1;
  return *this;
}

// -----------------------------------------------------------------------------
template <typename T>
DataBlock::IteratorCore<T> DataBlock::IteratorCore<T>::operator++(int) {
  IteratorCore ret = *this;
  ++(*this);
  return ret;
}

// -----------------------------------------------------------------------------
template <typename T>
DataBlock::IteratorCore<T> DataBlock::IteratorCore<T>::operator--(int) {
  IteratorCore ret = *this;
  ++(*this);
  return ret;
}

// -----------------------------------------------------------------------------
template <typename T>
[[maybe_unused]] size_t DataBlock::IteratorCore<T>::GetOffset() const {
  return position_;
}

// -----------------------------------------------------------------------------
template <typename T>
DataBlock::ProxyCore<T> DataBlock::IteratorCore<T>::operator*() const {
  return DataBlock::ProxyCore<T>(stream_, position_);
}

// -----------------------------------------------------------------------------
template <typename T>
bool DataBlock::IteratorCore<T>::operator==(const IteratorCore& c) const {
  return this->stream_ == c.stream_ && this->position_ == c.position_;
}

// -----------------------------------------------------------------------------
template <typename T>
bool DataBlock::IteratorCore<T>::operator!=(const IteratorCore& c) const {
  return !(*this == c);  // NOLINT
}

// -----------------------------------------------------------------------------
template <typename It1, typename It2>
[[maybe_unused]] void DataBlock::Insert(const It1& pos, const It2& start,
                                        const It2& end) {
  if (pos.getStream() != this || start.getStream() != end.getStream()) {
    return;
  }
  data_.insert(data_.begin() + pos.getOffset(),
               start.getStream()->data.begin() + start.getOffset(),
               end.getStream()->data.begin() + end.getOffset());
}

// -----------------------------------------------------------------------------
template <typename T>
DataBlock::DataBlock(std::vector<T>* vec) : lg_word_size_(0) {
  SetWordSize(sizeof(T));
  const size_t kSize = vec->size() * sizeof(T);
  this->data_.resize(kSize);
  this->data_.shrink_to_fit();
  std::memcpy(this->data_.data(), vec->data(), kSize);
  vec->clear();
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_DATA_BLOCK_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
