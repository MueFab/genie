
/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_CLASSIFIER_H_
#define SRC_GENIE_CORE_CLASSIFIER_H_

// -----------------------------------------------------------------------------

#include "genie/core/record/chunk.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class Classifier {
 public:
  /**
   * @brief
   * @return
   */
  virtual record::Chunk GetChunk() = 0;

  /**
   * @brief
   * @param c
   */
  virtual void Add(record::Chunk&& c) = 0;

  /**
   * @brief
   */
  virtual void Flush() = 0;

  /**
   * @brief
   */
  virtual ~Classifier() = default;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_CLASSIFIER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
