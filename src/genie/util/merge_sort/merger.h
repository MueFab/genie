/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_MERGE_SORT_MERGER_H_
#define SRC_GENIE_UTIL_MERGE_SORT_MERGER_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <queue>
#include <vector>

#include "genie/util/merge_sort/chunk.h"

// -----------------------------------------------------------------------------

namespace genie::util::merge_sort {

/**
 * @brief A merger for multiple MergeSortChunk objects, i.e. it merges multiple
 * already sorted chunks of data into a single stream of sorted data
 * @tparam T Type of the data
 * @tparam SortFun Function to compare two records during sorting.
 * Signature: bool (const T&, const T&). It should return true if the first
 * Element is less than the second.
 */
template <class T, typename SortFun>
class Merger {
  class Comparer {
    const SortFun& sort_fun_;
    const std::vector<std::unique_ptr<Chunk<T>>>& chunks_;

   public:
    explicit Comparer(const SortFun& sort_fun,
                      const std::vector<std::unique_ptr<Chunk<T>>>& chunks)
        : sort_fun_(sort_fun), chunks_(chunks) {}

    bool operator()(const size_t a, const size_t b) const {
      return !sort_fun_(chunks_[a]->Top(), chunks_[b]->Top());
    }
  };

  /// Function to compare two records during sorting
  SortFun sort_fun_;

  /// Owned chunks of data
  std::vector<std::unique_ptr<Chunk<T>>> chunks_;

  /// Comparator for the priority queue
  Comparer cmp_;

  /// Priority queue to keep track of the order of the chunks, based on
  /// their top element. The content of the queue is the index in _chunks
  std::priority_queue<size_t, std::vector<size_t>, Comparer> queue_;

 public:
  /**
   * @brief Construct a new MergeSortMerger object
   * @param sort_fun Function to compare two records during sorting.
   * @param chunks The chunks to merge
   */
  explicit Merger(SortFun sort_fun,
                  std::vector<std::unique_ptr<Chunk<T>>> chunks);

  /**
   * @brief Check if there are no more records in the merger
   * @return True if the merger is empty
   */
  [[nodiscard]] bool IsEmpty() const;

  /**
   * @brief Lookup and move the next element in the merger
   * @return The next element
   */
  T Get();
};

// -----------------------------------------------------------------------------

}  // namespace genie::util::merge_sort

// -----------------------------------------------------------------------------

#include "genie/util/merge_sort/merger.impl.h"  // NOLINT

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_MERGE_SORT_MERGER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
