/**
 * @file
 * @brief Header file for the ZSTD entropy encoder module.
 * @details This file declares the `Encoder` class, which provides functionality for compressing raw access units
 *          into block payloads using the ZSTD compression algorithm. The class is part of the GENIE MPEG-G toolkit
 *          and implements the required functionality to process MPEG-G descriptors and generate compressed data
 *          blocks for efficient storage.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_ZSTD_ENCODER_H_
#define SRC_GENIE_ENTROPY_ZSTD_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/access-unit.h"
#include "genie/core/entropy-encoder.h"
#include "genie/core/parameter/descriptor_present/decoder-regular.h"
#include "genie/util/stop-watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::zstd {

/**
 * @brief Encoder class for the ZSTD compression algorithm.
 * @details This class handles the compression of raw access units into block payloads using the ZSTD algorithm.
 *          It converts the raw MPEG-G descriptors into compressed representations, following the MPEG-G
 *          specifications and utilizing the GENIE core infrastructure.
 */
class Encoder : public core::EntropyEncoder {
 public:
    bool writeOutStreams{};  //!< @brief Flag to enable writing output streams for debugging purposes.

    /**
     * @brief Compress a given descriptor using the ZSTD algorithm.
     * @param desc Reference to the descriptor to be compressed.
     * @return The compressed entropy-coded data, encapsulated in the `EntropyCoded` structure.
     */
    EntropyCoded process(core::AccessUnit::Descriptor &desc) override;

    /**
     * @brief Construct a new Encoder object.
     * @param _writeOutStreams Flag to enable or disable writing out streams for debugging.
     */
    explicit Encoder(bool _writeOutStreams);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::zstd

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_ZSTD_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
