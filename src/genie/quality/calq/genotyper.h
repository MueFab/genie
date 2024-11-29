/**
 * Copyright 2018-2024 The Genie Authors.
 * @file genotyper.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Contains the declaration of the `Genotyper` class for genotype
 * likelihood calculation.
 *
 * This file provides the declaration of the `Genotyper` class, which is used to
 * compute genotype likelihoods and select quantizer indices for given sequence
 * and quality value pileups. It also calculates the entropy of the pileup to
 * assess the amount of information contained.
 *
 * @details The `Genotyper` class is primarily used in the context of CALQ
 * quality value compression to select optimal quantizers based on genotype
 * certainty. It maintains internal states for allele and genotype likelihoods
 * and supports various configurations, such as ploidy and quality value
 * offsets.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_GENOTYPER_H_
#define SRC_GENIE_QUALITY_CALQ_GENOTYPER_H_

// -----------------------------------------------------------------------------

#include <map>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

/**
 * @brief Computes genotype likelihoods and selects quantizer indices for given
 * pileups.
 *
 * The `Genotyper` class provides functionalities for calculating genotype
 * likelihoods based on input sequence and quality value pileups. It maintains
 * internal data structures for allele and genotype likelihoods and provides
 * methods for entropy calculation and quantizer index selection.
 */
class Genotyper {
 public:
  /**
   * @brief Constructs a `Genotyper` object with specified parameters.
   *
   * @param polyploidy The number of alleles considered (e.g., 2 for diploid
   * organisms).
   * @param qual_offset The quality value offset used in the input quality
   * values (e.g., 33 for Phred+33).
   * @param nr_quantizers The number of quantizers available for quality value
   * compression.
   * @param debug Flag indicating whether to output detailed debug
   * information.
   */
  Genotyper(int polyploidy, int qual_offset, int nr_quantizers, bool debug);

  /**
   * @brief Destroys the `Genotyper` object and releases any resources.
   */
  ~Genotyper();

  /**
   * @brief Computes the entropy of the given sequence and quality value
   * pileup.
   *
   * This method calculates the entropy of the input pileup, which is a
   * measure of the uncertainty or information content in the data.
   *
   * @param seq_pileup The sequence pileup represented as a string (e.g.,
   * "ACGTACGT").
   * @param qual_pileup The corresponding quality value pileup represented as a
   * string.
   * @return The computed entropy of the pileup.
   */
  double ComputeEntropy(const std::string& seq_pileup,
                        const std::string& qual_pileup);

  /**
   * @brief Computes the optimal quantizer index for the given sequence and
   * quality value pileup.
   *
   * This method analyzes the input pileup and returns the index of the
   * quantizer that best matches the genotype likelihoods.
   *
   * @param seq_pileup The sequence pileup represented as a string.
   * @param qual_pileup The corresponding quality value pileup represented as a
   * string.
   * @return The index of the selected quantizer.
   */
  int ComputeQuantizerIndex(const std::string& seq_pileup,
                            const std::string& qual_pileup);

  /**
   * @brief Retrieves the computed genotype likelihoods for a given pileup.
   *
   * This method returns a map of genotype strings to their associated
   * likelihood values based on the input sequence and quality value pileup.
   *
   * @param seq_pileup The sequence pileup represented as a string.
   * @param qual_pileup The corresponding quality value pileup represented as a
   * string.
   * @return A map of genotype strings to their likelihood values.
   */
  const std::map<std::string, double>& GetGenotypelikelihoods(
      const std::string& seq_pileup, const std::string& qual_pileup);

 private:
  /**
   * @brief Initializes the likelihood maps for alleles and genotypes.
   *
   * This method sets up the internal data structures for storing allele and
   * genotype likelihoods based on the allele alphabet and ploidy
   * configuration.
   */
  void InitLikelihoods();

  /**
   * @brief Resets the likelihood values to their initial states.
   *
   * This method clears the current likelihood values and resets them
   * to a neutral state for the next calculation.
   */
  void ResetLikelihoods();

  /**
   * @brief Computes the genotype likelihoods for the given sequence and
   * quality value pileup.
   *
   * This method calculates the likelihood of each possible genotype based on
   * the input sequence and quality value pileup. It considers the depth
   * (length) of the pileup.
   *
   * @param seq_pileup The sequence pileup represented as a string.
   * @param qual_pileup The corresponding quality value pileup represented as a
   * string.
   * @param depth The depth of the pileup, representing the number of
   * observations.
   */
  void ComputeGenotypeLikelihoods(const std::string& seq_pileup,
                                  const std::string& qual_pileup,
                                  const size_t& depth);

  const std::vector<char> allele_alphabet_ = {
      'A', 'C', 'G', 'T'};  //!< @brief The alphabet of valid alleles.
  static constexpr size_t allele_alphabet_size_ =
      4;  //!< @brief The Size of the allele alphabet.

  const std::vector<char>
      current_allele_alphabet_;  //!< @brief The active allele alphabet.
  std::map<char, double> allele_likelihoods_;  //!< @brief Map of alleles to
                                               //!< their likelihood values.
  std::vector<std::string>
      genotype_alphabet_;  //!< @brief List of possible genotypes.
  std::map<std::string, double>
      genotype_likelihoods_;  //!< @brief Map of genotypes to their likelihood
                              //!< values.
  const int nr_quantizers_;   //!< @brief Number of quantizers available.
  const int
      polyploidy_;  //!< @brief Ploidy of the organism (number of alleles).
  const int
      qual_offset_;   //!< @brief Quality value offset (e.g., 33 for Phred+33).
  const bool debug_;  //!< @brief Flag for enabling debug output.
};

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_GENOTYPER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
