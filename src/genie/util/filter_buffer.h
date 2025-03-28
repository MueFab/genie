/**
 * Copyright 2018-2024 The Genie Authors.
 * @file filter_buffer.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Contains the definitions of kernel classes and the `FilterBuffer`
 * class for signal processing.
 *
 * This file provides the declarations of the `GaussKernel` and
 * `RectangleKernel` classes, which represent different types of filter kernels.
 * Additionally, it defines the
 * `FilterBuffer` class, which is used to apply these kernels to a signal using
 * a circular buffer structure.
 *
 * @details The `GaussKernel` and `RectangleKernel` classes are used to sample
 * from Gaussian and rectangular distributions, respectively. The `FilterBuffer`
 * class implements a filter buffer using a circular buffer structure and a
 * specified kernel function. It is used for smoothing and filtering input
 * signals.
 */

#ifndef SRC_GENIE_UTIL_FILTER_BUFFER_H_
#define SRC_GENIE_UTIL_FILTER_BUFFER_H_

// -----------------------------------------------------------------------------

#include <cstddef>
#include <functional>
#include <vector>

#include "genie/util/circular_buffer.h"

// -----------------------------------------------------------------------------

namespace genie::util {

/**
 * @brief Represents a Gaussian kernel used for signal processing.
 *
 * The `GaussKernel` class samples from a normal (Gaussian) distribution
 * to calculate the values of the kernel at specified positions. It provides
 * methods for calculating the Gaussian value and determining the minimal
 * buffer Size required to capture significant values.
 */
class GaussKernel {
  /// The standard deviation of the Gaussian distribution.
  const double sigma_;

  /// The value of π (pi).
  constexpr static double pi_ = 3.14159265359;

  /// The value of Euler's number (e).
  constexpr static double euler_ = 2.71828182846;

  /// Precomputed constant for Gaussian function.
  const double inv_sqrt_sigma_2_pi_;

 public:
  /**
   * @brief Constructs a Gaussian kernel with the specified standard deviation.
   *
   * @param sigma The standard deviation of the Gaussian distribution.
   */
  explicit GaussKernel(double sigma = 1.0);

  /**
   * @brief Calculates the Gaussian value at a given position in the kernel.
   *
   * This method computes the value of the Gaussian distribution at a specific
   * position in the kernel, considering the specified size and a mean centered
   * at `Size/2`.
   *
   * @param pos The position in the kernel.
   * @param size The Size of the kernel.
   * @return The value of the Gaussian function at the specified position.
   */
  [[nodiscard]] double CalcValue(size_t pos, size_t size) const;

  /**
   * @brief Calculates the minimal kernel Size required to capture significant
   * values.
   *
   * This method determines the minimum Size of the kernel that is needed to
   * include values above a specified threshold, up to a given maximum Size.
   *
   * @param threshold The cutoff threshold for significant values.
   * @param maximum The maximum Size of the kernel.
   * @return The minimal Size required to capture significant values.
   */
  [[nodiscard]] size_t CalcMinSize(double threshold,
                                   size_t maximum = 101) const;
};

/**
 * @brief Represents a rectangular (uniform) kernel used for signal processing.
 *
 * The `RectangleKernel` class samples from a rectangular (uniform) distribution
 * to calculate the values of the kernel at specified positions. It provides
 * methods for calculating the uniform value and determining the minimal buffer
 * Size required.
 */
class RectangleKernel {
  /// The Size parameter of the rectangular distribution.
  const double size_;

 public:
  /**
   * @brief Constructs a rectangular kernel with the specified Size.
   *
   * @param size The Size parameter of the rectangular distribution.
   */
  explicit RectangleKernel(double size = 1.0);

  /**
   * @brief Calculates the uniform value at a given position in the kernel.
   *
   * This method computes the value of the rectangular (uniform) distribution at
   * a specific position in the kernel, considering the specified Size and a
   * mean centered at `size/2`.
   *
   * @param pos The position in the kernel.
   * @param size The Size of the kernel.
   * @return The value of the rectangular function at the specified position.
   */
  [[nodiscard]] double CalcValue(size_t pos, size_t size) const;

  /**
   * @brief Calculates the minimal kernel Size required to capture significant
   * values.
   *
   * This method determines the minimum Size of the kernel needed to include
   * values above a specified threshold, up to a given maximum Size.
   *
   * @param maximum The maximum Size of the kernel.
   * @return The minimal Size required to capture significant values.
   */
  [[nodiscard]] size_t CalcMinSize(size_t maximum = 101) const;
};

/**
 * @brief A buffer used for filtering an input signal using a specified kernel.
 *
 * The `FilterBuffer` class applies a specified filter kernel to an input signal
 * using a circular buffer structure. It allows for smoothing and filtering of
 * signals based on the selected kernel and buffer configuration.
 */
class FilterBuffer {
  std::vector<double> kernel_;  //!< @brief The filter kernel values.
  CircularBuffer<double>
      buffer_;  //!< @brief The circular buffer for storing input values.

 public:
  /**
   * @brief Pushes a new activity score into the filter buffer.
   *
   * This method inserts a new activity score into the filter buffer, updating
   * the internal state and shifting the buffer content as needed.
   *
   * @param activity_score The new activity score to be added.
   */
  void push(double activity_score);

  /**
   * @brief Calculates the filtered score at the current buffer position.
   *
   * This method computes the filtered score using the kernel values and the
   * current state of the circular buffer.
   *
   * @return The filtered score at the current position.
   */
  [[nodiscard]] double filter() const;

  /**
   * @brief Constructs a `FilterBuffer` with the specified kernel builder
   * function and Size.
   *
   * This constructor initializes the `FilterBuffer` with a kernel defined by
   * the provided function and a specified Size. The kernel builder function is
   * used to populate the kernel values.
   *
   * @param kernel_builder The function used to build the kernel values.
   * @param kernel_size The Size of the kernel.
   */
  FilterBuffer(const std::function<double(size_t, size_t)>& kernel_builder,
               size_t kernel_size);

  /**
   * @brief Constructs an empty `FilterBuffer` with no kernel.
   *
   * This constructor creates a dummy buffer with no active kernel, primarily
   * used for placeholder or testing purposes.
   */
  FilterBuffer();

  /**
   * @brief Retrieves the Size of the filter buffer.
   *
   * @return The Size of the filter buffer.
   */
  [[nodiscard]] size_t GetSize() const;

  /**
   * @brief Retrieves the distance between the buffer center and borders.
   *
   * This method returns the distance between the center of the buffer and its
   * borders, which is determined by the kernel Size.
   *
   * @return The offset distance between the center and the borders of the
   * buffer.
   */
  [[nodiscard]] size_t GetOffset() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_FILTER_BUFFER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
