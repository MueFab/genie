#ifndef CALQ_FILTER_BUFFER_H_
#define CALQ_FILTER_BUFFER_H_

// -----------------------------------------------------------------------------

#include <cstddef>
#include <functional>
#include <vector>

// -----------------------------------------------------------------------------

#include "circular_buffer.h"

// -----------------------------------------------------------------------------

namespace calq {

// -----------------------------------------------------------------------------

// Samples a normal distribution
class GaussKernel
{
 private:
    const double SIGMA;

    constexpr static double PI = 3.14159265359;
    constexpr static double EULER = 2.71828182846;
    const double INV_SQRT_SIGMA_2PI;

 public:
    // init
    explicit GaussKernel(double sigma = 1.0);

    // Get gauss value at position pos and buffersize size with mean=size/2
    double calcValue(size_t pos,
                     size_t size
    ) const;

    // Calculates how big a buffer must be to contain all values above
    // threshold. No size greater than maximum is returned.
    size_t calcMinSize(double threshold,
                       size_t maximum = 101
    ) const;
};

// -----------------------------------------------------------------------------

// Samples an uniform distribution
class RectangleKernel
{
 private:
    const double SIZE;

 public:
    // init
    explicit RectangleKernel(double size = 1.0);

    // Get rect value at position pos and buffersize size with mean=size/2
    double calcValue(size_t pos,
                     size_t size
    ) const;

    // Calculates how big a buffer must be to contain all values above
    // threshold. No size greater than maximum is returned.
    size_t calcMinSize(size_t maximum = 101) const;
};

// -----------------------------------------------------------------------------

// Filters an input signal using a filter kernel
class FilterBuffer
{
 private:
    std::vector<double> kernel;
    CircularBuffer<double> buffer;

 public:
    // New activity score in pipeline
    void push(double activityScore);

    // Calculate filter score at offset position
    double filter() const;

    // Initialize buffer and
    FilterBuffer(const std::function<double(size_t, size_t)>& kernelBuilder,
                 size_t kernelSize
    );

    // Create dummy buffer
    FilterBuffer();

    // Buffer size
    size_t getSize() const;

    // Distance between buffer center and borders
    size_t getOffset() const;
};

// -----------------------------------------------------------------------------

}  // namespace calq

// -----------------------------------------------------------------------------

#endif  // CALQ_FILTER_BUFFER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
