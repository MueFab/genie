/** @file SoftclipSpreader.h
 *  @brief This file contains the definition of the spreader of high quality soft bases.
 */

// Copyright 2015-2018 Leibniz Universitaet Hannover

#ifndef CALQ_QUALCODEC_SOFTCLIPSPREADER_H_
#define CALQ_QUALCODEC_SOFTCLIPSPREADER_H_

// ----------------------------------------------------------------------------------------------------------------------

#include <stddef.h>
#include <vector>
#include <utility>

#include "CircularBuffer.h"

// ----------------------------------------------------------------------------------------------------------------------

namespace calq {

// Detects areas with high-quality-softclips and spreads their activity score
class SoftclipSpreader {
 private:
    std::vector<std::pair<size_t, double>> forwardSpread;  // Memory in order to spread to future positions
    CircularBuffer<double> buffer;  // Buffer in order to spread to past positions
    CircularBuffer<double> original;  // Buffer in order to save raw scores
    const size_t MAX_PROPAGATION;  // How far activity scores are supposed to be spread at maximum
    const size_t MIN_HQ_SOFTCLIPS;  // How many softclips have to be there to trigger the spreading
    bool squashed;

    // Squashes activity score between 0 and 1 using the unsquashed activity score and an "anti score" measuring
    // probability of no variant occuring
    double squash(double activity, double antiActivity) const;
 public:
    double push(double score, size_t softclips);  // Push an activity score and average number of softclips at that position into buffer.
                                                  // Returns the oldest activity score which can't be influenced by new clips anymore.
    size_t getOffset() const;                     // Delay between push of score and processed output

    SoftclipSpreader(size_t max_prop, size_t min_hq_clips, bool isSquashed);
};
}  // namespace calq

// ----------------------------------------------------------------------------------------------------------------------

#endif  // CALQ_QUALCODEC_SOFTCLIPSPREADER_H_

// ----------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------------
