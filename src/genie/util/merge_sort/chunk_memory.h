/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_MERGE_SORT_CHUNK_MEMORY_H_
#define SRC_GENIE_UTIL_MERGE_SORT_CHUNK_MEMORY_H_

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "genie/util/merge_sort/chunk.h"
#include "genie/util/merge_sort/chunk_file.h"

// -----------------------------------------------------------------------------

namespace genie::util::merge_sort {

/**
 * @brief A chunk of data stored in memory
 * @tparam T Type of the data
 */
template <class T>
class ChunkMemory final : public Chunk<T> {
  /// Data records in the chunk
  std::vector<T> records_;

  /// Current read position in the chunk
  size_t pos_;

 public:
  /**
   * @brief Construct a new MergeSortChunkMemory object
   */
  ChunkMemory();

  /**
   * @brief Lookup the next element in the chunk
   * @return const T&
   */
  [[nodiscard]] const T& Top() const override;
  /**
   * @brief Check if there are no more records in the chunk
   * @return True if the chunk is empty
   */
  [[nodiscard]] bool IsEmpty() const override;

  /**
   * @brief Remove the next element from the chunk
   */
  void Pop() override;

  /**
   * @brief Move the next element from the chunk
   * @return T
   */
  [[nodiscard]]
  T Get() override;

  /**
   * @brief Write the records to a file and return a MergeSortChunkFile object
   * @tparam ReadRecFun Function to read a record from a file.
   * Signature: std::optional<T> (std::ifstream&)
   * @tparam WriteRecFun Function to write a record to a file.
   * Signature: void (std::ofstream&, const T&)
   * @param name Name of the file
   * @param read_fun Function to read a record from a file.
   * @param write_fun Function to write a record to a file.
   * @return File-based chunk
   */
  template <typename ReadRecFun, typename WriteRecFun>
  [[nodiscard]] std::unique_ptr<ChunkFile<T, ReadRecFun>> ToFile(
      const std::string& name, ReadRecFun read_fun, WriteRecFun write_fun);

  /**
   * @brief Add a record to the chunk
   * @param record Record to add
   */
  void AddRecord(T record);

  /**
   * @brief Return the number of records in the chunk
   * @return Number of records
   */
  [[nodiscard]] size_t size() const;

  /**
   * @brief Sort the records in the chunk
   * @tparam Compare Function to compare two records during sorting.
   * Signature: bool (const T&, const T&). It should return true if the first
   * Element is less than the second.
   * @param comp The comparison function
   */
  template <typename Compare>
  void sort(Compare comp);
};

// -----------------------------------------------------------------------------

}  // namespace genie::util::merge_sort

// -----------------------------------------------------------------------------

#include "genie/util/merge_sort/chunk_memory.impl.h"  // NOLINT(build/include)

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_MERGE_SORT_CHUNK_MEMORY_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
