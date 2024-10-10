/**
 * @file encoder-none.h
 *
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the NoneEncoder class for handling quality value encoding with no compression.
 *
 * This file contains the declaration of the `NoneEncoder` class, which is a specialized implementation
 * of the `QVEncoder` interface within the GENIE framework. The `NoneEncoder` class is responsible for
 * processing quality values in genomic records without performing any compression. It simply passes
 * through the quality values as they are.
 *
 * @details The `NoneEncoder` class is designed for scenarios where no quality value compression is desired.
 * It provides a straightforward implementation that outputs quality values in their original form, without
 * applying any modifications or compression techniques.
 */

#ifndef SRC_GENIE_QUALITY_QVWRITEOUT_ENCODER_NONE_H_
#define SRC_GENIE_QUALITY_QVWRITEOUT_ENCODER_NONE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/qv-encoder.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::quality::qvwriteout {

/**
 * @brief Implements a quality value encoder that performs no compression.
 *
 * The `NoneEncoder` class is responsible for encoding quality values in genomic records
 * without applying any compression or transformation. It is primarily used in scenarios
 * where quality values should be preserved as-is, without modifications.
 */
class NoneEncoder : public core::QVEncoder {
 public:
    /**
     * @brief Processes a chunk of genomic records and outputs quality values as-is.
     *
     * This method takes a chunk of genomic records, extracts the quality values, and
     * returns them in their original form without applying any compression or encoding.
     *
     * @param chunk The chunk of genomic records to be processed.
     * @return A `QVCoded` object containing the unmodified quality values from the input records.
     */
    QVCoded process(const core::record::Chunk& chunk) override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::quality::qvwriteout

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_QVWRITEOUT_ENCODER_NONE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
