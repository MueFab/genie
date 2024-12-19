/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder.cc
 *
 * @brief Implements the Encoder class for processing and encoding genomic
 * record names.
 *
 * This file is part of the Genie project, which focuses on efficient genomic
 * data compression and processing. The `encoder.cpp` file handles the
 * tokenization and encoding of record names into compressed data streams,
 * enabling optimized storage and processing workflows.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/name/tokenizer/encoder.h"

#include <tuple>
#include <vector>

#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::name::tokenizer {

// -----------------------------------------------------------------------------
std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats>
Encoder::Process(const core::record::Chunk& recs) {
  const util::Watch watch;
  std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> ret =
      std::make_tuple(core::AccessUnit::Descriptor(core::GenDesc::kReadName),
                      core::stats::PerfStats());
  std::vector<SingleToken> old;

  for (const auto& r : recs.GetData()) {
    TokenState state(old, r.GetName());
    auto new_tok = state.run();
    TokenState::encode(new_tok, std::get<0>(ret));
    old = patch(old, new_tok);
  }
  std::get<1>(ret).AddDouble("time-nametokenizer", watch.Check());
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::name::tokenizer

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
