/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_BENCHMARK_IMPL_H_
#define SRC_GENIE_ENTROPY_GABAC_BENCHMARK_IMPL_H_

// -----------------------------------------------------------------------------

#include <algorithm>  // NOLINT
#include <cmath>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::gabac {

// -----------------------------------------------------------------------------

template <typename Type>
SearchSpace<Type>::SearchSpaceIterator::SearchSpaceIterator(Type value,
                                                            Type stride)
    : value_(value), stride_(stride) {}

// -----------------------------------------------------------------------------

template <typename Type>
template <typename Ret>
Ret SearchSpace<Type>::SearchSpaceIterator::Get() const {
  return static_cast<Ret>(value_);
}

// -----------------------------------------------------------------------------

template <typename Type>
typename SearchSpace<Type>::SearchSpaceIterator&
SearchSpace<Type>::SearchSpaceIterator::operator-=(int64_t idx) {
  value_ -= stride_ * idx;
  return *this;
}

// -----------------------------------------------------------------------------

template <typename Type>
typename SearchSpace<Type>::SearchSpaceIterator
SearchSpace<Type>::SearchSpaceIterator::operator+(int64_t idx) const {
  SearchSpaceIterator ret = *this;
  ret += idx;
  return ret;
}

// -----------------------------------------------------------------------------

template <typename Type>
typename SearchSpace<Type>::SearchSpaceIterator
SearchSpace<Type>::SearchSpaceIterator::operator-(int64_t idx) const {
  SearchSpaceIterator ret = *this;
  ret -= idx;
  return ret;
}

// -----------------------------------------------------------------------------

template <typename Type>
int64_t SearchSpace<Type>::SearchSpaceIterator::operator-(
    SearchSpaceIterator& other) const {
  return (value_ - other.value_) / stride_;
}

// -----------------------------------------------------------------------------

template <typename Type>
typename SearchSpace<Type>::SearchSpaceIterator&
SearchSpace<Type>::SearchSpaceIterator::operator++() {
  return *this += 1;
}

// -----------------------------------------------------------------------------

template <typename Type>
typename SearchSpace<Type>::SearchSpaceIterator&
SearchSpace<Type>::SearchSpaceIterator::operator--() {
  return *this -= 1;
}

// -----------------------------------------------------------------------------

template <typename Type>
bool SearchSpace<Type>::SearchSpaceIterator::operator==(
    const SearchSpaceIterator& other) const {
  return value_ == other.value_;
}

// -----------------------------------------------------------------------------

template <typename Type>
bool SearchSpace<Type>::SearchSpaceIterator::operator!=(
    const SearchSpaceIterator& other) const {
  return !(*this == other);  // NOLINT
}

// -----------------------------------------------------------------------------

template <typename Type>
typename SearchSpace<Type>::SearchSpaceIterator&
SearchSpace<Type>::SearchSpaceIterator::operator+=(int64_t idx) {
  value_ += stride_ * idx;
  return *this;
}

// -----------------------------------------------------------------------------

template <typename Type>
SearchSpace<Type>::SearchSpace() : SearchSpace(0, 0, 1) {}

// -----------------------------------------------------------------------------

template <typename Type>
SearchSpace<Type>::SearchSpace(Type min, Type max, Type stride)
    : min_(min), max_(max), stride_(stride) {
  UTILS_DIE_IF((max_ - min_) % stride_ != 0, "Invalid stride_");
}

// -----------------------------------------------------------------------------

template <typename Type>
typename SearchSpace<Type>::SearchSpaceIterator
SearchSpace<Type>::begin()  // NOLINT
    const {
  return SearchSpaceIterator(min_, stride_);
}

// -----------------------------------------------------------------------------

template <typename Type>
typename SearchSpace<Type>::SearchSpaceIterator SearchSpace<Type>::end()
    const {  // NOLINT
  return SearchSpaceIterator(max_ + stride_, stride_);
}

// -----------------------------------------------------------------------------

template <typename Type>
size_t SearchSpace<Type>::Size() const {
  return (max_ - min_) / stride_ + 1;
}

// -----------------------------------------------------------------------------

template <typename Type>
Type SearchSpace<Type>::GetIndex(size_t idx) const {
  return static_cast<Type>(min_ + stride_ * idx);
}

// -----------------------------------------------------------------------------

template <typename Type>
Type SearchSpace<Type>::Mutate(const size_t index, float random) {
  float value = GetIndex(index);
  random = random * (max_ - min_);
  value += random;
  while (value < min_) {
    value = max_ - (min_ - value);
  }
  while (value > max_) {
    value = min_ + (value - max_);
  }
  return Type(std::round((value - min_) / static_cast<float>(stride_)));
}

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_BENCHMARK_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------