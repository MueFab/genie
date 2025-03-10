/**
 * Copyright 2018-2024 The Genie Authors.
 * @file uniform_min_max_quantizer.cc
 * @brief Implementation of UniformMinMaxQuantizer for Genie.
 *
 * Defines a quantizer that ensures the smallest and largest reconstruction
 * values align with specified minimum and maximum values
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/util/uniform_min_max_quantizer.h"

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

UniformMinMaxQuantizer::UniformMinMaxQuantizer(const int& value_min,
                                               const int& value_max,
                                               const size_t& nr_steps)
    : UniformQuantizer(value_min, value_max, nr_steps) {
  // Change the smallest and largest reconstruction values

  constexpr size_t smallest_index = 0;
  const size_t largest_index = nr_steps - 1;

  for (auto& [fst, snd] : lut_) {
    (void) fst;
    const size_t current_index = snd.first;
    if (current_index == smallest_index) {
      snd.second = value_min;
    }
    if (current_index == largest_index) {
      snd.second = value_max;
    }
  }

  for (auto& [fst, snd] : inverse_lut_) {
    const size_t current_index = fst;
    if (current_index == smallest_index) {
      snd = value_min;
    }
    if (current_index == largest_index) {
      snd = value_max;
    }
  }
}

// -----------------------------------------------------------------------------

UniformMinMaxQuantizer::~UniformMinMaxQuantizer() = default;

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
