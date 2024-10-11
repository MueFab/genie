/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/lloyd_max_quantizer.h"

// -----------------------------------------------------------------------------

#include <algorithm>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

void LloydMaxQuantizer::fillLUT(const ProbabilityDistribution& pdf) {
    size_t index = 0;
    size_t pos = pdf.getRangeMin();

    while (index < borders.size()) {
        while (pos <= pdf.getRangeMax()) {
            if (pos >= borders[index]) {
                break;
            }
            this->lut_[static_cast<int>(pos)] =
                std::pair<int, int>(static_cast<const int&>(index), static_cast<const int&>(std::round(values[index])));
            pos += 1;
        }
        index += 1;
    }

    for (index = 0; index < borders.size(); ++index) {
        inverseLut_[static_cast<int>(index)] = static_cast<int>(std::round(values[index]));
    }
}

// -----------------------------------------------------------------------------

double LloydMaxQuantizer::calcCentroid(const size_t left, size_t right, const ProbabilityDistribution& pdf) {
    double sum = 0;
    double weightSum = 0;

    const double THRESHOLD = 0.01;  // Avoid division by zero

    if (right == pdf.getRangeMax() + 1) {
        sum += pdf[pdf.size() - 1];
        weightSum += right * pdf[pdf.size() - 1];
        right -= 1;
    }

    for (size_t i = left; i <= right; ++i) {
        sum += pdf[i - pdf.getRangeMin()];
        weightSum += i * pdf[i - pdf.getRangeMin()];
    }

    return sum > THRESHOLD ? weightSum / sum : std::floor((left + right) / 2.0);
}

// -----------------------------------------------------------------------------

void LloydMaxQuantizer::calcBorders(const ProbabilityDistribution& pdf) {
    // Step 1: Init
    const double stepSize = pdf.size() / static_cast<double>(steps);
    for (size_t i = 0; i < steps; ++i) {
        borders[i] = pdf.getRangeMin() + (i + 1) * stepSize;
        values[i] = pdf.getRangeMin() + i * stepSize + stepSize / 2.0;
    }

    double change = 0.0;

    // Step 2: Lloyd's II. algorithm
    for (int k = 0; k < static_cast<int>(borders.size()); ++k) {
        const double left = k == 0 ? pdf.getRangeMin() : borders[k - 1];
        const double right = borders[k];

        // Calc centroid
        const double centroid = calcCentroid(static_cast<size_t>(ceil(left)), static_cast<size_t>(floor(right)), pdf);

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

        borders[k] = (values[k] + values[k + 1]) / 2;
    }
}

// -----------------------------------------------------------------------------

LloydMaxQuantizer::LloydMaxQuantizer(const size_t _steps) {
    this->steps = _steps;
    borders.resize(_steps, 0);
    values.resize(_steps, 0);
}

// -----------------------------------------------------------------------------

void LloydMaxQuantizer::build(const ProbabilityDistribution& pdf) {
    calcBorders(pdf);
    fillLUT(pdf);
}

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
