/**
 * @file
 * @brief Defines a BSC-specific encoder for entropy compression using the GABAC algorithm.
 * @details This file contains the implementation of the `Encoder` class, which performs compression of raw access units
 *          into entropy-coded block payloads using the GABAC library. The class inherits from the base `EntropyEncoder`
 *          interface and implements the necessary functionality to process MPEG-G descriptors.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_BSC_ENCODER_H_
#define SRC_GENIE_ENTROPY_BSC_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/access-unit.h"
#include "genie/core/entropy-encoder.h"
#include "genie/core/parameter/descriptor_present/decoder-regular.h"
#include "genie/util/stop-watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::bsc {

/**
 * @brief Module to compress raw access units into block payloads using GABAC.
 * @details The `Encoder` class is responsible for handling entropy compression of raw MPEG-G access units.
 *          It implements the core entropy encoding interface and integrates GABAC-specific algorithms to
 *          achieve high compression efficiency.
 */
class Encoder : public core::EntropyEncoder {
 public:
    bool writeOutStreams{};  //!< @brief Flag to enable or disable writing out streams for debugging.

    /**
     * @brief Compress the given descriptor using GABAC.
     * @param desc Reference to the descriptor to be compressed.
     * @return The compressed entropy-coded data.
     * @details The function compresses an MPEG-G descriptor using the GABAC library, returning the entropy-coded
     *          representation. The resulting data is stored in a structure that encapsulates all necessary
     *          metadata and bitstream information.
     */
    EntropyCoded process(core::AccessUnit::Descriptor &desc) override;

    /**
     * @brief Construct a new Encoder object.
     * @param _writeOutStreams Flag to enable or disable writing out streams for debugging purposes.
     * @details This constructor initializes the encoder with the specified stream writing configuration.
     */
    explicit Encoder(bool _writeOutStreams);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::bsc

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_BSC_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
