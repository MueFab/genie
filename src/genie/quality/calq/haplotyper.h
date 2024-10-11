/**
 * @file haplotyper.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the `Haplotyper` class for calculating local haplotype likelihoods.
 *
 * This file provides the declaration of the `Haplotyper` class, which is used to calculate
 * local haplotype likelihoods based on genotype likelihoods and quality value pileups.
 * It supports various configurations, such as filter type and quality value propagation,
 * to adapt to different genomic data scenarios.
 *
 * @details The `Haplotyper` class internally uses the `Genotyper` class to calculate
 * genotype likelihoods and manages a filter buffer to smooth local distortions.
 * Additionally, it tracks high-quality soft clips and spreads their effects
 * over neighboring positions.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_HAPLOTYPER_H_
#define SRC_GENIE_QUALITY_CALQ_HAPLOTYPER_H_

#include <cmath>
#include <functional>
#include <map>
#include <string>
#include <vector>
#include "genie/quality/calq/filter_buffer.h"
#include "genie/quality/calq/genotyper.h"
#include "genie/quality/calq/softclip_spreader.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::quality::calq {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Enumeration of the filter types for activity score calculation.
 */
enum struct FilterType;

/**
 * @brief Calculates local haplotype likelihoods using genotype and filter buffers.
 *
 * The `Haplotyper` class provides functionalities for calculating local haplotype
 * likelihoods based on genotype likelihoods and local filter buffers. It manages
 * activity score calculation, high-quality soft-clip tracking, and local smoothing
 * using various filter types.
 */
class Haplotyper {
 private:
    FilterBuffer buffer;       //!< @brief Filter buffer for smoothing activity scores.
    SoftclipSpreader spreader;  //!< @brief Manages the propagation of high-quality soft clips.
    Genotyper genotyper;        //!< @brief Computes genotype likelihoods for given pileups.

    size_t nr_quantizers;       //!< @brief Number of quantizers available for quality value encoding.
    size_t polyploidy;          //!< @brief Ploidy level of the organism (e.g., 2 for diploid).
    const bool DEBUG;           //!< @brief Flag for enabling debug output.

    const bool squashedActivity;  //!< @brief Indicates if activity values are squashed between 0.0 and 1.0.
    double localDistortion;       //!< @brief Measures the local distortion based on genotype likelihoods.

    /**
     * @brief Computes the index of the quantizer based on the activity score.
     *
     * This function selects the quantizer that best matches the activity score
     * to minimize distortion during quality value encoding.
     *
     * @param activity The activity score to evaluate.
     * @return The index of the selected quantizer.
     */
    [[nodiscard]] size_t getQuantizerIndex(double activity) const;

 public:
    /**
     * @brief Constructs a `Haplotyper` object with specified parameters.
     *
     * @param sigma Size of the filter used for smoothing the activity scores.
     * @param ploidy Ploidy level of the organism (e.g., 2 for diploid).
     * @param qualOffset Offset value for the quality score format (e.g., 33 for Phred+33).
     * @param nrQuantizers Number of quantizers available for quality value encoding.
     * @param maxHQSoftclip_propagation Maximum propagation distance for high-quality soft clips.
     * @param minHQSoftclip_streak Minimum number of high-quality soft clips required for propagation.
     * @param filterCutOff Cutoff size for the filter kernel.
     * @param debug Flag to enable or disable debug output.
     * @param squashed Indicates if activity values should be squashed between 0.0 and 1.0.
     * @param filterType Type of filter to use for activity score calculation.
     */
    Haplotyper(size_t sigma, size_t ploidy, size_t qualOffset, size_t nrQuantizers, size_t maxHQSoftclip_propagation,
               size_t minHQSoftclip_streak, size_t filterCutOff, bool debug, bool squashed, FilterType filterType);

    /**
     * @brief Retrieves the offset distance between the activity score position and the front of the buffer.
     *
     * @return The offset distance in bases.
     */
    [[nodiscard]] size_t getOffset() const;

    /**
     * @brief Pushes a new activity score into the buffer and returns the filtered score.
     *
     * This method updates the internal filter buffer with a new activity score based on
     * the sequence and quality pileup. It also tracks high-quality soft clips and propagates
     * their effects over neighboring positions.
     *
     * @param seqPile The sequence pileup represented as a string.
     * @param qualPile The corresponding quality value pileup represented as a string.
     * @param hq_softclips Number of high-quality soft clips in the current pileup.
     * @param reference The reference base character for the current position.
     * @return The index of the selected quantizer for the current position.
     */
    size_t push(const std::string& seqPile, const std::string& qualPile, size_t hq_softclips, char reference);

    /**
     * @brief Calculates the likelihood of non-reference alleles based on the given pileup.
     *
     * This method computes the likelihood values for all alleles that are different from
     * the reference allele. It uses the sequence and quality value pileup to assess the
     * certainty of each alternative allele.
     *
     * @param ref The reference base character.
     * @param seqPile The sequence pileup represented as a string.
     * @param qualPile The corresponding quality value pileup represented as a string.
     * @return A vector of likelihood values for non-reference alleles.
     */
    std::vector<double> calcNonRefLikelihoods(char ref, const std::string& seqPile, const std::string& qualPile);

    /**
     * @brief Computes the activity score for a given pileup based on heterozygosity.
     *
     * This method calculates the activity score, which is a measure of the local
     * distortion and variability in the sequence data. It takes into account the
     * reference base, sequence pileup, and heterozygosity value.
     *
     * @param ref The reference base character.
     * @param seqPile The sequence pileup represented as a string.
     * @param qualPile The corresponding quality value pileup represented as a string.
     * @param heterozygosity The expected heterozygosity value for the region.
     * @return The computed activity score.
     */
    double calcActivityScore(char ref, const std::string& seqPile, const std::string& qualPile, double heterozygosity);

    /**
     * @brief Computes the prior probabilities for different genotypes based on heterozygosity.
     *
     * @param hetero The expected heterozygosity value.
     * @return A vector of prior probabilities for the genotypes.
     */
    [[nodiscard]] std::vector<double> calcPriors(double hetero) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::quality::calq

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_HAPLOTYPER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
