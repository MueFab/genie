/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_QV_DECODER_H_
#define SRC_GENIE_CORE_QV_DECODER_H_

// -----------------------------------------------------------------------------

#include <string>
#include <tuple>
#include <vector>

#include "genie/core/access_unit.h"

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------

class QvDecoder {
 public:
  /**
   * @brief
   * @param param
   * @param ecigar
   * @param positions
   * @param desc
   * @return
   */
  virtual std::tuple<std::vector<std::string>, stats::PerfStats> Process(
      const parameter::QualityValues& param,
      const std::vector<std::string>& ecigar,
      const std::vector<uint64_t>& positions, AccessUnit::Descriptor& desc) = 0;
  /**
   * @brief For polymorphic destruction
   */
  virtual ~QvDecoder() = default;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_QV_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
