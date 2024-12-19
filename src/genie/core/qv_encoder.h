/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_QV_ENCODER_H_
#define SRC_GENIE_CORE_QV_ENCODER_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <tuple>
#include <utility>

#include "genie/core/access_unit.h"
#include "genie/core/record/chunk.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief The basic interface for modules encoding quality values
 */
class QvEncoder {
 public:
  using qv_coded = std::tuple<std::unique_ptr<parameter::QualityValues>,
                              AccessUnit::Descriptor, stats::PerfStats>;  //!<

  /**
   * @brief
   * @param rec
   * @return
   */
  virtual qv_coded Process(const record::Chunk& rec) = 0;

  /**
   * @brief For polymorphic destruction
   */
  virtual ~QvEncoder() = default;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_QV_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
