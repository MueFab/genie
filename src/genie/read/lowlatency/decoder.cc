/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.cc
 *
 * @brief Implementation of the low-latency decoder for the Genie framework.
 *
 * This file contains the implementation of the Decoder class within the
 * `lowlatency` namespace. The decoder processes sequencing data from Access
 * Units (AUs) back into record chunks, supporting low-latency operations for
 * unaligned reads.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/lowlatency/decoder.h"

#include <string>
#include <utility>
#include <vector>

#include "genie/core/global_cfg.h"
#include "genie/read/basecoder/decoder.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::read::lowlatency {

// -----------------------------------------------------------------------------
core::record::Chunk Decoder::decode_common(core::AccessUnit&& t) const {
  util::Watch watch;
  core::record::Chunk ret;
  core::AccessUnit data = std::move(t);
  data = EntropyCodeAu(std::move(data), true);
  const auto& qv_param = data.GetParameters().GetQvConfig(data.GetClassType());
  auto qv_stream = std::move(data.Get(core::GenDesc::kQv));
  auto names = namecoder_->Process(data.Get(core::GenDesc::kReadName));
  data.GetStats().AddDouble("time-name", watch.Check());
  watch.Reset();
  std::vector<std::string> e_cigars;
  std::vector<uint64_t> positions;
  // FIXME: loop condition is only correct if all records have the full number
  // of reads
  size_t i = 0;
  size_t rec_i = 0;
  while (i < data.GetNumReads()) {
    core::record::Record rec(
        static_cast<uint8_t>(data.GetParameters().GetNumberTemplateSegments()),
        core::record::ClassType::kClassU,
        std::get<0>(names).empty() ? "" : std::move(std::get<0>(names)[rec_i]),
        "", 0);

    size_t num_segments = 1;
    if (data.GetParameters().GetNumberTemplateSegments() > 1) {
      if (auto decoding_case = data.Pull(core::gen_sub::kPairDecodingCase);
          decoding_case == core::gen_const::kPairSameRecord) {
        num_segments = 2;
      } else if (decoding_case == core::gen_const::kPairR1Unpaired) {
        rec.SetRead1First(true);
      } else {
        rec.SetRead1First(false);
      }
    }

    for (size_t j = 0; j < num_segments; ++j) {
      size_t length = data.GetParameters().GetReadLength();
      e_cigars.emplace_back(length, '+');
      positions.emplace_back(std::numeric_limits<uint64_t>::max());
      if (!length) {
        length = data.Pull(core::gen_sub::kReadLength) + 1;
      }
      std::string seq(length, '\0');
      for (auto& c : seq) {
        c = GetAlphabetProperties(core::AlphabetId::kAcgtn)
                .lut[data.Pull(core::gen_sub::kUnalignedReads)];
      }

      core::record::Segment seg(std::move(seq));
      rec.AddSegment(std::move(seg));
    }

    ret.GetData().push_back(std::move(rec));
    i += num_segments;
    rec_i++;
  }

  data.GetStats().AddDouble("time-lowlatency", watch.Check());
  watch.Reset();
  if (auto qvs =
          this->qvcoder_->Process(qv_param, e_cigars, positions, qv_stream);
      !std::get<0>(qvs).empty()) {
    size_t qv_counter = 0;
    for (auto& r : ret.GetData()) {
      for (auto& s : r.GetSegments()) {
        if (!std::get<0>(qvs)[qv_counter].empty()) {
          s.AddQualities(std::move(std::get<0>(qvs)[qv_counter]));
        }
        qv_counter++;
      }
      if (!r.GetSegments().empty()) {
        r.SetQvDepth(static_cast<uint8_t>(
            r.GetSegments().front().GetQualities().size()));
      }
    }
  }

  data.GetStats().AddDouble("time-qv", watch.Check());
  watch.Reset();

  ret.SetStats(std::move(data.GetStats()));
  data.Clear();
  return ret;
}

// -----------------------------------------------------------------------------
void Decoder::FlowIn(core::AccessUnit&& t, const util::Section& id) {
  FlowOut(decode_common(std::move(t)), id);
}

// -----------------------------------------------------------------------------
std::string Decoder::Decode(core::AccessUnit&& t) {
  return decode_common(std::move(t))
      .GetData()
      .front()
      .GetSegments()
      .front()
      .GetSequence();
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::lowlatency

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
