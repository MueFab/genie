
/**
 * Copyright 2018-2024 The Genie Authors.
 * @file block_stepper.impl.h
 * @brief Implementation of the BlockStepper class for iterating over data
 * blocks.
 *
 * This file contains the implementation of the BlockStepper class, which
 * provides mechanisms for iterating over and manipulating data blocks with
 * different word sizes. It includes methods for checking validity, incrementing
 * the current position, and getting and setting values at the current position.
 *
 * @details The BlockStepper class supports operations such as checking if the
 * current position is valid, incrementing the position by the word Size, and
 * getting and setting values at the current position. It ensures compatibility
 * with different word sizes and provides utility functions for common data
 * manipulations.
 *
 * @tparam T The type of the data elements being managed by the BlockStepper.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_BLOCK_STEPPER_IMPL_H_
#define SRC_GENIE_UTIL_BLOCK_STEPPER_IMPL_H_

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

inline bool BlockStepper::IsValid() const { return curr != end; }

// -----------------------------------------------------------------------------

inline void BlockStepper::Inc() { curr += word_size; }

// -----------------------------------------------------------------------------

inline uint64_t BlockStepper::Get() const {
  switch (word_size) {
    case 1:
      return *curr;
    case 2:
      return *reinterpret_cast<uint16_t*>(curr);
    case 4:
      return *reinterpret_cast<uint32_t*>(curr);
    case 8:
      return *reinterpret_cast<uint64_t*>(curr);
    default:
      break;
  }
  return 0;
}

// -----------------------------------------------------------------------------

inline void BlockStepper::Set(const uint64_t val) const {
  switch (word_size) {
    case 1:
      *curr = static_cast<uint8_t>(val);
      return;
    case 2:
      *reinterpret_cast<uint16_t*>(curr) = static_cast<uint16_t>(val);
      return;
    case 4:
      *reinterpret_cast<uint32_t*>(curr) = static_cast<uint32_t>(val);
      return;
    case 8:
      *reinterpret_cast<uint64_t*>(curr) = val;
    default:
      break;
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_BLOCK_STEPPER_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
