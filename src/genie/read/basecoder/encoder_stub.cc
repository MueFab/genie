/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder_stub.cc
 * @brief Implementation of the EncoderStub class for encoding genomic records.
 *
 * This file contains the implementation of the EncoderStub class, which serves
 * as a base class for aligned record encoders in the GENIE framework. It
 * handles the encoding of record sequences, quality values, and read names for
 * a chunk of genomic data.
 *
 * @details The EncoderStub class provides a common interface and set of
 * utilities for encoding genomic data records. It manages the state for the
 * encoding process and defines the interface for specialized record encoders.
 * The class includes methods for encoding sequences, packing encoded data into
 * access units, and removing redundant descriptors.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#include "genie/read/basecoder/encoder_stub.h"

#include <string>
#include <utility>

#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::read::basecoder {

// -----------------------------------------------------------------------------
EncoderStub::EncodingState::EncodingState(const core::record::Chunk& data)
    : read_coder(data.GetData().front().GetAlignments().front().GetPosition()),
      paired_end(data.GetData().front().GetNumberOfTemplateSegments() > 1),
      read_length(
          data.GetData().front().GetSegments().front().GetSequence().length()),
      class_type(data.GetData().front().GetClassId()),
      min_pos(data.GetRef().GetGlobalStart()),
      max_pos(data.GetRef().GetGlobalEnd()),
      ref(data.GetData().front().GetAlignmentSharedData().GetSeqId()),
      read_num(0),
      last_read_position(0) {}

// -----------------------------------------------------------------------------
void EncoderStub::EncodeSeq(core::record::Chunk& data, EncodingState& state) {
  const util::Watch watch;
  for (auto& r : data.GetData()) {
    state.read_num += r.GetSegments().size();
    const uint64_t cur_pos = r.GetAlignments().front().GetPosition();
    UTILS_DIE_IF(cur_pos < state.last_read_position,
                 "Data seems to be unsorted. Local assembly encoding needs "
                 "sorted input data.");

    state.last_read_position = cur_pos;

    // Check if length constant
    for (const auto& s : r.GetSegments()) {
      if (s.GetSequence().length() != state.read_length) {
        state.read_length = 0;
      }
    }

    UTILS_DIE_IF(r.GetAlignmentSharedData().GetSeqId() != state.ref,
                 "Records belonging to different reference sequences in "
                 "one access unit");

    auto [fst, snd] = GetReferences(r, state);
    state.read_coder.Add(r, fst, snd);
  }
  data.GetStats().AddDouble("time-basecoder", watch.Check());
}

// -----------------------------------------------------------------------------
core::AccessUnit EncoderStub::Pack(const size_t id,
                                   core::QvEncoder::qv_coded qv,
                                   core::AccessUnit::Descriptor read_name,
                                   EncodingState& state) {
  constexpr auto data_type = core::parameter::DataUnit::DatasetType::kAligned;
  const auto qv_depth = static_cast<uint8_t>(std::get<1>(qv).IsEmpty() ? 0 : 1);
  core::parameter::ParameterSet ret(
      static_cast<uint8_t>(id), static_cast<uint8_t>(id), data_type,
      core::AlphabetId::kAcgtn, static_cast<uint32_t>(state.read_length),
      state.paired_end, false, qv_depth, 1, false, false);
  ret.GetEncodingSet().AddClass(state.class_type, std::move(std::get<0>(qv)));

  auto raw_au = state.read_coder.MoveStreams();

  raw_au.Get(core::GenDesc::kQv) = std::move(std::get<1>(qv));
  raw_au.Get(core::GenDesc::kReadName) = std::move(read_name);

  raw_au.SetParameters(std::move(ret.GetEncodingSet()));
  raw_au.SetReference(state.ref);
  raw_au.SetMinPos(state.min_pos);
  raw_au.SetMaxPos(state.max_pos);
  raw_au.SetClassType(state.class_type);
  raw_au.SetNumReads(state.read_num);

  RemoveRedundantDescriptors(raw_au);
  return raw_au;
}

// -----------------------------------------------------------------------------
void EncoderStub::RemoveRedundantDescriptors(core::AccessUnit& raw_au) {
  if (raw_au.GetClassType() < core::record::ClassType::kClassI) {
    raw_au.Get(core::gen_sub::kClipsHardClip) =
        core::AccessUnit::Subsequence(core::gen_sub::kClipsHardClip);
    raw_au.Get(core::gen_sub::kClipsType) =
        core::AccessUnit::Subsequence(core::gen_sub::kClipsType);
    raw_au.Get(core::gen_sub::kClipsSoftClip) =
        core::AccessUnit::Subsequence(core::gen_sub::kClipsSoftClip);
    raw_au.Get(core::gen_sub::kClipsRecordId) =
        core::AccessUnit::Subsequence(core::gen_sub::kClipsRecordId);
    raw_au.Get(core::gen_sub::kMismatchTypeInsert) =
        core::AccessUnit::Subsequence(core::gen_sub::kMismatchTypeInsert);
    raw_au.Get(core::gen_sub::kMismatchType) =
        core::AccessUnit::Subsequence(core::gen_sub::kMismatchType);
  }
  if (raw_au.GetClassType() < core::record::ClassType::kClassM) {
    raw_au.Get(core::gen_sub::kMismatchTypeSubstBase) =
        core::AccessUnit::Subsequence(core::gen_sub::kMismatchTypeSubstBase);
  }
  if (raw_au.GetClassType() < core::record::ClassType::kClassN) {
    raw_au.Get(core::gen_sub::kMismatchPosTerminator) =
        core::AccessUnit::Subsequence(core::gen_sub::kMismatchPosTerminator);
    raw_au.Get(core::gen_sub::kMismatchPosDelta) =
        core::AccessUnit::Subsequence(core::gen_sub::kMismatchPosDelta);
  }
}

// -----------------------------------------------------------------------------
core::QvEncoder::qv_coded EncoderStub::EncodeQVs(qv_selector* qv_coder,
                                                 core::record::Chunk& data) {
  const util::Watch watch;
  auto qv = qv_coder->Process(data);
  data.GetStats().AddDouble("time-quality", watch.Check());
  return qv;
}

// -----------------------------------------------------------------------------
core::AccessUnit::Descriptor EncoderStub::EncodeNames(
    name_selector* name_coder, core::record::Chunk& data) {
  const util::Watch watch;
  auto name = name_coder->Process(data);
  data.GetStats().AddDouble("time-name", watch.Check());
  return std::get<0>(name);
}

// -----------------------------------------------------------------------------
void EncoderStub::FlowIn(core::record::Chunk&& t, const util::Section& id) {
  core::record::Chunk data = std::move(t);

  // Empty block: do nothing
  if (data.GetData().empty()) {
    SkipOut(id);
    return;
  }

  auto qv = EncodeQVs(qvcoder_, data);
  auto read_name = EncodeNames(namecoder_, data);

  const auto state = CreateState(data);
  EncodeSeq(data, *state);

  auto raw_au = Pack(id.start, std::move(qv), std::move(read_name), *state);
  raw_au.SetStats(std::move(data.GetStats()));
  data.GetData().clear();
  raw_au = EntropyCodeAu(std::move(raw_au));
  FlowOut(std::move(raw_au), id);
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::basecoder

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
