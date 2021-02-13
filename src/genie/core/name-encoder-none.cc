/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "name-encoder-none.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<AccessUnit::Descriptor, core::stats::PerfStats> NameEncoderNone::process(const record::Chunk&) {
    return std::make_tuple(AccessUnit::Descriptor(GenDesc::RNAME), core::stats::PerfStats());
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------