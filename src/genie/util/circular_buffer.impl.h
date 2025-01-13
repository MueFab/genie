/**
 * Copyright 2018-2024 The Genie Authors.
 * @file circular_buffer.impl.h
 * @brief Implementation file for the CircularBuffer class template.
 *
 * This file provides the implementation of the `CircularBuffer` class template,
 * which is a fixed-size circular buffer that overwrites the oldest elements
 * when new elements are added.
 *
 * @details The `CircularBuffer` class template supports operations such as
 * accessing elements by index, retrieving the oldest and newest elements, and
 * adding new elements to the buffer. The buffer size is fixed upon
 * construction.
 *
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_CIRCULAR_BUFFER_IMPL_H_
#define SRC_GENIE_UTIL_CIRCULAR_BUFFER_IMPL_H_

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

template <typename T>
CircularBuffer<T>::CircularBuffer(size_t size, const T& val) : pos_(0) {
  data_.resize(size, val);
}

// -----------------------------------------------------------------------------

template <typename T>
T& CircularBuffer<T>::operator[](const size_t index) {
  return data_[(pos_ + index) % data_.size()];
}

// -----------------------------------------------------------------------------

template <typename T>
const T& CircularBuffer<T>::operator[](const size_t index) const {
  return data_[(pos_ + index) % data_.size()];
}

// -----------------------------------------------------------------------------

template <typename T>
T& CircularBuffer<T>::back() {
  return (*this)[pos_];
}

// -----------------------------------------------------------------------------

template <typename T>
[[nodiscard]] const T& CircularBuffer<T>::back() const {
  return (*this)[pos_];
}

// -----------------------------------------------------------------------------

template <typename T>
[[nodiscard]] T& CircularBuffer<T>::front() {
  return (*this)[(pos_ + data_.size() - 1) % data_.size()];
}

// -----------------------------------------------------------------------------

template <typename T>
[[nodiscard]] const T& CircularBuffer<T>::front() const {
  return (*this)[(pos_ + data_.size() - 1) % data_.size()];
}

// -----------------------------------------------------------------------------

template <typename T>
[[nodiscard]] size_t CircularBuffer<T>::size() const {
  return data_.size();
}

// -----------------------------------------------------------------------------

template <typename T>
T CircularBuffer<T>::push(const T& val) {
  T old_val = data_[pos_];
  data_[pos_] = val;
  pos_ = (pos_ + 1) % data_.size();
  return old_val;
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_CIRCULAR_BUFFER_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
