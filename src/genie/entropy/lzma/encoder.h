/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Header file for the LZMA-based entropy encoder class.
 * @details This file contains the definition of the `Encoder` class for
 * compressing raw access units using the LZMA algorithm. The `Encoder` class
 * implements the `EntropyEncoder` interface and provides functionality for
 * converting MPEG-G descriptors into their compressed representations.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_ENTROPY_LZMA_ENCODER_H_
#define SRC_GENIE_ENTROPY_LZMA_ENCODER_H_

// -----------------------------------------------------------------------------

#include "genie/core/access_unit.h"
#include "genie/core/entropy_encoder.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::lzma {

/**
 * @brief Class for compressing raw access units using the LZMA algorithm.
 * @details This encoder class inherits from the `EntropyEncoder` interface and
 * implements the `process` method to encode and Compress access units into
 * LZMA-compressed blocks. It also provides a configurable option to enable
 * writing out intermediate streams.
 */
class Encoder final : public core::EntropyEncoder {
 public:
  bool write_out_streams_{};  //!< @brief Flag to enable or disable writing out
                              //!< intermediate streams

  /**
   * @brief Compress a given descriptor using the LZMA algorithm.
   * @param desc Reference to the descriptor to be compressed.
   * @return The compressed entropy-coded data.
   * @details This method takes a raw MPEG-G descriptor and performs LZMA
   * compression, producing a compressed version of the descriptor in a standard
   * format.
   */
  entropy_coded Process(core::AccessUnit::Descriptor& desc) override;

  /**
   * @brief Construct a new Encoder object.
   * @param write_out_streams Flag to enable or disable writing out intermediate
   * streams.
   * @details If `write_out_streams_` is set to `true`, additional intermediate
   * data will be stored for debugging or analysis purposes.
   */
  explicit Encoder(bool write_out_streams);
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::lzma

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_LZMA_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
