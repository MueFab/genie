/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_CLASSIFIER_REGROUP_H_
#define SRC_GENIE_CORE_CLASSIFIER_REGROUP_H_

// -----------------------------------------------------------------------------

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/classifier.h"
#include "genie/core/record/record.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class ClassifierRegroup final : public Classifier {
 public:
  enum class RefMode { kNone = 0, kRelevant = 1, kFull = 2 };

 private:
  std::vector<record::Chunk> finished_chunks_;  //!< @brief

  using class_block_type = std::vector<record::Chunk>;      //!< @brief
  using paired_block_type = std::vector<class_block_type>;  //!< @brief
  using ref_no_ref_block_type = std::vector<paired_block_type>;
  ref_no_ref_block_type current_chunks_;                         //!< @brief
  record::Chunk current_unpaired_u_chunk_;                       //! @brief
  ReferenceManager* ref_mgr_;                                    //!< @brief
  uint16_t current_seq_id_;                                      //!< @brief
  std::vector<std::pair<size_t, size_t>> current_seq_coverage_;  //!< @brief
  std::map<std::string, std::vector<uint8_t>> ref_state_;        //!< @brief

  size_t au_size_;                    //!< @brief
  stats::PerfStats stats_;            //!< @brief
  RefMode ref_mode_;                  //!< @brief
  size_t ref_mode_full_seq_id_{0};    //!< @brief
  size_t ref_mode_full_cov_id_{0};    //!< @brief
  size_t ref_mode_full_chunk_id_{0};  //!< @brief

  bool raw_ref_mode_ = true;  //!< @brief

  /**
   * @brief
   * @param start
   * @param end
   * @return
   */
  [[nodiscard]] bool IsCovered(size_t start, size_t end) const;

  /**
   * @brief
   * @param r
   * @return
   */
  [[nodiscard]] bool IsCovered(const record::Record& r) const;

  /**
   * @brief
   * @param data
   */
  void QueueFinishedChunk(record::Chunk& data);

 public:
  /**
   * @brief
   * @param ref
   * @param index
   * @return
   */
  bool IsWritten(const std::string& ref, size_t index);

  /**
   * @brief
   * @param au_size
   * @param ref_manager
   * @param mode
   * @param raw_ref
   */
  ClassifierRegroup(size_t au_size, ReferenceManager* ref_manager, RefMode mode,
                    bool raw_ref);

  /**
   * @brief
   * @return
   */
  record::Chunk GetChunk() override;

  /**
   * @brief
   * @param c
   */
  void Add(record::Chunk&& c) override;

  /**
   * @brief
   */
  void Flush() override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_CLASSIFIER_REGROUP_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
