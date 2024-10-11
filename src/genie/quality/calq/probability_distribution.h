/**
 * @file probability_distribution.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the ProbabilityDistribution class for handling and manipulating probability distributions.
 *
 * This file contains the declaration of the `ProbabilityDistribution` class, which provides methods to
 * construct, modify, and query discrete probability distributions. The class is used to track the frequency
 * of quality values and provides functionality for querying value counts and resetting the distribution.
 *
 * @details The `ProbabilityDistribution` class operates on a range of values specified during instantiation.
 * It maintains a vector of counts for each value within this range and supports incrementing and resetting
 * these counts. The class is primarily used for analyzing the distribution of quality scores within a
 * specific interval and for constructing probability models.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_PROBABILITY_DISTRIBUTION_H_
#define SRC_GENIE_QUALITY_CALQ_PROBABILITY_DISTRIBUTION_H_

#include <cstddef>
#include <vector>

namespace genie::quality::calq {

/**
 * @brief A class for tracking and manipulating a discrete probability distribution.
 *
 * The `ProbabilityDistribution` class provides methods to construct a probability distribution
 * over a range of integer values, increment the frequency counts for specific values, and query
 * these counts. It is used for analyzing and modeling distributions, such as quality value
 * distributions in genomic data.
 */
class ProbabilityDistribution {
 private:
    std::vector<size_t> pdf;  //!< @brief The probability distribution function (PDF) represented as a vector of counts.
    size_t rangeMin;          //!< @brief The minimum value in the range of the distribution.

 public:
    /**
     * @brief Constructs a `ProbabilityDistribution` over a specified range of values.
     *
     * The constructor initializes a probability distribution with counts set to zero
     * for all values in the range `[rangeMin, rangeMax]`.
     *
     * @param rangeMin The minimum value in the range.
     * @param rangeMax The maximum value in the range.
     */
    ProbabilityDistribution(size_t rangeMin, size_t rangeMax);

    /**
     * @brief Increments the count for a specified value in the distribution.
     *
     * This method increases the count for a specified `qualScore` by `number`.
     * If the specified value is outside the range, the function will not modify the distribution.
     *
     * @param qualScore The quality score or value to increment.
     * @param number The amount to increment the count by. Defaults to 1.
     */
    void addToPdf(size_t qualScore, size_t number = 1);

    /**
     * @brief Resets the distribution by setting all counts to zero.
     *
     * This method clears the PDF, setting all values in the distribution to zero.
     */
    void resetPdf();

    /**
     * @brief Gets the size of the value range for the distribution.
     *
     * This function returns the number of distinct values in the range `[rangeMin, rangeMax]`.
     *
     * @return The number of values in the range of the distribution.
     */
    [[nodiscard]] size_t size() const;

    /**
     * @brief Retrieves the count for a specific value in the distribution.
     *
     * This method returns the frequency count of a specified value. If the value
     * is outside the valid range, the result is undefined.
     *
     * @param value The value for which to retrieve the count.
     * @return The count of the specified value.
     */
    [[nodiscard]] size_t getCount(size_t value) const;

    /**
     * @brief Retrieves the count at a specific index within the distribution.
     *
     * The index is relative to the internal vector representation of the PDF.
     * Index `0` corresponds to `rangeMin`, and the highest index corresponds to `rangeMax`.
     *
     * @param index The index of the count to retrieve.
     * @return The count at the specified index.
     */
    size_t operator[](size_t index) const;

    /**
     * @brief Gets the minimum value in the range of the distribution.
     *
     * @return The minimum value of the distribution range.
     */
    [[nodiscard]] size_t getRangeMin() const;

    /**
     * @brief Gets the maximum value in the range of the distribution.
     *
     * @return The maximum value of the distribution range.
     */
    [[nodiscard]] size_t getRangeMax() const;
};

}  // namespace genie::quality::calq

#endif  // SRC_GENIE_QUALITY_CALQ_PROBABILITY_DISTRIBUTION_H_

