/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_NAME_ENCODER_H_
#define SRC_GENIE_CORE_NAME_ENCODER_H_

// -----------------------------------------------------------------------------

#include <tuple>
#include <utility>

#include "genie/core/access_unit.h"
#include "genie/core/record/chunk.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief The basic interface for modules encoding quality values
 */
class NameEncoder {
 public:
  /**
   * @brief
   * @param rec
   * @return
   */
  virtual std::tuple<AccessUnit::Descriptor, stats::PerfStats> Process(
      const record::Chunk& rec) = 0;

  /**
   * @brief For polymorphic destruction
   */
  virtual ~NameEncoder() = default;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_NAME_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
