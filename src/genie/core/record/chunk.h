/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_CHUNK_H_
#define SRC_GENIE_CORE_RECORD_CHUNK_H_

// -----------------------------------------------------------------------------

#include <utility>
#include <vector>

#include "genie/core/record/record.h"
#include "genie/core/reference_manager.h"
#include "genie/core/stats/perf_stats.h"

// -----------------------------------------------------------------------------

namespace genie::core::record {

/**
 * @brief
 */
class Chunk {
  std::vector<Record> data_;                             //!< @brief
  ReferenceManager::ReferenceExcerpt reference_;         //!< @brief
  std::vector<std::pair<size_t, size_t>> ref_to_write_;  //!< @brief
  size_t ref_id_{};                                      //!< @brief
  stats::PerfStats stats_;                               //!< @brief
  bool reference_only_{false};                           //!< @brief

 public:
  /**
   * @brief
   * @return
   */
  std::vector<Record>& GetData();

  /**
   * @brief
   * @return
   */
  ReferenceManager::ReferenceExcerpt& GetRef();

  /**
   * @brief
   * @param start
   * @param end
   */
  void AddRefToWrite(size_t start, size_t end);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::vector<std::pair<size_t, size_t>>& GetRefToWrite()
      const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const ReferenceManager::ReferenceExcerpt& GetRef() const;

  /**
   * @brief
   * @param id
   */
  void SetRefId(size_t id);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] size_t GetRefId() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::vector<Record>& GetData() const;

  /**
   * @brief
   * @return
   */
  stats::PerfStats& GetStats();

  /**
   * @brief
   * @param s
   */
  void SetStats(stats::PerfStats&& s);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsReferenceOnly() const;

  /**
   * @brief
   * @param ref
   */
  void SetReferenceOnly(bool ref);
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_CHUNK_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
