/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
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
