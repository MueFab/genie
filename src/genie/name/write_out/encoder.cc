/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder.cc
 *
 * @brief Implementation of the Encoder class for writing out read names.
 *
 * This file contains the implementation of the `Encoder::Process` method,
 * which processes a chunk of genomic records to generate access unit
 * descriptors and performance statistics for the "Read Name" component. It
 * makes use of utilities like the `StopWatch` for measuring performance and
 * handles the encoding of read names into subsequences for downstream
 * processing.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
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
