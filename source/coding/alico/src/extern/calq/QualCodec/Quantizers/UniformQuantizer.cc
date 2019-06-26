/** @file UniformQuantizer.cc
 *  @brief This file contains the implementation of the UniformQuantizer class.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#include "QualCodec/Quantizers/UniformQuantizer.h"

#include <math.h>

#include <queue>
#include <utility>

#include "Common/Exceptions.h"

namespace calq {

UniformQuantizer::UniformQuantizer(const int &valueMin, const int &valueMax, const int &nrSteps)
    : Quantizer() {
    if ((valueMin > valueMax) || (nrSteps <= 1)) {
        throwErrorException("Error in quantizer initialization");
    }

    // Compute the step size
    int stepSize = (int)floor(((double)(valueMax - valueMin)) / ((double)nrSteps));

    // Compute the borders and the representative values
    std::queue<int> borders;
    std::queue<int> reconstructionValues;
    int newBorder = valueMin;
    borders.push(valueMin);
    reconstructionValues.push(valueMin + (int)round(((double)stepSize)/((double)2)));
    for (int i = 0; i < (nrSteps-1); i++) {
        newBorder += stepSize;
        borders.push(newBorder);
        reconstructionValues.push(newBorder + (int)round(((double)stepSize)/((double)2)));
    }
    borders.push(valueMax);

    // Fill the quantization table
    borders.pop();
    int currentIndex = 0;
    int currentReconstructionValue = reconstructionValues.front();
    int currentBorder = borders.front();
    for (int value = valueMin; value <= valueMax; ++value) {
        if (value > currentBorder) {
            currentIndex++;
            reconstructionValues.pop();
            borders.pop();
            currentReconstructionValue = reconstructionValues.front();
            currentBorder = borders.front();
        }
        std::pair<int, int> curr(currentIndex, currentReconstructionValue);
        lut_.insert(std::pair<int, std::pair<int, int>>(value, curr));
        inverseLut_.insert(curr);
    }
}

UniformQuantizer::~UniformQuantizer(void) {}

}  // namespace calq

