/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_PROBABILITY_DISTRIBUTION_H_
#define SRC_GENIE_QUALITY_CALQ_PROBABILITY_DISTRIBUTION_H_

// -----------------------------------------------------------------------------

#include <cstddef>

// -----------------------------------------------------------------------------

#include <vector>

// -----------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace calq {

// Samples a probability distribution
class ProbabilityDistribution {
 private:
    std::vector<size_t> pdf;

    // Offset
    size_t rangeMin;

 public:
    ProbabilityDistribution(size_t rangeMin, size_t rangeMax);

    // Increases counter of value
    void addToPdf(size_t qualScore, size_t number = 1);

    // Reset all counters to zero
    void resetPdf();

    // Returns size of interval of valid values
    size_t size() const;

    // Returns current count of value
    size_t getCount(size_t value) const;

    // Returns current count of index (starting at zero)
    size_t operator[](size_t index) const;

    // Lowest possible value
    size_t getRangeMin() const;

    // Greatest possible value
    size_t getRangeMax() const;
};

// -----------------------------------------------------------------------------

}  // namespace calq
}  // namespace quality
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_PROBABILITY_DISTRIBUTION_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
