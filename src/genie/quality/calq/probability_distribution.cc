/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/probability_distribution.h"

#include "genie/quality/calq/error_exception_reporter.h"

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

ProbabilityDistribution::ProbabilityDistribution(const size_t range_min,
                                                 const size_t range_max) {
  this->range_min_ = range_min;
  this->pdf_.resize(range_max - range_min + 1);
}

// -----------------------------------------------------------------------------

void ProbabilityDistribution::AddToPdf(const size_t qual_score,
                                       const size_t number) {
  if (qual_score < range_min_ || qual_score > range_min_ + pdf_.size() - 1)
    THROW_ERROR_EXCEPTION("PDF: Score not in range");
  pdf_[qual_score - range_min_] += number;
}

// -----------------------------------------------------------------------------

void ProbabilityDistribution::ResetPdf() {
  std::fill(pdf_.begin(), pdf_.end(), 0);
}

// -----------------------------------------------------------------------------

size_t ProbabilityDistribution::GetCount(const size_t value) const {
  if (value < range_min_ || value > range_min_ + pdf_.size() - 1)
    THROW_ERROR_EXCEPTION("PDF: Value not in range");
  return value;
}

// -----------------------------------------------------------------------------

size_t ProbabilityDistribution::operator[](const size_t index) const {
  if (index >= pdf_.size()) THROW_ERROR_EXCEPTION("PDF: Index not in range");
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

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
