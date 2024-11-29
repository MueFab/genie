/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/filter_buffer.h"

#include <algorithm>
#include <cmath>

#include "genie/quality/calq/error_exception_reporter.h"

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

GaussKernel::GaussKernel(const double sigma)
    : sigma_(sigma),
      inv_sqrt_sigma_2_pi_(1.0 / (std::sqrt(2.0 * pi_) * sigma)) {}

// -----------------------------------------------------------------------------

double GaussKernel::CalcValue(const size_t pos, const size_t size) const {
  const double mean = std::floor(static_cast<double>(size - 1) / 2.0);
  double exponent = (static_cast<double>(pos) - mean) / sigma_;
  exponent = exponent * exponent * -0.5;
  return inv_sqrt_sigma_2_pi_ * std::pow(euler_, exponent);
}

// -----------------------------------------------------------------------------

size_t GaussKernel::CalcMinSize(double threshold, const size_t maximum) const {
  threshold /= inv_sqrt_sigma_2_pi_;
  threshold = std::log(threshold);
  threshold *= -2.0;
  threshold = std::sqrt(threshold) * sigma_;  // Euler now reversed

  // + 1 to make sure it is odd.
  const auto size = static_cast<size_t>(std::ceil(threshold) * 2 + 1);
  return std::min(size, maximum);
}

// -----------------------------------------------------------------------------

// New activity score in pipeline
void FilterBuffer::push(const double activity_score) {
  buffer_.push(activity_score);
}

// -----------------------------------------------------------------------------

// Calculate filter score at offset position
double FilterBuffer::filter() const {
  double result = 0.0;
  for (size_t i = 0; i < kernel_.size(); ++i) {
    result += kernel_[i] * buffer_[i];
    // std::cout << kernel[i] << " " << buffer[i] << std::endl;
  }
  return result;
}

// -----------------------------------------------------------------------------

// Initialize buffer and
FilterBuffer::FilterBuffer(
    const std::function<double(size_t, size_t)>& kernel_builder,
    const size_t kernel_size)
    : buffer_(kernel_size, 0.0) {
  if (!(kernel_size % 2)) {
    THROW_ERROR_EXCEPTION("Kernel Size must be an odd number");
  }
  kernel_.resize(kernel_size, 0.0);

  for (size_t i = 0; i < kernel_.size(); ++i) {
    kernel_[i] = kernel_builder(i, kernel_size);
  }
}

// -----------------------------------------------------------------------------

FilterBuffer::FilterBuffer() : buffer_(1, 0.0) {}

// -----------------------------------------------------------------------------

size_t FilterBuffer::GetSize() const { return buffer_.size(); }

// -----------------------------------------------------------------------------

size_t FilterBuffer::GetOffset() const { return (buffer_.size() + 1) / 2; }

// -----------------------------------------------------------------------------

RectangleKernel::RectangleKernel(const double size) : size_(size) {}

// -----------------------------------------------------------------------------

double RectangleKernel::CalcValue(const size_t pos, const size_t size) const {
  const double mean = std::floor(static_cast<double>(size - 1) / 2.0);
  return static_cast<double>(pos) - mean <= size_ ? 1.0 : 0.0;
}

// -----------------------------------------------------------------------------

size_t RectangleKernel::CalcMinSize(const size_t maximum) const {
  return static_cast<size_t>(
      std::min(size_ * 2 + 1, static_cast<double>(maximum)));
}

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
