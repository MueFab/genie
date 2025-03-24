/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_MERGE_SORT_BUILDER_H_
#define SRC_GENIE_UTIL_MERGE_SORT_BUILDER_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>

#include "genie/util/merge_sort/chunk.h"
#include "genie/util/merge_sort/chunk_memory.h"
#include "genie/util/merge_sort/merger.h"

// -----------------------------------------------------------------------------

namespace genie::util::merge_sort {

/**
 * @brief A builder for MergeSortChunk objects. It collects records,
 * splits them into chunks of a certain size, sorts the chunks and writes them
 * to files. The files can then be read back by creating a MergeSortMerger
 * @tparam T Type of the data
 * @tparam ReadRecFun Function to read a record from a file.
 * Signature: std::optional<T> (std::ifstream&)
 * @tparam WriteRecFun Function to write a record to a file.
 * Signature: void (std::ofstream&, const T&)
 * @tparam SortFun Function to compare two records during sorting.
 * Signature: bool (const T&, const T&). It should return true if the first
 * Element is less than the second.
 */
template <class T, typename ReadRecFun, typename WriteRecFun, typename SortFun>
class Builder {
  /// Chunks of data (including those on disk)
  std::vector<std::unique_ptr<Chunk<T>>> chunks_;

  /// Current chunk of data (in memory)
  std::unique_ptr<ChunkMemory<T>> cur_chunk_;

  /// Maximum size of a chunk
  size_t chunk_size_;

  /// Base path for the temp files for chunks on disk
  std::string file_base_name_;

  /// Function to read a record from a file
  ReadRecFun read_fun_;

  /// Function to write a record to a file
  WriteRecFun write_fun_;

  /// Function to compare two records during sorting
  SortFun sort_fun_;

 public:
  /**
   * @brief Construct a new MergeSortChunkBuilder object
   * @param chunk_size Maximum size of a chunk. 0 = infinite
   * @param file_base_name Base path for the temp files for chunks on disk
   * @param read_fun Function to read a record from a file.
   * @param write_fun Function to write a record to a file.
   * @param sort_fun Function to compare two records during sorting.
   */
  Builder(size_t chunk_size, std::string file_base_name, ReadRecFun read_fun,
          WriteRecFun write_fun, SortFun sort_fun);

  /**
   * @brief Manually start a new chunk
   */
  void NewChunk();

  /**
   * @brief Add a record to the builder
   * @param record Record to add
   */
  void AddRecord(T record);

  /**
   * @brief Finish the builder and return a MergeSortMerger object
   * @param force_file If true, the last chunk is written to a file. Otherwise,
   * it remains in memory for the subsequent MergeSortMerger object
   * @return MergeSortMerger<T, SortFun>
   */
  Merger<T, SortFun> Finish(bool force_file);
};

// -----------------------------------------------------------------------------

}  // namespace genie::util::merge_sort

// -----------------------------------------------------------------------------

#include "genie/util/merge_sort/builder.impl.h"  // NOLINT(build/include)

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_MERGE_SORT_BUILDER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
