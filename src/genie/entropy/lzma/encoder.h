/**
 * @file
 * @brief Header file for the LZMA-based entropy encoder class.
 * @details This file contains the definition of the `Encoder` class for compressing raw access units
 *          using the LZMA algorithm. The `Encoder` class implements the `EntropyEncoder` interface
 *          and provides functionality for converting MPEG-G descriptors into their compressed representations.
 * @copyright This file is part of GENIE.
 *            See LICENSE and/or https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_ENTROPY_LZMA_ENCODER_H_
#define SRC_GENIE_ENTROPY_LZMA_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/access-unit.h"
#include "genie/core/entropy-encoder.h"
#include "genie/core/parameter/descriptor_present/decoder-regular.h"
#include "genie/util/stop-watch.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::entropy::lzma {

/**
 * @brief Class for compressing raw access units using the LZMA algorithm.
 * @details This encoder class inherits from the `EntropyEncoder` interface and implements the
 *          `process` method to encode and compress access units into LZMA-compressed blocks. It
 *          also provides a configurable option to enable writing out intermediate streams.
 */
class Encoder : public core::EntropyEncoder {
 public:
    bool writeOutStreams{};  //!< @brief Flag to enable or disable writing out intermediate streams

    /**
     * @brief Compress a given descriptor using the LZMA algorithm.
     * @param desc Reference to the descriptor to be compressed.
     * @return The compressed entropy-coded data.
     * @details This method takes a raw MPEG-G descriptor and performs LZMA compression,
     *          producing a compressed version of the descriptor in a standard format.
     */
    EntropyCoded process(core::AccessUnit::Descriptor &desc) override;

    /**
     * @brief Construct a new Encoder object.
     * @param _writeOutStreams Flag to enable or disable writing out intermediate streams.
     * @details If `writeOutStreams` is set to `true`, additional intermediate data will be
     *          stored for debugging or analysis purposes.
     */
    explicit Encoder(bool _writeOutStreams);
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::entropy::lzma

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_LZMA_ENCODER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
