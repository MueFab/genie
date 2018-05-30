/** @file LloydMaxQuantizer.cc
 *  @brief This file contains implementation of a Lloyd-Max-Quantizer
 */

// Copyright 2015-2018 Leibniz Universitaet Hannover

#include "QualCodec/Quantizers/LloydMaxQuantizer.h"

#include <utility>
#include <algorithm>

// ----------------------------------------------------------------------------------------------------------------------

namespace calq {
void LloydMaxQuantizer::fillLUT() {
    size_t index = 0;
    size_t pos = 0;

    while (index < borders.size()) {
        while (pos < pdf.size()) {
            if (pos + rangeMin >= borders[index]) {
                break;
            }
            this->lut_[pos+rangeMin] = std::pair<int, int> (index, std::round(values[index]));
            pos += 1;
        }
        index += 1;
    }

    for (size_t index = 0; index < borders.size(); ++index) {
        inverseLut_[index] = std::round(values[index]);
    }
}

// ----------------------------------------------------------------------------------------------------------------------

double LloydMaxQuantizer::calcCentroid(size_t left, size_t right) {
    double sum = 0;
    double weightSum = 0;

    if (right == pdf.size() + rangeMin) {
        sum += pdf[pdf.size() - 1];
        weightSum += (right) * pdf[pdf.size() - 1];
        right -= 1;
    }

    for (size_t i = left; i <= right; ++i) {
        sum += pdf[i - rangeMin];
        weightSum += i * pdf[i - rangeMin];
    }

    return weightSum / sum;
}

// ----------------------------------------------------------------------------------------------------------------------

void LloydMaxQuantizer::calcBorders() {
    // Step 1: Init
    double stepSize = pdf.size() / static_cast<double>(steps);
    for (size_t i = 0; i < steps; ++i) {
        borders[i] = rangeMin + (i + 1) * stepSize;
        values[i] = rangeMin + i * stepSize + stepSize / 2.0;
    }

    double change = 0.0;

    // Step 2: Lloyd's II. algorithm
    for (int k = 0; k < static_cast<int>(borders.size()); ++k) {
        double left = (k == 0) ? rangeMin : borders[k-1];
        double right = borders[k];

        // Calc centroid
        double centroid = calcCentroid(ceil(left), floor(right));

        change = std::max(change, std::abs(values[k] - centroid));
        values[k] = centroid;

        if (k == static_cast<int>(borders.size() - 1)) {
            constexpr double EPSILON = 0.05;
            if (change < EPSILON) {
                break;
            }
            k = -1;
            change = 0.0;
            continue;
        }

        borders[k] = (values[k] + values[k+1])/2;
    }
}

// ----------------------------------------------------------------------------------------------------------------------

LloydMaxQuantizer::LloydMaxQuantizer(size_t rangeMin, size_t rangeMax, size_t steps) {
    this->steps = steps;
    this->rangeMin = rangeMin;
    borders.resize(steps, 0);
    values.resize(steps, 0);
    pdf.resize(rangeMax-rangeMin + 1, 0);
}

// ----------------------------------------------------------------------------------------------------------------------

void LloydMaxQuantizer::addToPdf(size_t qualScore, size_t number) {
    pdf[qualScore-rangeMin] += number;
}

// ----------------------------------------------------------------------------------------------------------------------

void LloydMaxQuantizer::resetPdf() {
    for (size_t i = 0; i < pdf.size(); ++i) {
        pdf[i] = 0;
    }
}

// ----------------------------------------------------------------------------------------------------------------------

void LloydMaxQuantizer::build() {
    calcBorders();
    fillLUT();
}

// ----------------------------------------------------------------------------------------------------------------------

std::vector<double> LloydMaxQuantizer::exportValues() {
    return values;
}
}  // namespace calq

// ----------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------------
