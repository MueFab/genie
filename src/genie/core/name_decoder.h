/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_NAME_DECODER_H_
#define SRC_GENIE_CORE_NAME_DECODER_H_

// -----------------------------------------------------------------------------

#include <string>
#include <tuple>
#include <vector>

#include "genie/core/access_unit.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class NameDecoder {
 public:
  /**
   * @brief
   * @param desc
   * @return
   */
  virtual std::tuple<std::vector<std::string>, stats::PerfStats> Process(
      AccessUnit::Descriptor& desc) = 0;

  /**
   * @brief For polymorphic destruction
   */
  virtual ~NameDecoder() = default;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_NAME_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
