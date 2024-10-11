/**
 * @file lloyd_max_quantizer.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the `LloydMaxQuantizer` class for quality value quantization using the Lloyd-Max algorithm.
 *
 * This file provides the declaration of the `LloydMaxQuantizer` class, which implements a quantizer
 * based on the Lloyd-Max algorithm for optimal quantization of continuous probability distributions.
 * The class provides functionality to compute decision boundaries and representative values for each
 * quantization interval, minimizing distortion and preserving information content.
 *
 * @details The `LloydMaxQuantizer` class is derived from the base `Quantizer` class and implements
 * a quantization method that adjusts interval boundaries to achieve a locally optimal quantization.
 * It uses a probability distribution function (PDF) to calculate centroids and decision thresholds
 * for each quantization interval, providing a tailored quantization solution for the given distribution.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_LLOYD_MAX_QUANTIZER_H_
#define SRC_GENIE_QUALITY_CALQ_LLOYD_MAX_QUANTIZER_H_

#include <cmath>
#include <vector>
#include "genie/quality/calq/exceptions.h"
#include "genie/quality/calq/probability_distribution.h"
#include "genie/quality/calq/quantizer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::quality::calq {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Implements the Lloyd-Max quantizer for quality value compression.
 *
 * The `LloydMaxQuantizer` class implements the Lloyd-Max quantization algorithm, which is a
 * special case of the k-means clustering algorithm used for optimal scalar quantization.
 * It computes decision thresholds and centroids for a given probability distribution function (PDF)
 * to minimize quantization distortion. The resulting quantizer provides better preservation of
 * probability characteristics compared to uniform quantizers.
 */
class LloydMaxQuantizer : public Quantizer {
 private:
    std::vector<double> borders;  //!< @brief Decision thresholds for each quantization interval.
    std::vector<double> values;   //!< @brief Representative values (centroids) for each interval.
    size_t steps;                 //!< @brief Number of quantization steps.

    /**
     * @brief Fills the lookup table (LUT) based on the provided probability distribution.
     *
     * This method calculates the optimal representative values and decision thresholds
     * using the Lloyd-Max algorithm and fills the base class lookup table (`lut`) accordingly.
     *
     * @param pdf Probability distribution function (PDF) for the data to be quantized.
     */
    void fillLUT(const ProbabilityDistribution& pdf);

    /**
     * @brief Computes the centroid for a given interval in the PDF.
     *
     * This method calculates the centroid (i.e., the optimal representative value) for
     * a specific interval in the probability distribution. It uses the weighted average
     * of all values within the interval to determine the centroid.
     *
     * @param left The index of the left boundary of the interval.
     * @param right The index of the right boundary of the interval.
     * @param pdf The probability distribution function (PDF) for the data.
     * @return The computed centroid for the interval.
     */
    static double calcCentroid(size_t left, size_t right, const ProbabilityDistribution& pdf);

    /**
     * @brief Computes the decision thresholds for each quantization interval.
     *
     * This method calculates the decision thresholds (or borders) that define each
     * quantization interval. It iteratively updates the thresholds based on the
     * current centroids until convergence is achieved.
     *
     * @param pdf The probability distribution function (PDF) for the data.
     */
    void calcBorders(const ProbabilityDistribution& pdf);

 public:
    /**
     * @brief Constructs a `LloydMaxQuantizer` object with a specified number of quantization steps.
     *
     * @param steps The number of quantization steps to be used.
     */
    explicit LloydMaxQuantizer(size_t steps);

    /**
     * @brief Builds the quantizer using the provided probability distribution.
     *
     * This method sets up the quantization intervals and representative values based
     * on the provided probability distribution. It uses the Lloyd-Max algorithm to
     * calculate the optimal quantization boundaries and fills the lookup table.
     *
     * @param pdf The probability distribution function (PDF) for the data to be quantized.
     */
    void build(const ProbabilityDistribution& pdf);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::quality::calq

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_LLOYD_MAX_QUANTIZER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
