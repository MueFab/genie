/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Header file for the LZMA-based entropy decoder class.
 * @details This file contains the definition of the `Decoder` class for
 * decompressing entropy-coded access units using the LZMA algorithm. The
 * `Decoder` class implements the `EntropyDecoder` interface and provides the
 * necessary functionality to restore MPEG-G descriptors from their compressed
 * representations.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_ENTROPY_LZMA_DECODER_H_
#define SRC_GENIE_ENTROPY_LZMA_DECODER_H_

// -----------------------------------------------------------------------------

#include <tuple>
#include <vector>

#include "genie/core/access_unit.h"
#include "genie/core/entropy_decoder.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::lzma {

/**
 * @brief Class to handle LZMA-based decompression of entropy-coded block
 * payloads.
 * @details This decoder class inherits from the `EntropyDecoder` interface and
 * is responsible for decoding LZMA-encoded blocks back into raw access units.
 * It provides an implementation of the `process` method that takes a descriptor
 * configuration and performs the actual decompression, returning the
 * reconstructed descriptor and performance statistics.
 */
class Decoder final : public core::EntropyDecoder {
 public:
  /**
   * @brief Decompress a given descriptor using the LZMA algorithm.
   * @param param Configuration of the descriptor subsequence.
   * @param d Descriptor to be processed and decompressed.
   * @param mm_coder_enabled Flag indicating if the mismatchCoder is enabled.
   * @return A tuple containing the decompressed descriptor and performance
   * statistics.
   * @details This method is responsible for reading the LZMA-compressed data,
   * decoding it, and restoring the original descriptor using the specified
   * configuration.
   */
  std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> Process(
      const core::parameter::DescriptorSubSequenceCfg& param,
      core::AccessUnit::Descriptor& d, bool mm_coder_enabled) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::lzma

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_LZMA_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
