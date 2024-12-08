/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_CLASS_TYPE_H_
#define SRC_GENIE_CORE_RECORD_CLASS_TYPE_H_

// -----------------------------------------------------------------------------

#include <cstdint>

// -----------------------------------------------------------------------------

namespace genie::core::record {

/**
 * @brief
 */
enum class ClassType : uint8_t {
  kNone = 0,
  kClassP = 1,
  kClassN = 2,
  kClassM = 3,
  kClassI = 4,
  kClassHm = 5,
  kClassU = 6,
  kCount = 6
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_CLASS_TYPE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
