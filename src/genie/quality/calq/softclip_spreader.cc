/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/softclip_spreader.h"

// -----------------------------------------------------------------------------

#include <algorithm>
#include <iostream>

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

double SoftclipSpreader::squash(const double activity, const double antiActivity) {
    return activity / (activity + antiActivity);
}

// -----------------------------------------------------------------------------

double SoftclipSpreader::push(double score, const size_t softclips) {
    // Trigger spreading
    if (softclips >= MIN_HQ_SOFTCLIPS) {
        // Radius
        const auto clipped = static_cast<int>(std::min(softclips, MAX_PROPAGATION));

        // Remember for future positions
        forwardSpread.emplace_back(clipped + 1, score);

        // Change past positions
        for (auto i = static_cast<int>(buffer.size() - clipped); i < static_cast<int>(buffer.size()); ++i) {
            buffer[i] += score;
        }
    }

    double ownscore = score;

    // Apply all changes to current position in memory from other softclips
    for (auto it = forwardSpread.begin(); it != forwardSpread.end();) {
        ownscore += (*it).second;
        --(*it).first;
        if ((*it).first == 0) {
            it = forwardSpread.erase(it);
        } else {
            ++it;
        }
    }

    const double orig = std::min(original.push(score), 1.0);

    return squashed ? squash(buffer.push(ownscore), 1.0 - orig) : buffer.push(ownscore);
}

// -----------------------------------------------------------------------------

size_t SoftclipSpreader::getOffset() const { return MAX_PROPAGATION; }

// -----------------------------------------------------------------------------

SoftclipSpreader::SoftclipSpreader(const size_t max_prop, const size_t min_hq_clips, const bool isSquashed)
    : buffer(max_prop, 0.0),
      original(max_prop, 0.0),
      MAX_PROPAGATION(max_prop),
      MIN_HQ_SOFTCLIPS(min_hq_clips),
      squashed(isSquashed) {}

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
