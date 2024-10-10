/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/name/writeout/encoder.h"
#include <tuple>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::name::writeout {

// ---------------------------------------------------------------------------------------------------------------------

std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> Encoder::process(const core::record::Chunk& recs) {
    util::Watch watch;
    std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> ret =
        std::make_tuple(core::AccessUnit::Descriptor(core::GenDesc::RNAME), core::stats::PerfStats());

    core::AccessUnit::Subsequence subseq(1, core::GenSubIndex(core::GenDesc::RNAME, 0));

    for (const auto& r : recs.getData()) {
        for (auto& c : r.getName()) {
            subseq.push(c);
        }
        subseq.push('\0');
    }
    std::get<0>(ret).add(std::move(subseq));
    std::get<1>(ret).addDouble("time-namewriteout", watch.check());
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::name::writeout

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
