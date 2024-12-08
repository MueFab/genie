/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/parameter/quality_values.h"

// -----------------------------------------------------------------------------

namespace genie::core::parameter {

// -----------------------------------------------------------------------------
uint8_t QualityValues::GetMode() const { return qv_coding_mode_; }

// -----------------------------------------------------------------------------
QualityValues::QualityValues(const uint8_t qv_coding_mode,
                             const bool qv_reverse_flag)
    : qv_coding_mode_(qv_coding_mode), qv_reverse_flag_(qv_reverse_flag) {}

// -----------------------------------------------------------------------------
bool QualityValues::Equals(const QualityValues* qv) const {
  return qv_coding_mode_ == qv->qv_coding_mode_ &&
         qv_reverse_flag_ == qv->qv_reverse_flag_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::parameter

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
