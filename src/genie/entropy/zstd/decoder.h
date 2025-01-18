/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.h
 * @brief Header file for the ZSTD entropy decoder module.
 * @details This file declares the `Decoder` class, which provides functionality
 * for decompressing a block payload back into a raw access unit using the ZSTD
 * compression algorithm. The class is a part of the GENIE MPEG-G toolkit and
 * implements the required functionality to process MPEG-G descriptors and
 * generate decompressed access units.
 * @copyright This file is part of Genie
 *            See LICENSE and/or https://github.com/MueFab/genie for more
 * details.
 */

#ifndef SRC_GENIE_ENTROPY_ZSTD_DECODER_H_
#define SRC_GENIE_ENTROPY_ZSTD_DECODER_H_

// -----------------------------------------------------------------------------

#include <tuple>

#include "genie/core/access_unit.h"
#include "genie/core/entropy_decoder.h"

// -----------------------------------------------------------------------------

namespace genie::entropy::zstd {

/**
 * @brief Decoder class for the ZSTD compression algorithm.
 * @details This class handles the decompression of block payloads encoded using
 * the ZSTD algorithm. It converts the compressed descriptors back into raw
 * access units, following the MPEG-G specifications and utilizing the GENIE
 * core infrastructure.
 */
class Decoder final : public core::EntropyDecoder {
 public:
  /**
   * @brief Decompress a block payload into a raw access unit.
   * @param param Descriptor configuration parameters for subsequence
   * processing.
   * @param d The descriptor to be decompressed.
   * @param mm_coder_enabled Flag to indicate if mismatchCoder is enabled.
   * @return A tuple containing the decompressed descriptor and performance
   * statistics.
   */
  std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> Process(
      const core::parameter::DescriptorSubSequenceCfg& param,
      core::AccessUnit::Descriptor& d, bool mm_coder_enabled) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::entropy::zstd

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ENTROPY_ZSTD_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
