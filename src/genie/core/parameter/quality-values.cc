/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "quality-values.h"
#include "genie/util/exceptions.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace parameter {

// ---------------------------------------------------------------------------------------------------------------------

QualityValues::QualityValues(uint8_t _qv_coding_mode, bool _qv_reverse_flag)
    : qv_coding_mode(_qv_coding_mode), qv_reverse_flag(_qv_reverse_flag) {}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace parameter
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------