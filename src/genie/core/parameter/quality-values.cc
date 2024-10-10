/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/parameter/quality-values.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::parameter {

// ---------------------------------------------------------------------------------------------------------------------

uint8_t QualityValues::getMode() const { return qv_coding_mode; }

// ---------------------------------------------------------------------------------------------------------------------

QualityValues::QualityValues(uint8_t _qv_coding_mode, bool _qv_reverse_flag)
    : qv_coding_mode(_qv_coding_mode), qv_reverse_flag(_qv_reverse_flag) {}

// ---------------------------------------------------------------------------------------------------------------------

bool QualityValues::equals(const QualityValues* qv) const {
    return qv_coding_mode == qv->qv_coding_mode && qv_reverse_flag == qv->qv_reverse_flag;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::parameter

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
