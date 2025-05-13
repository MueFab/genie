/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder.cc
 *
 * @brief Implementation of the low-latency encoder for the Genie framework.
 *
 * This file contains the implementation of the Encoder class within the
 * `lowlatency` namespace. The encoder processes sequencing data into Access
 * Units (AUs) with low-latency considerations, supporting both reference-based
 * and non-reference workflows.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/lowlatency/encoder.h"

#include <memory>
#include <utility>

#include "genie/name/tokenizer/decoder.h"
#include "genie/name/tokenizer/encoder.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::read::lowlatency {

// -----------------------------------------------------------------------------

void EncodeFlags(const uint8_t flags, Encoder::LlState& state) {
  state.streams.Push(core::gen_sub::kFlagsNotPrimary,
                     (flags & core::gen_const::kFlagsNotPrimaryMask) >>
                         core::gen_const::kFlagsNotPrimaryPos);
  state.streams.Push(core::gen_sub::kFlagsSupplementary,
                     (flags & core::gen_const::kFlagsSupplementaryMask) >>
                         core::gen_const::kFlagsSupplementaryPos);
  state.streams.Push(core::gen_sub::kFlagsPcrDuplicate,
                     (flags & core::gen_const::kFlagsPcrDuplicateMask) >>
                         core::gen_const::kFlagsPcrDuplicatePos);
  state.streams.Push(core::gen_sub::kFlagsProperPair,
                     (flags & core::gen_const::kFlagsProperPairMask) >>
                         core::gen_const::kFlagsProperPairPos);
  state.streams.Push(core::gen_sub::kFlagsQualityFail,
                     (flags & core::gen_const::kFlagsQualityFailMask) >>
                         core::gen_const::kFlagsQualityFailPos);
}

void Encoder::FlowIn(core::record::Chunk&& t, const util::Section& id) {
  util::Watch watch;
  core::record::Chunk data = std::move(t);

  if (data.GetData().empty()) {
    core::parameter::ParameterSet set;
    core::AccessUnit au(std::move(set.GetEncodingSet()), 0);
    au.SetReference(data.GetRef(), data.GetRefToWrite());
    au.SetReference(static_cast<uint16_t>(data.GetRefId()));
    FlowOut(std::move(au), id);
    return;
  }

  core::parameter::ParameterSet set;

  LlState state{
      data.GetData().front().GetSegments().front().GetSequence().length(),
      data.GetData().front().GetNumberOfTemplateSegments() > 1,
      core::AccessUnit(std::move(set.GetEncodingSet()), data.GetData().size()),
      data.IsReferenceOnly(),
      data.GetData().front().IsExtendedAlignment(),
      demultiplex_tag::Encoder()};
  size_t num_reads = 0;
  for (auto& r : data.GetData()) {
    if (!state.extended_alignment) {
      EncodeFlags(r.GetFlags().front(), state);
    } else {
      for (size_t i = 0; i < r.GetSegments().size(); ++i) {
        EncodeFlags(r.GetFlags()[i], state);
      }
    }
    for (auto& s : r.GetSegments()) {
      num_reads++;
      state.streams.Push(core::gen_sub::kReadLength,
                         s.GetSequence().length() - 1);
      if (state.read_length != s.GetSequence().length()) {
        state.read_length = 0;
      }

      for (auto c : s.GetSequence()) {
        state.streams.Push(
            core::gen_sub::kUnalignedReads,
            GetAlphabetProperties(core::AlphabetId::kAcgtn).inverse_lut[c]);
      }
    }
    state.demultiplex_tag_encoder.EncodeTags(r.GetTags().front());
    if (r.GetSegments().size() > 1) {
      state.streams.Push(core::gen_sub::kPairDecodingCase,
                         core::gen_const::kPairSameRecord);
      state.demultiplex_tag_encoder.EncodeTags(r.GetTags()[1]);
    } else if (r.GetNumberOfTemplateSegments() > 1) {
      if (r.GetRead1First()) {
        state.streams.Push(core::gen_sub::kPairDecodingCase,
                           core::gen_const::kPairR1Unpaired);
      } else {
        state.streams.Push(core::gen_sub::kPairDecodingCase,
                           core::gen_const::kPairR2Unpaired);
      }
    }
  }
  watch.Pause();

  auto qv = qvcoder_->Process(data);
  auto read_names = namecoder_->Process(data);
  watch.Resume();
  auto raw_au = pack(id, std::get<1>(qv).IsEmpty() ? 0 : 1,
                     std::move(std::get<0>(qv)), state);

  if (!data.IsReferenceOnly()) {
    raw_au.Get(core::GenDesc::kQv) = std::move(std::get<1>(qv));
    raw_au.Get(core::GenDesc::kReadName) = std::move(std::get<0>(read_names));
  }
  if (state.read_length != 0) {
    raw_au.Set(core::gen_sub::kReadLength,
               core::AccessUnit::Subsequence(core::gen_sub::kReadLength));
  }
  raw_au.Set(core::GenDesc::kTag,
    state.demultiplex_tag_encoder.MoveDescriptor());

  raw_au.SetStats(std::move(data.GetStats()));
  raw_au.GetStats().AddDouble("time-lowlatency", watch.Check());
  raw_au.GetStats().Add(std::get<2>(qv));
  raw_au.GetStats().Add(std::get<1>(read_names));
  raw_au = EntropyCodeAu(std::move(raw_au));
  raw_au.SetNumReads(num_reads);
  raw_au.SetReferenceOnly(data.IsReferenceOnly());
  raw_au.SetReference(static_cast<uint16_t>(data.GetRefId()));
  raw_au.SetReference(data.GetRef(), {});
  data.GetData().clear();
  FlowOut(std::move(raw_au), id);
}

// -----------------------------------------------------------------------------

core::AccessUnit Encoder::pack(
    const util::Section& id, const uint8_t qv_depth,
    std::unique_ptr<core::parameter::QualityValues> quality_value_params,
    LlState& state) {
  const core::parameter::DataUnit::DatasetType data_type =
      state.ref_only ? core::parameter::DataUnit::DatasetType::kReference
                     : core::parameter::DataUnit::DatasetType::kNonAligned;
  core::parameter::ParameterSet ret(
      static_cast<uint8_t>(id.start), static_cast<uint8_t>(id.start), data_type,
      core::AlphabetId::kAcgtn, static_cast<uint32_t>(state.read_length),
      state.paired_end, false, qv_depth, 0, false, false,
      state.extended_alignment);
  ret.GetEncodingSet().AddClass(core::record::ClassType::kClassU,
                                std::move(quality_value_params));

  auto raw_au = std::move(state.streams);

  raw_au.SetParameters(std::move(ret.GetEncodingSet()));
  raw_au.SetReference(0);
  raw_au.SetMinPos(0);
  raw_au.SetMaxPos(0);
  raw_au.SetClassType(core::record::ClassType::kClassU);

  return raw_au;
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::lowlatency

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
