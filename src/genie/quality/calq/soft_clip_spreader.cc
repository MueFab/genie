/**
 * Copyright 2018-2024 The Genie Authors.
 * @file soft_clip_spreader.cc
 *
 * @brief Implementation of the SoftClipSpreader class for quality score
 * propagation.
 *
 * This source file implements the `SoftClipSpreader` class, part of the GENIE
 * quality value coding module. The class is designed to propagate quality
 * scores influenced by soft-clipped bases, ensuring accurate representation
 * of local quality in genomic data. It applies score adjustments based on
 * configurable thresholds and handles buffering for efficient computation.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/soft_clip_spreader.h"

#include <algorithm>
#include <iostream>

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

double SoftClipSpreader::squash(const double activity,
                                const double anti_activity) {
  return activity / (activity + anti_activity);
}

// -----------------------------------------------------------------------------

double SoftClipSpreader::push(double score, const size_t soft_clips) {
  // Trigger spreading
  if (soft_clips >= min_hq_soft_clips_) {
    // Radius
    const auto clipped =
        static_cast<int>(std::min(soft_clips, max_propagation_));

    // Remember for future positions
    forward_spread_.emplace_back(clipped + 1, score);

    // Change past positions
    for (auto i = static_cast<int>(buffer_.size() - clipped);
         i < static_cast<int>(buffer_.size()); ++i) {
      buffer_[i] += score;
    }
  }

  double own_score = score;

  // Apply all changes to current position in memory from other soft clips
  for (auto it = forward_spread_.begin(); it != forward_spread_.end();) {
    own_score += it->second;
    --it->first;
    if (it->first == 0) {
      it = forward_spread_.erase(it);
    } else {
      ++it;
    }
  }

  const double orig = std::min(original_.push(score), 1.0);

  return squashed_ ? squash(buffer_.push(own_score), 1.0 - orig)
                   : buffer_.push(own_score);
}

// -----------------------------------------------------------------------------

size_t SoftClipSpreader::GetOffset() const { return max_propagation_; }

// -----------------------------------------------------------------------------

SoftClipSpreader::SoftClipSpreader(const size_t max_prop,
                                   const size_t min_hq_clips,
                                   const bool is_squashed)
    : buffer_(max_prop, 0.0),
      original_(max_prop, 0.0),
      max_propagation_(max_prop),
      min_hq_soft_clips_(min_hq_clips),
      squashed_(is_squashed) {}

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
