/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_ENTROPY_ENCODER_H_
#define SRC_GENIE_CORE_ENTROPY_ENCODER_H_

// -----------------------------------------------------------------------------

#include <tuple>

#include "genie/core/access_unit.h"
#include "genie/core/module.h"
#include "genie/core/parameter/descriptor_present/decoder.h"
#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/record/annotation_parameter_set/record.h"

// -----------------------------------------------------------------------------

namespace genie::core {

class EntropyEncoderAnnotation {
 public:
    virtual ~EntropyEncoderAnnotation() = default;
    virtual core::record::annotation_access_unit::Record process(
        core::record::annotation_parameter_set::DescriptorConfiguration& desc) = 0;
};

/**
 * @brief Interface for entropy coders. They convert raw access units to access
 * unit payloads
 */
class EntropyEncoder {
 public:
  using entropy_coded =
      std::tuple<parameter::DescriptorSubSequenceCfg, AccessUnit::Descriptor,
                 stats::PerfStats>;  //!< @brief

  /**
   * @brief
   */
  virtual ~EntropyEncoder() = default;

  /**
   * @brief
   * @param desc
   * @return
   */
  virtual entropy_coded Process(AccessUnit::Descriptor& desc) = 0;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_ENTROPY_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
