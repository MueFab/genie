/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_GABAC_BENCHMARK_IMPL_H_
#define SRC_GENIE_ENTROPY_GABAC_BENCHMARK_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <cmath>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::gabac {

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
SearchSpace<TYPE>::SearchSpaceIterator::SearchSpaceIterator(TYPE _value, TYPE _stride)
    : value(_value), stride(_stride) {}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
template <typename RET>
RET SearchSpace<TYPE>::SearchSpaceIterator::get() const {
    return static_cast<RET>(value);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
typename SearchSpace<TYPE>::SearchSpaceIterator& SearchSpace<TYPE>::SearchSpaceIterator::operator-=(int64_t idx) {
    value -= stride * idx;
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
typename SearchSpace<TYPE>::SearchSpaceIterator SearchSpace<TYPE>::SearchSpaceIterator::operator+(int64_t idx) const {
    SearchSpaceIterator ret = *this;
    ret += idx;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
typename SearchSpace<TYPE>::SearchSpaceIterator SearchSpace<TYPE>::SearchSpaceIterator::operator-(int64_t idx) const {
    SearchSpaceIterator ret = *this;
    ret -= idx;
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
int64_t SearchSpace<TYPE>::SearchSpaceIterator::operator-(SearchSpaceIterator& other) const {
    return (value - other.value) / stride;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
typename SearchSpace<TYPE>::SearchSpaceIterator& SearchSpace<TYPE>::SearchSpaceIterator::operator++() {
    return *this += 1;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
typename SearchSpace<TYPE>::SearchSpaceIterator& SearchSpace<TYPE>::SearchSpaceIterator::operator--() {
    return *this -= 1;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
bool SearchSpace<TYPE>::SearchSpaceIterator::operator==(const SearchSpaceIterator& other) const {
    return value == other.value;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
bool SearchSpace<TYPE>::SearchSpaceIterator::operator!=(const SearchSpaceIterator& other) const {
    return !(*this == other);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
typename SearchSpace<TYPE>::SearchSpaceIterator& SearchSpace<TYPE>::SearchSpaceIterator::operator+=(int64_t idx) {
    value += stride * idx;
    return *this;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
SearchSpace<TYPE>::SearchSpace() : SearchSpace(0, 0, 1) {}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
SearchSpace<TYPE>::SearchSpace(TYPE _min, TYPE _max, TYPE _stride) : min(_min), max(_max), stride(_stride) {
    UTILS_DIE_IF((max - min) % stride != 0, "Invalid stride");
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
typename SearchSpace<TYPE>::SearchSpaceIterator SearchSpace<TYPE>::begin() const {
    return SearchSpaceIterator(min, stride);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
typename SearchSpace<TYPE>::SearchSpaceIterator SearchSpace<TYPE>::end() const {
    return SearchSpaceIterator(max + stride, stride);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
size_t SearchSpace<TYPE>::size() const {
    return (max - min) / stride + 1;
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
TYPE SearchSpace<TYPE>::getIndex(size_t idx) const {
    return static_cast<TYPE>(min + stride * idx);
}

// ---------------------------------------------------------------------------------------------------------------------

template <typename TYPE>
TYPE SearchSpace<TYPE>::mutate(size_t index, float random) {
    float value = getIndex(index);
    random = random * (max - min);
    value += random;
    while (value < min) {
        value = max - (min - value);
    }
    while (value > max) {
        value = min + (value - max);
    }
    return TYPE(std::round((value - min) / static_cast<float>(stride)));
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::gabac

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_GABAC_BENCHMARK_IMPL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
