/** @file UniformMinMaxQuantizer.cc
 *  @brief This file contains the implementation of the UniformMinMaxQuantizer
 *         class.
 */

// Copyright 2015-2017 Leibniz Universitaet Hannover

#include "QualCodec/Quantizers/UniformMinMaxQuantizer.h"

namespace calq {

UniformMinMaxQuantizer::UniformMinMaxQuantizer(const int &valueMin, const int &valueMax, const int &nrSteps)
    : UniformQuantizer(valueMin, valueMax, nrSteps) {
    // Change the smallest and largest reconstruction values

    int smallestIndex = 0;
    int largestIndex = nrSteps - 1;

    for (auto &lutElem : lut_) {
        int currentIndex = lutElem.second.first;
        if (currentIndex == smallestIndex) {
            lutElem.second.second = valueMin;
        }
        if (currentIndex == largestIndex) {
            lutElem.second.second = valueMax;
        }
    }

    for (auto &inverseLutElem : inverseLut_) {
        int currentIndex = inverseLutElem.first;
        if (currentIndex == smallestIndex) {
            inverseLutElem.second = valueMin;
        }
        if (currentIndex == largestIndex) {
            inverseLutElem.second = valueMax;
        }
    }
}

UniformMinMaxQuantizer::~UniformMinMaxQuantizer(void) {}

}  // namespace calq

