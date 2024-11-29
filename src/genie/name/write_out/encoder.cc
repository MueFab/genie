/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/name/write_out/encoder.h"

#include <tuple>
#include <utility>

#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::name::write_out {

// -----------------------------------------------------------------------------
std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats>
Encoder::Process(const core::record::Chunk& recs) {
  const util::Watch watch;
  std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> ret =
      std::make_tuple(core::AccessUnit::Descriptor(core::GenDesc::kReadName),
                      core::stats::PerfStats());

  core::AccessUnit::Subsequence sub_seq(
      1, core::GenSubIndex(core::GenDesc::kReadName, 0));

  for (const auto& r : recs.GetData()) {
    for (auto& c : r.GetName()) {
      sub_seq.Push(c);
    }
    sub_seq.Push('\0');
  }
  std::get<0>(ret).Add(std::move(sub_seq));
  std::get<1>(ret).AddDouble("time-namewriteout", watch.Check());
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::name::write_out

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
