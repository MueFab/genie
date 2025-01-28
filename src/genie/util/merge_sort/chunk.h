/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_MERGE_SORT_CHUNK_H_
#define SRC_GENIE_UTIL_MERGE_SORT_CHUNK_H_

// -----------------------------------------------------------------------------

namespace genie::util::merge_sort {
/**
 * @brief A generic chunk of data, either in memory or on disk
 * @tparam T Type of the data
 */
template <class T>
class Chunk {
 public:
  /**
   * @brief Destroy the MergeSortChunk object
   */
  virtual ~Chunk() = default;

  /**
   * @brief Lookup the next element in the chunk
   * @return const T&
   */
  [[nodiscard]] virtual const T& Top() const = 0;

  /**
   * @brief Remove the next element from the chunk
   */
  virtual void Pop() = 0;

  /**
   * @brief Move the next element from the chunk
   * @return T
   */
  [[nodiscard]] virtual T Get() = 0;

  /**
   * @brief Check if the chunk is empty
   * @return true
   * @return false
   */
  [[nodiscard]] virtual bool IsEmpty() const = 0;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util::merge_sort

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_MERGE_SORT_CHUNK_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
