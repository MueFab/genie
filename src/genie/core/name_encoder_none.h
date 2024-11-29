/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_NAME_ENCODER_NONE_H_
#define SRC_GENIE_CORE_NAME_ENCODER_NONE_H_

// -----------------------------------------------------------------------------

#include <tuple>
#include <vector>

#include "genie/core/name_encoder.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class NameEncoderNone final : public NameEncoder {
 public:
  /**
   * @brief
   * @return
   */
  std::tuple<AccessUnit::Descriptor, stats::PerfStats> Process(
      const record::Chunk&) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_NAME_ENCODER_NONE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
