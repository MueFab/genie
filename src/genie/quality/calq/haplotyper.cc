/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/haplotyper.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "genie/quality/calq/calq_coder.h"
#include "genie/quality/calq/error_exception_reporter.h"
#include "genie/quality/calq/log.h"

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

// Returns score, takes sequence and quality pileup and position
Haplotyper::Haplotyper(const size_t sigma, const size_t ploidy,
                       const size_t quality_offset, const size_t nr_quantizers,
                       const size_t max_hq_soft_clip_propagation,
                       const size_t min_hq_soft_clip_streak,
                       const size_t filter_cut_off, const bool debug,
                       const bool squashed, const FilterType filter_type)
    : spreader_(max_hq_soft_clip_propagation, min_hq_soft_clip_streak,
                squashed),
      genotyper_(static_cast<int>(ploidy), static_cast<int>(quality_offset),
                 static_cast<int>(nr_quantizers), debug),
      nr_quantizers_(nr_quantizers),
      polyploidy_(ploidy),
      debug_(debug),
      squashed_activity_(squashed) {
  if (filter_type == FilterType::GAUSS) {
    GaussKernel kernel(static_cast<double>(sigma));
    constexpr double threshold = 0.0000001;
    const size_t size = kernel.CalcMinSize(threshold, filter_cut_off * 2 + 1);

    buffer_ = FilterBuffer(
        [kernel](const size_t pos, const size_t kernel_size) -> double {
          return kernel.CalcValue(pos, kernel_size);
        },
        size);
    local_distortion_ = kernel.CalcValue((size - 1) / 2, size);
  } else if (filter_type == FilterType::RECTANGLE) {
    RectangleKernel kernel(static_cast<double>(sigma));
    const size_t size = kernel.CalcMinSize(filter_cut_off * 2 + 1);

    buffer_ = FilterBuffer(
        [kernel](const size_t pos, const size_t kernel_size) -> double {
          return kernel.CalcValue(pos, kernel_size);
        },
        size);
    local_distortion_ = kernel.CalcValue((size - 1) / 2, size);
  } else {
    THROW_ERROR_EXCEPTION("FilterType not supported by haplotyper");
  }
}

// -----------------------------------------------------------------------------

size_t Haplotyper::GetOffset() const {
  return buffer_.GetOffset() + spreader_.GetOffset() - 1;
}

// -----------------------------------------------------------------------------

double log10sum(const double a, const double b) {  // NOLINT
  if (a > b) {
    return log10sum(b, a);
  }
  if (a == -std::numeric_limits<double>::infinity()) {
    return b;
  }
  return b + log10(1 + pow(10.0, -(b - a)));
}

// -----------------------------------------------------------------------------

// Calc priors like in GATK
std::vector<double> Haplotyper::CalcPriors(double hetero) const {
  hetero = log10(hetero);
  std::vector result(polyploidy_ + 1, hetero);
  double sum = -std::numeric_limits<double>::infinity();
  for (size_t i = 1; i < polyploidy_ + 1; ++i) {
    result[i] -= log10(static_cast<double>(i));
    sum = log10sum(sum, result[i]);
  }
  result[0] = log10(1.0 - pow(10, sum));

  return result;
}

// -----------------------------------------------------------------------------

std::vector<double> Haplotyper::CalcNonRefLikelihoods(
    const char ref, const std::string& seq_pile,
    const std::string& quality_pile) {
  std::vector result(polyploidy_ + 1, 0.0);
  const std::map<std::string, double> snp_likelihoods =
      genotyper_.GetGenotypelikelihoods(seq_pile, quality_pile);

  for (const auto& [fst, snd] : snp_likelihoods) {
    const size_t alt_count =
        polyploidy_ - std::count(fst.begin(), fst.end(), ref);
    result[alt_count] += snd;
  }

  for (double& i : result) {
    i = log10(i);
  }

  return result;
}

// -----------------------------------------------------------------------------

double Haplotyper::CalcActivityScore(const char ref,
                                     const std::string& seq_pile,
                                     const std::string& quality_pile,
                                     const double heterozygosity) {
  if (ref == 'N') {
    return 1.0;
  }

  const std::vector<double> likelihoods =
      CalcNonRefLikelihoods(ref, seq_pile, quality_pile);
  static std::vector<double> priors;
  if (priors.empty()) {
    priors = CalcPriors(heterozygosity);
  }

  // --------------Calc Posteriors like in GATK ------------------------------
  double posteriori0 = likelihoods[0] + priors[0];
  bool map0 = true;
  for (size_t i = 1; i < polyploidy_ + 1; ++i) {
    if (likelihoods[i] + priors[i] > posteriori0) {
      map0 = false;
      break;
    }
  }

  if (map0) {
    return 0.0;
  }

  double alt_likelihood_sum = -std::numeric_limits<double>::infinity();
  double alt_prior_sum = -std::numeric_limits<double>::infinity();

  for (size_t i = 1; i < polyploidy_ + 1; ++i) {
    alt_likelihood_sum = log10sum(alt_likelihood_sum, likelihoods[i]);
    alt_prior_sum = log10sum(alt_prior_sum, priors[i]);
  }

  const double alt_posterior_sum = alt_likelihood_sum + alt_prior_sum;
  // Normalize
  posteriori0 = posteriori0 - log10sum(alt_posterior_sum, posteriori0);

  // -------------------------------------------------------------------------

  /*  const double CUTOFF = -1.0;

    if (posteriori0 > CUTOFF)
        return 0.0;*/

  return 1.0 - pow(10, posteriori0);
}

// -----------------------------------------------------------------------------

size_t Haplotyper::push(const std::string& seq_pile,
                        const std::string& quality_pile,
                        const size_t high_quality_soft_clips,
                        const char reference) {
  // Empty input
  if (seq_pile.empty()) {
    buffer_.push(spreader_.push(0.0, 0));
    return 0;
  }

  // Build reference string

  constexpr double heterozygosity = 1.0 / 1000.0;

  const double alt_prob =
      CalcActivityScore(reference, seq_pile, quality_pile, heterozygosity);

  // Filter activity score
  buffer_.push(
      spreader_.push(alt_prob, high_quality_soft_clips / quality_pile.size()));
  double activity = buffer_.filter();
  if (squashed_activity_) {
    activity = std::min(activity, 1.0);
  }

  const size_t quant = GetQuantizerIndex(activity);

  if (debug_) {
    static CircularBuffer<std::string> debug(this->GetOffset(), "\n");
    std::stringstream s;

    s << reference << " " << seq_pile << " ";

    s << std::fixed << std::setw(6) << std::setprecision(4) << std::setfill('0')
      << alt_prob;

    const std::string out = debug.push(s.str());

    s.str("");
    if (out != "\n") {
      s << out << " " << std::fixed << std::setw(6) << std::setprecision(4)
        << std::setfill('0') << activity << " " << quant << std::endl;
    }

    if (high_quality_soft_clips > 0) {
      s << high_quality_soft_clips << " soft clips detected!" << std::endl;
    }

    std::string line;
    while (std::getline(s, line)) {
      GetLogging().error_out(line);
    }
  }

  return quant;
}

// -----------------------------------------------------------------------------

size_t Haplotyper::GetQuantizerIndex(const double activity) const {
  return static_cast<size_t>(
      std::min(std::floor(activity / local_distortion_ *
                          static_cast<double>(nr_quantizers_)),
               static_cast<double>(nr_quantizers_ - 1)));
}

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
