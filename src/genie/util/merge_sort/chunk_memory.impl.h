/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_MERGE_SORT_CHUNK_MEMORY_IMPL_H_
#define SRC_GENIE_UTIL_MERGE_SORT_CHUNK_MEMORY_IMPL_H_

#include <algorithm>
#include <string>
#include <memory>
#include <utility>

#include "genie/util/merge_sort/chunk_memory.h"

// -----------------------------------------------------------------------------

namespace genie::util::merge_sort {

// -----------------------------------------------------------------------------

template <class T>
ChunkMemory<T>::ChunkMemory() : pos_(0) {}

// -----------------------------------------------------------------------------

template <class T>
[[nodiscard]] const T& ChunkMemory<T>::Top() const {
  return records_[pos_];
}

// -----------------------------------------------------------------------------

template <class T>
[[nodiscard]] bool ChunkMemory<T>::IsEmpty() const {
  return pos_ >= records_.size();
}

// -----------------------------------------------------------------------------

template <class T>
void ChunkMemory<T>::Pop() {
  ++pos_;
}

// -----------------------------------------------------------------------------

template <class T>
T ChunkMemory<T>::Get() {
  return std::move(records_[pos_]);
}

// -----------------------------------------------------------------------------

template <class T>
template <typename ReadRecFun, typename WriteRecFun>
std::unique_ptr<ChunkFile<T, ReadRecFun>> ChunkMemory<T>::ToFile(
    const std::string& name, ReadRecFun read_fun, WriteRecFun write_fun) {
  return std::make_unique<ChunkFile<T, ReadRecFun>>(std::move(records_), name,
                                                    write_fun, read_fun);
}

// -----------------------------------------------------------------------------

template <class T>
void ChunkMemory<T>::AddRecord(T record) {
  records_.emplace_back(std::move(record));
}

// -----------------------------------------------------------------------------

template <class T>
size_t ChunkMemory<T>::size() const {
  return records_.size();
}

// -----------------------------------------------------------------------------

template <class T>
template <typename Compare>
void ChunkMemory<T>::sort(Compare comp) {
  std::sort(records_.begin(), records_.end(), comp);
}

// -----------------------------------------------------------------------------

}  // namespace genie::util::merge_sort

// -----------------------------------------------------------------------------

#include "genie/util/merge_sort/chunk_memory.impl.h"  // NOLINT(build/include)

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_MERGE_SORT_CHUNK_MEMORY_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
