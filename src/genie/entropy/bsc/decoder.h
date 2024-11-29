/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @brief Defines the BSC-specific entropy decoder class for decompressing
 * access units.
 * @details This file contains the implementation of the `Decoder` class, which
 * is responsible for decompressing entropy-coded block payloads back into raw
 * access units using the GABAC algorithm. The class inherits from the base
 * `EntropyDecoder` interface and overrides the necessary methods to handle
 * MPEG-G descriptors.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_ENTROPY_BSC_DECODER_H_
#define SRC_GENIE_ENTROPY_BSC_DECODER_H_

// -----------------------------------------------------------------------------

#include <tuple>

#include "genie/core/access_unit.h"
#include "genie/core/entropy_decoder.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::bsc {

/**
 * @brief Class to Decompress entropy-coded block payloads into raw access
 * units.
 * @details This decoder class is responsible for handling the decompression of
 * GABAC-encoded access units. It implements the `EntropyDecoder` interface and
 * provides the functionality to reverse entropy coding on block payloads and
 * restore the original data.
 */
class Decoder final : public core::EntropyDecoder {
 public:
  /**
   * @brief Decompress a given descriptor using the specified subsequence
   * configuration.
   * @param param Configuration of the descriptor subsequence.
   * @param d Descriptor to be processed and decompressed.
   * @param mm_coder_enabled Flag indicating if the mismatchCoder is enabled.
   * @return A tuple containing the decompressed descriptor and performance
   * statistics.
   * @details This method takes a descriptor and its subsequence
   * configuration, applies the decompression algorithm, and returns the
   * restored descriptor along with associated performance statistics.
   */
  std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> Process(
      const core::parameter::DescriptorSubSequenceCfg& param,
      core::AccessUnit::Descriptor& d, bool mm_coder_enabled) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::bsc

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_BSC_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
