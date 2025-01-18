/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_ENTROPY_DECODER_H_
#define SRC_GENIE_CORE_ENTROPY_DECODER_H_

// -----------------------------------------------------------------------------

#include <tuple>

#include "genie/core/access_unit.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief Interface for entropy decoders. They convert access unit payloads to
 * raw access units
 */
class EntropyDecoder {
 public:
  /**
   * @brief
   */
  virtual ~EntropyDecoder() = default;

  /**
   * @brief
   * @param param
   * @param desc
   * @param mm_coder_enabled
   * @return
   */
  virtual std::tuple<AccessUnit::Descriptor, stats::PerfStats> Process(
      const parameter::DescriptorSubSequenceCfg& param,
      AccessUnit::Descriptor& desc, bool mm_coder_enabled) = 0;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_ENTROPY_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
