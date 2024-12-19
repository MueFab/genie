/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder_none.cc
 *
 * @brief Implementation of the NoneEncoder for the Genie framework.
 *
 * This file contains the implementation of the `NoneEncoder` class within the
 * `qvwriteout` namespace. The `NoneEncoder` is a no-operation encoder for
 * sequencing quality values (QV), returning empty parameters and descriptors
 * while maintaining compatibility with the Genie framework's interface.
 *
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/qvwriteout/encoder_none.h"

#include <memory>
#include <utility>

#include "genie/quality/paramqv1/qv_coding_config_1.h"

// -----------------------------------------------------------------------------

namespace genie::quality::qvwriteout {

// -----------------------------------------------------------------------------
core::QvEncoder::qv_coded NoneEncoder::Process(const core::record::Chunk&) {
  auto param = std::make_unique<paramqv1::QualityValues1>(
      paramqv1::QualityValues1::QualityParametersPresetId::ASCII, false);
  core::AccessUnit::Descriptor desc(core::GenDesc::kQv);

  return std::make_tuple(std::move(param), std::move(desc),
                         core::stats::PerfStats());
}

// -----------------------------------------------------------------------------

}  // namespace genie::quality::qvwriteout

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
