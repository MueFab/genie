/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/name/tokenizer/encoder.h"
#include <tuple>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::name::tokenizer {

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> Encoder::process(const core::record::Chunk& recs) {
    util::Watch watch;
    std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> ret =
        std::make_tuple(core::AccessUnit::Descriptor(core::GenDesc::RNAME), core::stats::PerfStats());
    std::vector<SingleToken> old;

    for (const auto& r : recs.getData()) {
        TokenState state(old, r.getName());
        auto newTok = state.run();
        TokenState::encode(newTok, std::get<0>(ret));
        old = patch(old, newTok);
    }
    std::get<1>(ret).addDouble("time-nametokenizer", watch.check());
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::name::tokenizer

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
