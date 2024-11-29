/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for the SoftClipSpreader class.
 *
 * This file defines the `SoftClipSpreader` class, which is used in the CALQ
 * quality value compression framework to identify and spread the impact of
 * high-quality soft clips in genomic data. Soft clips are regions in read
 * alignments where the read is only partially aligned to the reference, often
 * indicating variant-rich areas. The spreader class helps in enhancing the
 * activity scores of these regions, taking into account their context.
 *
 * @details The `SoftClipSpreader` class detects soft clipped regions that have
 * a high average quality score and spreads the activity score of these regions
 * over nearby positions. This is useful in scenarios where a single
 * high-quality soft clip indicates a variant event, influencing adjacent
 * regions.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_SOFT_CLIP_SPREADER_H_
#define SRC_GENIE_QUALITY_CALQ_SOFT_CLIP_SPREADER_H_

// -----------------------------------------------------------------------------

#include <cstddef>
#include <utility>
#include <vector>

#include "genie/quality/calq/circular_buffer.h"

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

/**
 * @brief The `SoftClipSpreader` class enhances activity scores in regions
 * around high-quality soft clips.
 *
 * The `SoftClipSpreader` class uses a combination of circular buffers to track
 * soft clip occurrences and their quality scores over a sliding window. When
 * the number of soft clips at a particular position exceeds a defined
 * threshold, the activity score is spread over adjacent positions, simulating
 * the propagation of the variant signal.
 */
class SoftClipSpreader {
  std::vector<std::pair<size_t, double>>
      forward_spread_;  //!< @brief Forward spread buffer to propagate scores
                        //!< ahead.
  CircularBuffer<double>
      buffer_;  //!< @brief Buffer to manage activity scores for past positions.
  CircularBuffer<double>
      original_;  //!< @brief Circular buffer to keep track of raw scores.

  const size_t max_propagation_;  //!< @brief Maximum distance over which scores
                                  //!< are spread.
  const size_t min_hq_soft_clips_;  //!< @brief Minimum number of high-quality
                                    //!< soft clips to trigger spread.
  bool squashed_;  //!< @brief Whether to squash scores between 0 and 1.

  /**
   * @brief Squashes the activity score between 0 and 1.
   *
   * This function uses the unsquashed activity score and an "anti-score" that
   * measures the probability of no variant occurring to create a final score in
   * the range [0,1].
   *
   * @param activity Unsquashed activity score.
   * @param anti_activity Score indicating lack of a variant.
   * @return Squashed score.
   */
  [[nodiscard]] static double squash(double activity, double anti_activity);

 public:
  /**
   * @brief Adds a new activity score and soft clip count to the spreader.
   *
   * This function takes in the activity score for a position and the number of
   * high-quality soft clips observed at that position. If the number of
   * soft clips exceeds the configured threshold, the score is spread to nearby
   * positions, simulating a local variant effect.
   *
   * @param score The activity score to be processed.
   * @param soft_clips The number of high-quality soft clips observed at this
   * position.
   * @return The oldest score in the buffer that can no longer be influenced by
   * new soft clips.
   */
  double push(double score, size_t soft_clips);

  /**
   * @brief Returns the delay between pushing a score and obtaining the
   * processed output.
   *
   * This value is equal to the Size of the internal buffer minus 1. It
   * indicates the number of positions the activity score can influence before
   * it is fully propagated.
   *
   * @return The delay (offset) in positions.
   */
  [[nodiscard]] size_t GetOffset() const;

  /**
   * @brief Constructs a new `SoftClipSpreader` with the given configuration.
   *
   * @param max_prop The maximum number of positions over which a score should
   * be spread.
   * @param min_hq_clips Minimum number of high-quality soft clips required to
   * trigger a spread.
   * @param is_squashed Whether to squash the scores between 0 and 1.
   */
  SoftClipSpreader(size_t max_prop, size_t min_hq_clips, bool is_squashed);
};

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_SOFT_CLIP_SPREADER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
