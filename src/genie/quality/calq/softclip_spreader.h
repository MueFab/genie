/**
 * @file
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for the SoftclipSpreader class.
 *
 * This file defines the `SoftclipSpreader` class, which is used in the CALQ
 * quality value compression framework to identify and spread the impact of high-quality
 * softclips in genomic data. Softclips are regions in read alignments where the read
 * is only partially aligned to the reference, often indicating variant-rich areas.
 * The spreader class helps in enhancing the activity scores of these regions, taking into
 * account their context.
 *
 * @details The `SoftclipSpreader` class detects softclipped regions that have a high average
 * quality score and spreads the activity score of these regions over nearby positions. This is
 * useful in scenarios where a single high-quality softclip indicates a variant event, influencing
 * adjacent regions.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_SOFTCLIP_SPREADER_H_
#define SRC_GENIE_QUALITY_CALQ_SOFTCLIP_SPREADER_H_

#include <cstddef>
#include <utility>
#include <vector>
#include "genie/quality/calq/circular_buffer.h"

namespace genie::quality::calq {

/**
 * @brief The `SoftclipSpreader` class enhances activity scores in regions around high-quality softclips.
 *
 * The `SoftclipSpreader` class uses a combination of circular buffers to track softclip occurrences
 * and their quality scores over a sliding window. When the number of softclips at a particular
 * position exceeds a defined threshold, the activity score is spread over adjacent positions,
 * simulating the propagation of the variant signal.
 */
class SoftclipSpreader {
 private:
    std::vector<std::pair<size_t, double>> forwardSpread;  //!< @brief Forward spread buffer to propagate scores ahead.
    CircularBuffer<double> buffer;    //!< @brief Buffer to manage activity scores for past positions.
    CircularBuffer<double> original;  //!< @brief Circular buffer to keep track of raw scores.

    const size_t MAX_PROPAGATION;   //!< @brief Maximum distance over which scores are spread.
    const size_t MIN_HQ_SOFTCLIPS;  //!< @brief Minimum number of high-quality softclips to trigger spread.
    bool squashed;                  //!< @brief Whether to squash scores between 0 and 1.

    /**
     * @brief Squashes the activity score between 0 and 1.
     *
     * This function uses the unsquashed activity score and an "anti score" that measures the probability
     * of no variant occurring to create a final score in the range [0,1].
     *
     * @param activity Unsquashed activity score.
     * @param antiActivity Score indicating lack of a variant.
     * @return Squashed score.
     */
    [[nodiscard]] static double squash(double activity, double antiActivity);

 public:
    /**
     * @brief Adds a new activity score and softclip count to the spreader.
     *
     * This function takes in the activity score for a position and the number of high-quality softclips
     * observed at that position. If the number of softclips exceeds the configured threshold, the score
     * is spread to nearby positions, simulating a local variant effect.
     *
     * @param score The activity score to be processed.
     * @param softclips The number of high-quality softclips observed at this position.
     * @return The oldest score in the buffer that can no longer be influenced by new softclips.
     */
    double push(double score, size_t softclips);

    /**
     * @brief Returns the delay between pushing a score and obtaining the processed output.
     *
     * This value is equal to the size of the internal buffer minus 1. It indicates the number
     * of positions the activity score can influence before it is fully propagated.
     *
     * @return The delay (offset) in positions.
     */
    [[nodiscard]] size_t getOffset() const;

    /**
     * @brief Constructs a new `SoftclipSpreader` with the given configuration.
     *
     * @param max_prop The maximum number of positions over which a score should be spread.
     * @param min_hq_clips Minimum number of high-quality softclips required to trigger a spread.
     * @param isSquashed Whether to squash the scores between 0 and 1.
     */
    SoftclipSpreader(size_t max_prop, size_t min_hq_clips, bool isSquashed);
};

}  // namespace genie::quality::calq

#endif  // SRC_GENIE_QUALITY_CALQ_SOFTCLIP_SPREADER_H_
