/**
 * Copyright 2018-2024 The Genie Authors.
 * @file block_stepper.cc
 * @brief Implementation of the BlockStepper class for iterating over data
 * blocks.
 *
 * This file contains the implementation of the BlockStepper class, which
 * provides mechanisms for iterating over blocks of data. It includes a
 * constructor for initializing the iterator with the current position, end
 * position, and word Size.
 *
 * @details The BlockStepper class is used to step through data blocks in a
 * controlled manner, ensuring that the iteration respects the word Size
 * boundaries. It is useful for processing data blocks in a sequential order.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#include "genie/util/block_stepper.h"

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

BlockStepper::BlockStepper(uint8_t* cur_v, uint8_t* end_v,
                           const uint8_t word_size_v)
    : curr(cur_v), end(end_v), word_size(word_size_v) {}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
