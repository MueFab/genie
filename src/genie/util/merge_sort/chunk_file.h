/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_MERGE_SORT_CHUNK_FILE_H_
#define SRC_GENIE_UTIL_MERGE_SORT_CHUNK_FILE_H_

#include <fstream>
#include <optional>
#include <string>
#include <vector>

#include "genie/util/merge_sort/chunk.h"

// -----------------------------------------------------------------------------

namespace genie::util::merge_sort {
/**
 * @brief A chunk of data stored in a file
 * @tparam T Type of the data
 * @tparam ReadRecFun Function to read a record from a file.
 * Signature: std::optional<T> (std::ifstream&)
 */
template <class T, typename ReadRecFun>
class ChunkFile final : public Chunk<T> {
  /// Input file stream to read the data records
  std::ifstream file_;

  /// Name of the file
  std::string filename_;

  /// Buffer for the next record in the file
  std::optional<T> cur_record_;

  /// Function to read a record from the file
  ReadRecFun read_fun_;

 public:
  /**
   * @brief Construct a new MergeSortChunkFile object
   * @param sorted_records The records to write to the file
   * @param filename Name of the file
   * @param write_fun Function to write a record to a file.
   * Signature: void (std::ofstream&, const T&)
   * @param fun Function to read a record from a file.
   * Signature: std::optional<T> (std::ifstream&)
   */
  template <typename WriteRecFun>
  explicit ChunkFile(std::vector<T> sorted_records, const std::string& filename,
                     WriteRecFun write_fun, ReadRecFun fun);

  /**
   * @brief Destroy the MergeSortChunkFile object and delete the file
   */
  ~ChunkFile() override;

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
  [[nodiscard]] T Get() override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util::merge_sort

// -----------------------------------------------------------------------------

#include "genie/util/merge_sort/chunk_file.impl.h"  // NOLINT(build/include)

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_MERGE_SORT_CHUNK_FILE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
