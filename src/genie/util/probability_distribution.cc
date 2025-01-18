/**
 * Copyright 2018-2024 The Genie Authors.
 * @file probability_distribution.cc
 * @brief Implementation file for the ProbabilityDistribution class.
 *
 * This file provides the implementation of the `ProbabilityDistribution` class,
 * which handles and manipulates probability distributions. The class allows for
 * constructing, modifying, and querying discrete probability distributions.
 *
 * @details The `ProbabilityDistribution` class operates on a range of values
 * specified during instantiation. It maintains a vector of counts for each
 * value within this range and supports incrementing and resetting these counts.
 * The class is primarily used for analyzing the distribution of quality scores
 * within a specific interval and for constructing probability models.
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/util/probability_distribution.h"

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

ProbabilityDistribution::ProbabilityDistribution(const size_t range_min,
                                                 const size_t range_max) {
  this->range_min_ = range_min;
  this->pdf_.resize(range_max - range_min + 1);
}

// -----------------------------------------------------------------------------

void ProbabilityDistribution::AddToPdf(const size_t qual_score,
                                       const size_t number) {
  UTILS_DIE_IF(
      qual_score < range_min_ || qual_score > range_min_ + pdf_.size() - 1,
      "PDF: Score not in range");
  pdf_[qual_score - range_min_] += number;
}

// -----------------------------------------------------------------------------

void ProbabilityDistribution::ResetPdf() {
  std::fill(pdf_.begin(), pdf_.end(), 0);
}

// -----------------------------------------------------------------------------

size_t ProbabilityDistribution::GetCount(const size_t value) const {
  UTILS_DIE_IF(value < range_min_ || value > range_min_ + pdf_.size() - 1,
               "PDF: Value not in range");
  return value;
}

// -----------------------------------------------------------------------------

size_t ProbabilityDistribution::operator[](const size_t index) const {
  UTILS_DIE_IF(index >= pdf_.size(), "PDF: Index not in range");
  return pdf_[index];
}

// -----------------------------------------------------------------------------

size_t ProbabilityDistribution::size() const { return pdf_.size(); }

// -----------------------------------------------------------------------------

size_t ProbabilityDistribution::GetRangeMin() const { return range_min_; }

// -----------------------------------------------------------------------------

size_t ProbabilityDistribution::GetRangeMax() const {
  return range_min_ + pdf_.size() - 1;
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
