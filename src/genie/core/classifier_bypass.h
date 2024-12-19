/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_CLASSIFIER_BYPASS_H_
#define SRC_GENIE_CORE_CLASSIFIER_BYPASS_H_

// -----------------------------------------------------------------------------

#include <vector>

#include "genie/core/classifier.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class ClassifierBypass final : public Classifier {
  std::vector<record::Chunk> vec_;  //!< @brief
  bool flushing_ = false;           //!< @brief

 public:
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

#endif  // SRC_GENIE_CORE_CLASSIFIER_BYPASS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
