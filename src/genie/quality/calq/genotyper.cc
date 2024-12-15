/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/genotyper.h"

// -----------------------------------------------------------------------------

#include <cmath>

// -----------------------------------------------------------------------------

#include <iomanip>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// -----------------------------------------------------------------------------

#include "genie/util/runtime_exception.h"
#include "genie/util/log.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "Calq";

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

static int CombinationsWithRepetitions(  // NOLINT
    std::vector<std::string>* geno_alphabet,
    const std::vector<char>& allele_alphabet, int* got, const int n_chosen,
    const int len, const int at, const int max_types) {
  if (n_chosen == len) {
    if (!got) {
      return 1;
    }
    std::string tmp;
    for (int i = 0; i < len; i++) {
      tmp += allele_alphabet[got[i]];
    }
    geno_alphabet->push_back(tmp);
    return 1;
  }

  int count = 0;
  for (int i = at; i < max_types; i++) {
    if (got) {
      got[n_chosen] = i;
    }
    count += CombinationsWithRepetitions(geno_alphabet, allele_alphabet, got,
                                         n_chosen + 1, len, i, max_types);
  }

  return count;
}

// -----------------------------------------------------------------------------

Genotyper::Genotyper(const int polyploidy, const int qual_offset,
                     const int nr_quantizers, const bool debug)
    : current_allele_alphabet_(allele_alphabet_),
      nr_quantizers_(nr_quantizers),
      polyploidy_(polyploidy),
      qual_offset_(qual_offset),
      debug_(debug) {
  UTILS_DIE_IF(nr_quantizers < 1, "nrQuantizers must be greater than zero");
  UTILS_DIE_IF(polyploidy < 1, "polyploidy must be greater than zero");
  UTILS_DIE_IF(qual_offset < 0, "qualOffset must not be negative");

  InitLikelihoods();
}

// -----------------------------------------------------------------------------

Genotyper::~Genotyper() = default;

// -----------------------------------------------------------------------------

double Genotyper::ComputeEntropy(const std::string& seq_pileup,
                                 const std::string& qual_pileup) {
  const size_t depth = seq_pileup.length();

  UTILS_DIE_IF(depth != qual_pileup.length(),
               "Lengths of seqPileup and qualPileup differ");

  if (depth == 0) {
    return -1.0;  // computation of entropy not possible
  }
  if (depth == 1) {
    return 0.0;  // no information content for one symbol
  }

  ComputeGenotypeLikelihoods(seq_pileup, qual_pileup, depth);

  const double entropy = std::accumulate(
      genotype_likelihoods_.begin(), genotype_likelihoods_.end(), 0.0,
      [](const double& a, const std::pair<std::string, double>& b) {
        return a - b.second * log(b.second);
      });

  return entropy;
}

// -----------------------------------------------------------------------------

int Genotyper::ComputeQuantizerIndex(const std::string& seq_pileup,
                                     const std::string& qual_pileup) {
  const size_t depth = seq_pileup.length();

  UTILS_DIE_IF(depth != qual_pileup.length(),
               "Lengths of seqPileup and qualPileup differ");

  if (depth == 0) {
    return nr_quantizers_;  // computation of quantizer index not possible
  }
  if (depth == 1) {
    return nr_quantizers_ - 1;  // no inference can be made, stay safe
  }

  ComputeGenotypeLikelihoods(seq_pileup, qual_pileup, depth);

  double largest_genotype_likelihood = 0.0;
  double second_largest_genotype_likelihood = 0.0;
  for (const auto& [fst, snd] : genotype_likelihoods_) {
    if (snd > second_largest_genotype_likelihood) {
      second_largest_genotype_likelihood = snd;
    }
    if (second_largest_genotype_likelihood > largest_genotype_likelihood) {
      second_largest_genotype_likelihood = largest_genotype_likelihood;
      largest_genotype_likelihood = snd;
    }
  }

  const double confidence =
      largest_genotype_likelihood - second_largest_genotype_likelihood;

  const auto quant = static_cast<int>((1 - confidence) * (nr_quantizers_ - 1));

  if (debug_) {
    std::stringstream s;
    s << 'N' << " " << seq_pileup << " ";

    s << std::fixed << std::setw(6) << std::setprecision(4) << std::setfill('0')
      << 1 - confidence;

    s << " " << std::fixed << std::setw(6) << std::setprecision(4)
      << std::setfill('0') << 1 - confidence << " " << quant << std::endl;

    UTILS_LOG(util::Logger::Severity::INFO, s.str());
  }

  return quant;
}

// -----------------------------------------------------------------------------

void Genotyper::InitLikelihoods() {
  // Initialize map containing the allele likelihoods
  for (const auto& allele : current_allele_alphabet_) {
    allele_likelihoods_.insert(std::pair(allele, 0.0));
  }

  // Initialize map containing the genotype likelihoods
  std::vector chosen(polyploidy_, 0);
  CombinationsWithRepetitions(&genotype_alphabet_, current_allele_alphabet_,
                              chosen.data(), 0, polyploidy_, 0,
                              allele_alphabet_size_);

  // Initialize genotype alphabet
  for (auto& genotype : genotype_alphabet_) {
    genotype_likelihoods_.insert(std::pair(genotype, 0.0));
  }
}

// -----------------------------------------------------------------------------

void Genotyper::ResetLikelihoods() {
  for (auto& [fst, snd] : genotype_likelihoods_) {
    snd = 0.0;
  }

  for (auto& [fst, snd] : allele_likelihoods_) {
    snd = 0.0;
  }
}

// -----------------------------------------------------------------------------

void Genotyper::ComputeGenotypeLikelihoods(const std::string& seq_pileup,
                                           const std::string& qual_pileup,
                                           const size_t& depth) {
  ResetLikelihoods();

  auto* temp_genotype_likelihoods =
      static_cast<double*>(calloc(genotype_alphabet_.size(), sizeof(double)));
  int itr = 0;
  for (size_t d = 0; d < depth; d++) {
    const auto y = static_cast<char>(seq_pileup[d]);
    const auto q = static_cast<double>(qual_pileup[d] - qual_offset_);

    double p_strike = 1 - pow(10.0, -q / 10.0);
    double p_error = (1 - p_strike) / (allele_alphabet_size_ - 1);

    itr = 0;
    for (const auto& genotype : genotype_alphabet_) {
      double p = 0.0;
      for (int i = 0; i < polyploidy_; i++) {
        p += y == genotype[i] ? p_strike : p_error;
      }
      p /= polyploidy_;

      // We are using the Log likelihood to avoid numerical problems
      temp_genotype_likelihoods[itr++] += log(p);
    }
  }
  itr = 0;
  for (const auto& genotype : genotype_alphabet_) {
    genotype_likelihoods_[genotype] = temp_genotype_likelihoods[itr++];
  }
  free(temp_genotype_likelihoods);

  // Normalize the genotype likelihoods
  double cum = 0.0;
  for (auto& [fst, snd] : genotype_likelihoods_) {
    snd = exp(snd);
    cum += snd;
  }
  for (auto& [fst, snd] : genotype_likelihoods_) {
    snd /= cum;
  }
}

// -----------------------------------------------------------------------------

const std::map<std::string, double>& Genotyper::GetGenotypelikelihoods(
    const std::string& seq_pileup, const std::string& qual_pileup) {
  ComputeGenotypeLikelihoods(seq_pileup, qual_pileup, qual_pileup.size());
  return genotype_likelihoods_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
