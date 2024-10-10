/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/name-encoder-none.h"
#include <tuple>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<AccessUnit::Descriptor, core::stats::PerfStats> NameEncoderNone::process(const record::Chunk&) {
    return std::make_tuple(AccessUnit::Descriptor(GenDesc::RNAME), core::stats::PerfStats());
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
