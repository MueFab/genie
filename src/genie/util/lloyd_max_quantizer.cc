/**
 * Copyright 2018-2024 The Genie Authors.
 * @file lloyd_max_quantizer.cc
 *
 * @brief Implementation file for the LloydMaxQuantizer class.
 *
 * This file provides the implementation of the `LloydMaxQuantizer` class,
 * which performs Lloyd-Max quantization on a range of values. The class
 * calculates optimal quantization intervals and representative values
 * using the Lloyd-Max algorithm.
 *
 * @details The `LloydMaxQuantizer` class is useful for quantization tasks
 * where the distribution of values is known. It includes methods for
 * initializing the quantizer, calculating centroids, decision thresholds,
 * and filling quantization tables.
 *
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/util/lloyd_max_quantizer.h"

// -----------------------------------------------------------------------------

#include <algorithm>
#include <cmath>
#include <utility>

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

void LloydMaxQuantizer::FillLut(const ProbabilityDistribution& pdf) {
  size_t index = 0;
  size_t pos = pdf.GetRangeMin();

  while (index < borders_.size()) {
    while (pos <= pdf.GetRangeMax()) {
      if (static_cast<double>(pos) >= borders_[index]) {
        break;
      }
      this->lut_[static_cast<int>(pos)] =
          std::pair(static_cast<int>(index),
                    static_cast<int>(std::round(values_[index])));
      pos += 1;
    }
    index += 1;
  }

  for (index = 0; index < borders_.size(); ++index) {
    inverse_lut_[static_cast<int>(index)] =
        static_cast<int>(std::round(values_[index]));
  }
}

// -----------------------------------------------------------------------------

double LloydMaxQuantizer::CalcCentroid(const size_t left, size_t right,
                                       const ProbabilityDistribution& pdf) {
  double sum = 0;
  double weight_sum = 0;

  constexpr double threshold = 0.01;  // Avoid division by zero

  if (right == pdf.GetRangeMax() + 1) {
    sum += static_cast<double>(pdf[pdf.size() - 1]);
    weight_sum +=
        static_cast<double>(right) * static_cast<double>(pdf[pdf.size() - 1]);
    right -= 1;
  }

  for (size_t i = left; i <= right; ++i) {
    sum += static_cast<double>(pdf[i - pdf.GetRangeMin()]);
    weight_sum += static_cast<double>(i) *
                  static_cast<double>(pdf[i - pdf.GetRangeMin()]);
  }

  return sum > threshold ? weight_sum / sum
                         : std::floor(static_cast<double>(left + right) / 2.0);
}

// -----------------------------------------------------------------------------

void LloydMaxQuantizer::CalcBorders(const ProbabilityDistribution& pdf) {
  // Step 1: Init
  const double step_size =
      static_cast<double>(pdf.size()) / static_cast<double>(steps_);
  for (size_t i = 0; i < steps_; ++i) {
    borders_[i] = static_cast<double>(pdf.GetRangeMin()) +
                  static_cast<double>(i + 1) * step_size;
    values_[i] = static_cast<double>(pdf.GetRangeMin()) +
                 static_cast<double>(i) * step_size + step_size / 2.0;
  }

  double change = 0.0;

  // Step 2: Lloyd's II. algorithm
  for (int k = 0; k < static_cast<int>(borders_.size()); ++k) {
    const double left =
        k == 0 ? static_cast<double>(pdf.GetRangeMin()) : borders_[k - 1];
    const double right = borders_[k];

    // Calc centroid
    const double centroid =
        CalcCentroid(static_cast<size_t>(ceil(left)),
                     static_cast<size_t>(floor(right)), pdf);

    change = std::max(change, std::abs(values_[k] - centroid));
    values_[k] = centroid;

    if (k == static_cast<int>(borders_.size() - 1)) {
      if (constexpr double epsilon = 0.05; change < epsilon) {
        break;
      }
      k = -1;
      change = 0.0;
      continue;
    }

    borders_[k] = (values_[k] + values_[k + 1]) / 2;
  }
}

// -----------------------------------------------------------------------------

LloydMaxQuantizer::LloydMaxQuantizer(const size_t steps) {
  this->steps_ = steps;
  borders_.resize(steps, 0);
  values_.resize(steps, 0);
}

// -----------------------------------------------------------------------------

void LloydMaxQuantizer::build(const ProbabilityDistribution& pdf) {
  CalcBorders(pdf);
  FillLut(pdf);
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
