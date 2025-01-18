/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_REF_DECODER_H_
#define SRC_GENIE_CORE_REF_DECODER_H_

// -----------------------------------------------------------------------------

#include <string>

#include "genie/core/access_unit.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class RefDecoder {
 public:
  /**
   * @brief
   */
  virtual ~RefDecoder() = default;

  /**
   * @brief
   * @param t
   * @return
   */
  virtual std::string Decode(AccessUnit&& t) = 0;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_REF_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
