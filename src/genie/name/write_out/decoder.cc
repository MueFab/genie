/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.cc
 *
 * @brief Implementation of the Decoder class for parsing and extracting read
 * names.
 *
 * This file contains the implementation of the `Decoder::Process` method,
 * which decodes access unit descriptors to extract read names and generates
 * performance statistics. It efficiently processes the encoded subsequences,
 * reconstructing the original read names into a vector of strings.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/name/write_out/decoder.h"

#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::name::write_out {

// -----------------------------------------------------------------------------
std::tuple<std::vector<std::string>, core::stats::PerfStats> Decoder::Process(
    core::AccessUnit::Descriptor& desc) {
  std::tuple<std::vector<std::string>, core::stats::PerfStats> ret;
  const util::Watch watch;
  std::string cur_name;
  if (desc.IsEmpty()) {
    return ret;
  }
  while (!desc.Get(0).end()) {
    if (const auto cur_char = desc.Get(0).Pull(); cur_char == '\0') {
      std::get<0>(ret).emplace_back(std::move(cur_name));
      cur_name.clear();
    } else {
      cur_name.push_back(static_cast<char>(cur_char));
    }
  }

  std::get<1>(ret).AddDouble("time-namewriteout", watch.Check());
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::name::write_out

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
