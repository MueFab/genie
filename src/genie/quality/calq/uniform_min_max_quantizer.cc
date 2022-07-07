#include "uniform_min_max_quantizer.h"

// -----------------------------------------------------------------------------

namespace calq {

// -----------------------------------------------------------------------------

UniformMinMaxQuantizer::UniformMinMaxQuantizer(const int& valueMin,
                                               const int& valueMax,
                                               const int& nrSteps
)
        : UniformQuantizer(
        valueMin,
        valueMax,
        nrSteps
){
    // Change the smallest and largest reconstruction values

    int smallestIndex = 0;
    int largestIndex = nrSteps - 1;

    for (auto& lutElem : lut_) {
        int currentIndex = lutElem.second.first;
        if (currentIndex == smallestIndex) {
            lutElem.second.second = valueMin;
        }
        if (currentIndex == largestIndex) {
            lutElem.second.second = valueMax;
        }
    }

    for (auto& inverseLutElem : inverseLut_) {
        int currentIndex = inverseLutElem.first;
        if (currentIndex == smallestIndex) {
            inverseLutElem.second = valueMin;
        }
        if (currentIndex == largestIndex) {
            inverseLutElem.second = valueMax;
        }
    }
}

// -----------------------------------------------------------------------------

UniformMinMaxQuantizer::~UniformMinMaxQuantizer() = default;

// -----------------------------------------------------------------------------

}  // namespace calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
