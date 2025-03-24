/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_MERGE_SORT_CHUNK_FILE_IMPL_H_
#define SRC_GENIE_UTIL_MERGE_SORT_CHUNK_FILE_IMPL_H_

#include <filesystem>  // NOLINT
#include <string>
#include <utility>

#include "genie/util/merge_sort/chunk_file.h"  // NOLINT(build/include)
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::util::merge_sort {

template <class T, typename ReadRecFun>
template <typename WriteRecFun>
ChunkFile<T, ReadRecFun>::ChunkFile(std::vector<T> sorted_records,
                                    const std::string& filename,
                                    WriteRecFun write_fun, ReadRecFun fun)
    : filename_(filename), read_fun_(fun) {
  {
    std::ofstream outfile(filename);

    for (const auto& r : sorted_records) {
      write_fun(outfile, r);
    }
  }
  sorted_records.clear();
  file_ = std::ifstream(filename);
  UTILS_DIE_IF(!file_, "MergeSortChunkFile input error");
  cur_record_ = read_fun_(file_);
}

// -----------------------------------------------------------------------------

template <class T, typename ReadRecFun>
ChunkFile<T, ReadRecFun>::~ChunkFile() {
  std::filesystem::remove(filename_);
}

// -----------------------------------------------------------------------------

template <class T, typename ReadRecFun>
[[nodiscard]] const T& ChunkFile<T, ReadRecFun>::Top() const {
  return *cur_record_;
}

// -----------------------------------------------------------------------------

template <class T, typename ReadRecFun>
[[nodiscard]] bool ChunkFile<T, ReadRecFun>::IsEmpty() const {
  return !cur_record_;
}

// -----------------------------------------------------------------------------

template <class T, typename ReadRecFun>
void ChunkFile<T, ReadRecFun>::Pop() {
  if (file_) {
    cur_record_ = read_fun_(file_);
  } else {
    cur_record_ = std::nullopt;
  }
}

// -----------------------------------------------------------------------------

template <class T, typename ReadRecFun>
T ChunkFile<T, ReadRecFun>::Get() {
  return std::move(*cur_record_);
}

// -----------------------------------------------------------------------------

}  // namespace genie::util::merge_sort

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_MERGE_SORT_CHUNK_FILE_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
