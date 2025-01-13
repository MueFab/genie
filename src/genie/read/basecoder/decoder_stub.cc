/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder_stub.cc
 * @brief Implementation of the DecoderStub class for decoding aligned genomic
 * records.
 *
 * This file contains the implementation of the DecoderStub class, which serves
 * as a base class for various aligned record decoders in the GENIE framework.
 * It handles the decoding of sequences, quality values, and read names from
 * access units, and provides hooks for additional processing.
 *
 * @details The DecoderStub class interacts with the core modules of the GENIE
 * framework, such as core::AccessUnit and core::record::Chunk, to manage the
 * flow of input data and support customized decoding logic. It includes methods
 * for decoding sequences, names, and quality values, and for adding extended
 * CIGAR strings to records.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#include "genie/read/basecoder/decoder_stub.h"

#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "genie/core/record/alignment_split/same_rec.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::read::basecoder {

// -----------------------------------------------------------------------------

DecoderStub::DecodingState::DecodingState(core::AccessUnit& t_data) {
  num_records = t_data.GetNumReads();
  num_segments = t_data.GetParameters().GetNumberTemplateSegments();
  ref = t_data.GetReference();
  qv_stream = std::move(t_data.Get(core::GenDesc::kQv));
  name_stream = std::move(t_data.Get(core::GenDesc::kReadName));
  qv_param = t_data.GetParameters().GetQvConfig(t_data.GetClassType()).Clone();
}

// -----------------------------------------------------------------------------

core::record::Chunk DecoderStub::DecodeSequences(DecodingState& state,
                                                 core::AccessUnit& t_data) {
  util::Watch watch;
  core::record::Chunk chunk;
  chunk.SetStats(std::move(t_data.GetStats()));
  Decoder decoder(std::move(t_data), state.num_segments, t_data.GetMinPos());
  for (size_t rec_id = 0; rec_id < state.num_records;) {
    auto meta = decoder.ReadSegmentMeta();

    auto refs = GetReferences(meta, state);

    auto rec =
        decoder.Pull(static_cast<uint16_t>(state.ref), std::move(refs), meta);
    AddECigar(rec, state.e_cigars, state.positions);
    RecordDecodedHook(state, rec);

    chunk.GetData().emplace_back(std::move(rec));
    rec_id += meta.num_segments;
  }
  chunk.GetStats().AddDouble("time-basecoder", watch.Check());
  return chunk;
}

// -----------------------------------------------------------------------------

void DecoderStub::DecodeNames(DecodingState& state,
                              core::record::Chunk& chunk) {
  const std::tuple<std::vector<std::string>, core::stats::PerfStats> names =
      namecoder_->Process(state.name_stream);
  chunk.GetStats().Add(std::get<1>(names));
  for (size_t i = 0; i < std::get<0>(names).size(); ++i) {
    chunk.GetData()[i].SetName(std::get<0>(names)[i]);
  }
}

// -----------------------------------------------------------------------------

void DecoderStub::DecodeQualities(DecodingState& state,
                                  core::record::Chunk& chunk) {
  auto qvs = qvcoder_->Process(*state.qv_param, state.e_cigars, state.positions,
                               state.qv_stream);
  chunk.GetStats().Add(std::get<1>(qvs));
  if (std::get<0>(qvs).empty()) {
    return;
  }
  size_t qv_counter = 0;
  for (auto& r : chunk.GetData()) {
    auto& s_first = r.GetSegments()[0];
    if (!std::get<0>(qvs)[qv_counter].empty()) {
      s_first.AddQualities(std::move(std::get<0>(qvs)[qv_counter++]));
      r.SetQvDepth(1);
    }

    if (r.GetSegments().size() == 1) {
      continue;
    }

    auto& s_second = r.GetSegments()[1];
    if (!std::get<0>(qvs)[qv_counter].empty()) {
      s_second.AddQualities(std::move(std::get<0>(qvs)[qv_counter++]));
    }
  }
}

// -----------------------------------------------------------------------------

void DecoderStub::AddECigar(const core::record::Record& rec,
                            std::vector<std::string>& cig_vec,
                            std::vector<uint64_t>& pos_vec) {
  const auto& s_first = rec.GetSegments()[0];

  if (rec.GetAlignments().empty()) {
    cig_vec.emplace_back(std::to_string(s_first.GetSequence().length()) + '+');
    pos_vec.emplace_back(std::numeric_limits<uint64_t>::max());
  } else {
    cig_vec.emplace_back(
        rec.GetAlignments().front().GetAlignment().GetECigar());
    pos_vec.emplace_back(rec.GetAlignments().front().GetPosition());
  }

  if (rec.GetSegments().size() == 1) {
    return;
  }

  if (rec.GetClassId() == core::record::ClassType::kClassHm) {
    const auto& s_second = rec.GetSegments()[1];
    cig_vec.emplace_back(std::to_string(s_second.GetSequence().length()) + '+');
    pos_vec.emplace_back(std::numeric_limits<uint64_t>::max());
    return;
  }

  const auto& split =
      dynamic_cast<const core::record::alignment_split::SameRec*>(
          rec.GetAlignments().front().GetAlignmentSplits().front().get());
  cig_vec.emplace_back(split->GetAlignment().GetECigar());
  pos_vec.emplace_back(rec.GetAlignments().front().GetPosition() +
                       split->GetDelta());
}

// -----------------------------------------------------------------------------

void DecoderStub::RecordDecodedHook(DecodingState&,
                                    const core::record::Record&) {}

// -----------------------------------------------------------------------------

void DecoderStub::FlowIn(core::AccessUnit&& t, const util::Section& id) {
  auto t_data = std::move(t);
  t_data = EntropyCodeAu(std::move(t_data), true);
  const auto state = CreateDecodingState(t_data);
  auto chunk = DecodeSequences(*state, t_data);
  DecodeQualities(*state, chunk);
  DecodeNames(*state, chunk);
  FlowOut(std::move(chunk), id);
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::basecoder

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
