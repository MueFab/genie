/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_LLOYD_MAX_QUANTIZER_H_
#define SRC_GENIE_QUALITY_CALQ_LLOYD_MAX_QUANTIZER_H_

// -----------------------------------------------------------------------------

#include <cmath>

// -----------------------------------------------------------------------------

#include <vector>

// -----------------------------------------------------------------------------

#include "exceptions.h"
#include "probability_distribution.h"
#include "quantizer.h"

// -----------------------------------------------------------------------------

namespace genie {
namespace quality {
namespace calq {

// -----------------------------------------------------------------------------

class LloydMaxQuantizer : public Quantizer {
 private:
    // Decision thresholds
    std::vector<double> borders;

    // Representative values for each interval
    std::vector<double> values;

    size_t steps;

    // Fill LUT of base class
    void fillLUT(const ProbabilityDistribution& pdf);

    // Calculates the centroid in a region of a pdf
    double calcCentroid(size_t left, size_t right, const ProbabilityDistribution& pdf);

    // Calculates quantization borders using pdf
    void calcBorders(const ProbabilityDistribution& pdf);

 public:
    explicit LloydMaxQuantizer(size_t steps);

    // Creates quantization boundaries from a probability
    // distribution and fills LUT
    void build(const ProbabilityDistribution& pdf);
};

// -----------------------------------------------------------------------------

}  // namespace calq
}  // namespace quality
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_LLOYD_MAX_QUANTIZER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
