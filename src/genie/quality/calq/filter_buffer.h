/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_FILTER_BUFFER_H_
#define SRC_GENIE_QUALITY_CALQ_FILTER_BUFFER_H_

// -----------------------------------------------------------------------------

#include <cstddef>
#include <functional>
#include <vector>

// -----------------------------------------------------------------------------

#include "genie/quality/calq/circular_buffer.h"

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

// Samples a normal distribution
class GaussKernel {
 private:
    const double SIGMA;

    constexpr static double PI = 3.14159265359;
    constexpr static double EULER = 2.71828182846;
    const double INV_SQRT_SIGMA_2PI;

 public:
    // init
    explicit GaussKernel(double sigma = 1.0);

    // Get gauss value at position pos and buffersize size with mean=size/2
    [[nodiscard]] double calcValue(size_t pos, size_t size) const;

    // Calculates how big a buffer must be to contain all values above
    // threshold. No size greater than maximum is returned.
    [[nodiscard]] size_t calcMinSize(double threshold, size_t maximum = 101) const;
};

// -----------------------------------------------------------------------------

// Samples an uniform distribution
class RectangleKernel {
 private:
    const double SIZE;

 public:
    // init
    explicit RectangleKernel(double size = 1.0);

    // Get rect value at position pos and buffersize size with mean=size/2
    [[nodiscard]] double calcValue(size_t pos, size_t size) const;

    // Calculates how big a buffer must be to contain all values above
    // threshold. No size greater than maximum is returned.
    [[nodiscard]] size_t calcMinSize(size_t maximum = 101) const;
};

// -----------------------------------------------------------------------------

// Filters an input signal using a filter kernel
class FilterBuffer {
 private:
    std::vector<double> kernel;
    CircularBuffer<double> buffer;

 public:
    // New activity score in pipeline
    void push(double activityScore);

    // Calculate filter score at offset position
    [[nodiscard]] double filter() const;

    // Initialize buffer and
    FilterBuffer(const std::function<double(size_t, size_t)>& kernelBuilder, size_t kernelSize);

    // Create dummy buffer
    FilterBuffer();

    // Buffer size
    [[nodiscard]] size_t getSize() const;

    // Distance between buffer center and borders
    [[nodiscard]] size_t getOffset() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_FILTER_BUFFER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
