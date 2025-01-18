/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for the UniformQuantizer class.
 *
 * This file defines the `UniformQuantizer` class.
 *
 * @details The `UniformQuantizer` inherits from the base `Quantizer` class and
 * implements a uniform quantization scheme. This is a simpler quantization
 * method compared to others like Lloyd-Max, where the quantization boundaries
 * are evenly spaced. It is particularly useful when the distribution of values
 * is roughly uniform and doesn't have distinct clusters or peaks.
 */

#ifndef SRC_GENIE_UTIL_UNIFORM_QUANTIZER_H_
#define SRC_GENIE_UTIL_UNIFORM_QUANTIZER_H_

// -----------------------------------------------------------------------------

#include "genie/util/quantizer.h"

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief The `UniformQuantizer` class provides a simple uniform quantization
 * scheme.
 *
 * The `UniformQuantizer` divides the range of quality values into evenly spaced
 * steps. Each step is assigned a representative value, which is used as the
 * quantized value for all input values falling within that step. This
 * quantization method is particularly effective for datasets where quality
 * values are evenly distributed, or when a simple compression method is
 * required.
 */
class UniformQuantizer : public Quantizer {
 public:
  /**
   * @brief Constructs a `UniformQuantizer` with a specified value range and
   * number of steps.
   *
   * @param value_min The minimum value in the range to be quantized.
   * @param value_max The maximum value in the range to be quantized.
   * @param nr_steps The number of quantization steps (intervals) within the
   * specified range.
   *
   * This constructor initializes the quantizer with evenly spaced intervals
   * between `valueMin` and `valueMax`. Each interval is assigned a unique
   * quantized value, which is used during the encoding and decoding
   * processes.
   */
  UniformQuantizer(int value_min, int value_max, size_t nr_steps);

  /**
   * @brief Destructor for the `UniformQuantizer` class.
   *
   * This function cleans up any allocated resources and performs any
   * necessary finalization.
   */
  ~UniformQuantizer() override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_UNIFORM_QUANTIZER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
