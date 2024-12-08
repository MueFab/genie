/**
 * Copyright 2018-2024 The Genie Authors.
 * @file block_stepper.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the BlockStepper structure for iterating over data
 * blocks.
 *
 * This file contains the declaration of the BlockStepper structure, which
 * provides methods to iterate over a block of data. It allows for accessing,
 * modifying, and navigating through the elements of a data block with a
 * specified word Size.
 *
 * @details The BlockStepper struct provides an interface for low-level
 * manipulation of data blocks, offering functionalities like retrieving the
 * current element, updating its value, and checking validity of the current
 * position. It is designed to be used when working with raw memory data in a
 * structured manner.
 */

#ifndef SRC_GENIE_UTIL_BLOCK_STEPPER_H_
#define SRC_GENIE_UTIL_BLOCK_STEPPER_H_

// -----------------------------------------------------------------------------

#include <cstdint>

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief A handle to quickly iterate over a block of raw data.
 *
 * The BlockStepper struct provides a convenient way to step through a block of
 * data, treating each element in the block as a separate unit based on the
 * specified `wordSize`. It is commonly used when the Size of each data element
 * is larger than a byte.
 */
struct BlockStepper {
  uint8_t* curr;  //!< @brief Pointer to the current position in the data block.
  uint8_t* end;   //!< @brief Pointer to the end of the data block (first
                  //!< invalid position).
  uint8_t
      word_size;  //!< @brief Size in bytes of each element in the data block.

  /**
   * @brief Constructs a BlockStepper object for iterating over a data block.
   *
   * This constructor initializes the BlockStepper with a starting and ending
   * position, along with the Size of each element. It provides a way to
   * define the boundaries of the block and set up the stepping mechanism.
   *
   * @param cur_v A pointer to the starting position in the data block.
   * @param end_v A pointer to the end of the data block (first invalid
   * position).
   * @param word_size_v The Size of each element in bytes. Defaults to 1.
   */
  explicit BlockStepper(uint8_t* cur_v = nullptr, uint8_t* end_v = nullptr,
                        uint8_t word_size_v = 1);

  /**
   * @brief Checks if the current position is still within the valid range.
   *
   * This function checks whether the current pointer is within the valid data
   * range. It returns true if the current position is less than the end
   * position, indicating that more elements can be processed.
   *
   * @return True if the current position is valid and not yet at the end
   * position, false otherwise.
   */
  [[nodiscard]] bool IsValid() const;

  /**
   * @brief Advances the current pointer by one element.
   *
   * This function increments the current pointer by `wordSize` bytes, moving
   * it to the next element in the data block. It does not perform boundary
   * checks, so ensure that `isValid()` is true before calling this function.
   */
  void Inc();

  /**
   * @brief Retrieves the value of the current element.
   *
   * This function reads the value at the current position, interpreting it as
   * a 64-bit unsigned integer. The value is constructed from `wordSize`
   * bytes, starting at the current pointer position.
   *
   * @return The value of the current element as an uint64_t.
   */
  [[nodiscard]] uint64_t Get() const;

  /**
   * @brief Sets the value of the current element to the specified value.
   *
   * This function updates the value at the current pointer position, writing
   * the new value as `wordSize` bytes. The value is stored as a 64-bit
   * unsigned integer and is written in a byte-wise manner.
   *
   * @param val The new value to set at the current position.
   */
  void Set(uint64_t val) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#include "genie/util/block_stepper.impl.h"  // NOLINT

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_BLOCK_STEPPER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
