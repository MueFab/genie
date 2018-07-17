/** @file SoftclipSpreader.cc
 *  @brief This file contains the implementation of the softclip spreader.
 */

// Copyright 2015-2018 Leibniz Universitaet Hannover

#include "QualCodec/SoftclipSpreader.h"

// ----------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <algorithm>

// ----------------------------------------------------------------------------------------------------------------------

namespace calq {

double SoftclipSpreader::squash(double activity, double antiActivity) const {
    return activity / (activity + antiActivity);
}

// ----------------------------------------------------------------------------------------------------------------------

double SoftclipSpreader::push(double score, size_t softclips) {
    // Trigger spreading
    if (softclips >= MIN_HQ_SOFTCLIPS) {
        // Radius
        int clipped = std::min(softclips, MAX_PROPAGATION);

        // Remember for future positions
        forwardSpread.push_back(std::pair<size_t, double>(clipped+1, score));

        // Change past positions
        for (int i = buffer.size()-clipped; i < static_cast<int>(buffer.size()); ++i) {
            buffer[i] += score;
        }
    }

    double ownscore = score;

    // Apply all changes to current position in memory from other softclips
    for (auto it = forwardSpread.begin(); it != forwardSpread.end();) {
        ownscore += (*it).second;
        --(*it).first;
        if ((*it).first == 0)
            it = forwardSpread.erase(it);
        else
            ++it;
    }

    double orig =  original.push(score);

    return squashed ? squash(buffer.push(ownscore), 1.0 - orig) : buffer.push(ownscore);
}

// ----------------------------------------------------------------------------------------------------------------------

size_t SoftclipSpreader::getOffset() const {
    return MAX_PROPAGATION;
}

// ----------------------------------------------------------------------------------------------------------------------

SoftclipSpreader::SoftclipSpreader(size_t max_prop, size_t min_hq_clips, bool isSquashed) : buffer(max_prop, 0.0), original(max_prop, 0.0),
    MAX_PROPAGATION(max_prop), MIN_HQ_SOFTCLIPS(min_hq_clips), squashed(isSquashed) {
}
}  // namespace calq

// ----------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------------
