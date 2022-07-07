#include "filter_buffer.h"

// -----------------------------------------------------------------------------

#include <cmath>

// -----------------------------------------------------------------------------

#include <algorithm>

// -----------------------------------------------------------------------------

#include "error_exception_reporter.h"

// -----------------------------------------------------------------------------

namespace calq {

// -----------------------------------------------------------------------------

GaussKernel::GaussKernel(double sigma)
        : SIGMA(sigma),
        INV_SQRT_SIGMA_2PI(1.0 / (std::sqrt(2.0 * PI) * sigma)){
}

// -----------------------------------------------------------------------------

double GaussKernel::calcValue(size_t pos,
                              size_t size
) const{
    const double MEAN = std::floor((size - 1) / 2.0);
    double exponent = (pos - MEAN) / SIGMA;
    exponent = exponent * exponent * (-0.5);
    return INV_SQRT_SIGMA_2PI * std::pow(EULER, exponent);
}

// -----------------------------------------------------------------------------

size_t GaussKernel::calcMinSize(double threshold,
                                size_t maximum
) const{
    threshold /= INV_SQRT_SIGMA_2PI;
    threshold = std::log(threshold);
    threshold *= -2.0;
    threshold = std::sqrt(threshold) * SIGMA;  // Euler now reversed

    // + 1 to make sure it is odd.
    auto size = static_cast<size_t>(std::ceil(threshold) * 2 + 1);
    return std::min(size, maximum);
}

// -----------------------------------------------------------------------------

// New activity score in pipeline
void FilterBuffer::push(double activityScore){
    buffer.push(activityScore);
}

// -----------------------------------------------------------------------------

// Calculate filter score at offset position
double FilterBuffer::filter() const{
    double result = 0.0;
    for (size_t i = 0; i < kernel.size(); ++i) {
        result += kernel[i] * buffer[i];
        // std::cout << kernel[i] << " " << buffer[i] << std::endl;
    }
    return result;
}

// -----------------------------------------------------------------------------

// Initialize buffer and
FilterBuffer::FilterBuffer(const std::function<
        double(size_t, size_t)>& kernelBuilder,
                           size_t kernelSize
)
        : buffer(
        kernelSize,
        0.0
){
    if (!(kernelSize % 2)) {
        throwErrorException("Kernel size must be an odd number");
    }
    kernel.resize(kernelSize, 0.0);

    for (size_t i = 0; i < kernel.size(); ++i) {
        kernel[i] = kernelBuilder(i, kernelSize);
    }
}

// -----------------------------------------------------------------------------

FilterBuffer::FilterBuffer()
        : buffer(
        1,
        0.0
){
}

// -----------------------------------------------------------------------------

size_t FilterBuffer::getSize() const{
    return buffer.size();
}

// -----------------------------------------------------------------------------

size_t FilterBuffer::getOffset() const{
    return (buffer.size() + 1) / 2;
}

// -----------------------------------------------------------------------------

RectangleKernel::RectangleKernel(double size)
        : SIZE(size){
}

// -----------------------------------------------------------------------------

double RectangleKernel::calcValue(size_t pos,
                                  size_t size
) const{
    const double MEAN = std::floor((size - 1) / 2.0);
    return (pos - MEAN) <= SIZE ? 1.0 : 0.0;
}

// -----------------------------------------------------------------------------

size_t RectangleKernel::calcMinSize(size_t maximum) const{
    return (size_t) std::min(SIZE * 2 + 1, static_cast<double>(maximum));
}

// -----------------------------------------------------------------------------

}  // namespace calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
