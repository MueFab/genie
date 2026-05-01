/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_MERGE_SORT_BUILDER_IMPL_H_
#define SRC_GENIE_UTIL_MERGE_SORT_BUILDER_IMPL_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>
#include <utility>

#include "genie//util/merge_sort/builder.h"

// -----------------------------------------------------------------------------

namespace genie::util::merge_sort {
template <class T, typename ReadRecFun, typename WriteRecFun, typename SortFun>
Builder<T, ReadRecFun, WriteRecFun, SortFun>::Builder(
    const size_t chunk_size, std::string file_base_name, ReadRecFun read_fun,
    WriteRecFun write_fun, SortFun sort_fun)
    : chunk_size_(chunk_size),
      file_base_name_(std::move(file_base_name)),
      read_fun_(read_fun),
      write_fun_(write_fun),
      sort_fun_(sort_fun) {
  cur_chunk_ = std::make_unique<ChunkMemory<T>>();
}

// -----------------------------------------------------------------------------

template <class T, typename ReadRecFun, typename WriteRecFun, typename SortFun>
void Builder<T, ReadRecFun, WriteRecFun, SortFun>::AddRecord(T record) {
  cur_chunk_->AddRecord(std::move(record));
  if (cur_chunk_->size() >= chunk_size_) {
    NewChunk();
  }
}

// -----------------------------------------------------------------------------

template <class T, typename ReadRecFun, typename WriteRecFun, typename SortFun>
void Builder<T, ReadRecFun, WriteRecFun, SortFun>::NewChunk() {
  cur_chunk_->sort(sort_fun_);
  chunks_.emplace_back(cur_chunk_->ToFile(
      file_base_name_ + std::to_string(chunks_.size()), read_fun_, write_fun_));
  cur_chunk_ = std::make_unique<ChunkMemory<T>>();
}

// -----------------------------------------------------------------------------

template <class T, typename ReadRecFun, typename WriteRecFun, typename SortFun>
Merger<T, SortFun> Builder<T, ReadRecFun, WriteRecFun, SortFun>::Finish(
    const bool force_file) {
  if (cur_chunk_->IsEmpty()) {
    return Merger<T, SortFun>(sort_fun_, std::move(chunks_));
  }
  cur_chunk_->sort(sort_fun_);
  if (force_file) {
    chunks_.emplace_back(
        cur_chunk_->ToFile(file_base_name_ + std::to_string(chunks_.size()),
                           read_fun_, write_fun_));
  } else {
    chunks_.emplace_back(std::move(cur_chunk_));
  }
  return Merger<T, SortFun>(sort_fun_, std::move(chunks_));
}

// -----------------------------------------------------------------------------

}  // namespace genie::util::merge_sort

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_MERGE_SORT_BUILDER_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
