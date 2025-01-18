/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for the UniformMinMaxQuantizer class.
 *
 * This file defines the `UniformMinMaxQuantizer` class, which is a specialized
 * version of the `UniformQuantizer`. The
 * `UniformMinMaxQuantizer` class provides a uniform quantization strategy
 * within a specified minimum and maximum value range, offering flexibility and
 * control over quantization intervals.
 *
 * @details The class inherits from the `UniformQuantizer` base class and sets
 * up a uniform quantizer that divides the range between a defined minimum and
 * maximum value into a specified number of quantization steps. This quantizer
 * is useful when a strict value range is required for quality score
 * compression, ensuring that all scores fall within the given bounds.
 */

#ifndef SRC_GENIE_UTIL_UNIFORM_MIN_MAX_QUANTIZER_H_
#define SRC_GENIE_UTIL_UNIFORM_MIN_MAX_QUANTIZER_H_

// -----------------------------------------------------------------------------

#include "genie/util/uniform_quantizer.h"

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief A specialized uniform quantizer with configurable value range.
 *
 * The `UniformMinMaxQuantizer` class is derived from the `UniformQuantizer`
 * class and provides a quantization strategy that ensures the input values are
 * uniformly distributed between a defined minimum and maximum range.
 *
 * @details This class initializes a uniform quantizer based on the specified
 * value range and the number of quantization steps. It guarantees that all
 * quantized values are mapped within the range `[valueMin, valueMax]`, making
 * it suitable for encoding quality scores with bounded value distributions.
 *
 * ### Example Usage
 * @code
 * // Create a uniform quantizer with range [0, 40] and 10 quantization steps
 * genie::util::UniformMinMaxQuantizer quantizer(0, 40, 10);
 *
 * // Perform quantization on a quality value
 * int quantized_value = quantizer.valueToIndex(35);
 * @endcode
 */
class UniformMinMaxQuantizer final : public UniformQuantizer {
 public:
  /**
   * @brief Constructs a uniform quantizer with specified value range and
   * steps.
   *
   * Initializes the `UniformMinMaxQuantizer` with a minimum and maximum value
   * range and the number of quantization steps. The range is divided
   * uniformly into the given number of intervals.
   *
   * @param value_min The minimum value for the quantizer range.
   * @param value_max The maximum value for the quantizer range.
   * @param nr_steps The number of quantization steps within the range.
   */
  UniformMinMaxQuantizer(const int& value_min, const int& value_max,
                         const size_t& nr_steps);

  /**
   * @brief Default destructor for the UniformMinMaxQuantizer class.
   *
   * Cleans up any resources allocated by the `UniformMinMaxQuantizer`. As
   * this class does not hold any dynamically allocated resources, the
   * destructor is straightforward and primarily used for completeness.
   */
  ~UniformMinMaxQuantizer() override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_UNIFORM_MIN_MAX_QUANTIZER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
