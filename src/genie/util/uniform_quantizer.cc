/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/util/uniform_quantizer.h"

#include <cmath>
#include <queue>
#include <utility>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

UniformQuantizer::UniformQuantizer(const int value_min, const int value_max,
                                   const size_t nr_steps) {
  UTILS_DIE_IF(value_min > value_max || nr_steps <= 1,
               "Error in quantizer initialization");

  // Compute the step Size
  const auto step_size =
      static_cast<int>(floor(static_cast<double>(value_max - value_min) /
                             static_cast<double>(nr_steps)));

  // Compute the borders and the representative values
  std::queue<int> borders;
  std::queue<int> reconstruction_values;
  int new_border = value_min;
  borders.push(value_min);
  reconstruction_values.push(
      value_min +
      static_cast<int>(round(static_cast<double>(step_size) / 2.0)));
  for (int i = 0; i < static_cast<int>(nr_steps - 1); i++) {
    new_border += step_size;
    borders.push(new_border);
    reconstruction_values.push(
        new_border +
        static_cast<int>(round(static_cast<double>(step_size) / 2.0)));
  }
  borders.push(value_max);

  // Fill the quantization table
  borders.pop();
  int current_index = 0;
  int current_reconstruction_value = reconstruction_values.front();
  int current_border = borders.front();
  for (int value = value_min; value <= value_max; ++value) {
    if (value > current_border) {
      current_index++;
      reconstruction_values.pop();
      borders.pop();
      current_reconstruction_value = reconstruction_values.front();
      current_border = borders.front();
    }
    std::pair curr(current_index, current_reconstruction_value);
    lut_.insert(std::pair(value, curr));
    inverse_lut_.insert(curr);
  }
}

// -----------------------------------------------------------------------------

UniformQuantizer::~UniformQuantizer() = default;

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
