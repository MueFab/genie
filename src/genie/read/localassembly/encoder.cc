/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/localassembly/encoder.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "genie/quality/paramqv1/qv_coding_config_1.h"

// -----------------------------------------------------------------------------

namespace genie::read::localassembly {

// -----------------------------------------------------------------------------
Encoder::LaEncodingState::LaEncodingState(const core::record::Chunk& data,
                                          const uint32_t cr_buf_max_size)
    : EncodingState(data), ref_coder(cr_buf_max_size) {
  min_pos = data.GetData().front().GetAlignments().front().GetPosition();
  max_pos = 0;
}

// -----------------------------------------------------------------------------
void Encoder::PrintDebug(const LaEncodingState& state, const std::string& ref1,
                         const std::string& ref2,
                         const core::record::Record& r) const {
  if (!debug_) {
    return;
  }
  state.ref_coder.PrintWindow();
  std::cerr << "pair!" << std::endl;
  std::cerr << "ref1: " << std::endl;
  for (size_t i = 0; i < r.GetAlignments().front().GetPosition() -
                             state.ref_coder.GetWindowBorder();
       ++i) {
    std::cerr << " ";
  }
  std::cerr << ref1 << std::endl;

  std::cerr << "ref2: " << std::endl;
  for (size_t i = 0; i < r.GetAlignments().front().GetPosition() -
                             state.ref_coder.GetWindowBorder();
       ++i) {
    std::cerr << " ";
  }
  std::cerr << ref2 << std::endl;
  std::cerr << std::endl;
}

// -----------------------------------------------------------------------------
core::AccessUnit Encoder::Pack(const size_t id, core::QvEncoder::qv_coded qv,
                               core::AccessUnit::Descriptor read_name,
                               EncodingState& state) {
  auto ret = EncoderStub::Pack(id, std::move(qv), std::move(read_name), state);

  auto computed_reference_params = core::parameter::ComputedRef(
      core::parameter::ComputedRef::Algorithm::kLocalAssembly);
  computed_reference_params.SetExtension(core::parameter::ComputedRefExtended(
      0, dynamic_cast<LaEncodingState&>(state).ref_coder.GetMaxBufferSize()));
  ret.GetParameters().SetComputedRef(computed_reference_params);

  return ret;
}

// -----------------------------------------------------------------------------
std::pair<std::string, std::string> Encoder::GetReferences(
    const core::record::Record& r, EncodingState& state) {
  std::pair<std::string, std::string> ret;
  {
    const auto begin = r.GetAlignments().front().GetPosition();
    const auto e_cigar = r.GetAlignments().front().GetAlignment().GetECigar();
    ret.first = dynamic_cast<LaEncodingState&>(state).ref_coder.GetReference(
        static_cast<uint32_t>(begin), e_cigar);

    // Update AU end
    const auto end = begin + core::record::Record::GetLengthOfCigar(e_cigar);
    state.max_pos = std::max(state.max_pos, end);
  }

  if (r.GetSegments().size() > 1) {
    const auto& second_record =
        *reinterpret_cast<const core::record::alignment_split::SameRec*>(
            r.GetAlignments().front().GetAlignmentSplits().front().get());
    const auto begin =
        r.GetAlignments().front().GetPosition() + second_record.GetDelta();
    const auto e_cigar = second_record.GetAlignment().GetECigar();
    ret.second = dynamic_cast<LaEncodingState&>(state).ref_coder.GetReference(
        static_cast<uint32_t>(begin), e_cigar);

    // Update AU end
    const auto end = begin + second_record.GetDelta() +
                     core::record::Record::GetLengthOfCigar(e_cigar);
    state.max_pos = std::max(state.max_pos, end);
  }

  // Update computed reference
  dynamic_cast<LaEncodingState&>(state).ref_coder.AddRead(r);
  PrintDebug(dynamic_cast<LaEncodingState&>(state), ret.first, ret.second, r);
  return ret;
}

// -----------------------------------------------------------------------------
std::unique_ptr<Encoder::EncodingState> Encoder::CreateState(
    const core::record::Chunk& data) const {
  constexpr uint32_t reads_per_assembly = 10;
  uint32_t max_read_size = 0;
  for (const auto& r : data.GetData()) {
    for (const auto& s : r.GetSegments()) {
      max_read_size = std::max(max_read_size,
                               static_cast<uint32_t>(s.GetSequence().length()));
    }
  }
  uint32_t buf_max_size = 1024;
  while (buf_max_size < max_read_size * reads_per_assembly) {
    buf_max_size *= 2;
  }
  buf_max_size = std::min(buf_max_size, (1u << 24u) - 1u);
  return std::make_unique<LaEncodingState>(data, buf_max_size);
}

// -----------------------------------------------------------------------------
Encoder::Encoder(const bool debug, const bool write_raw)
    : EncoderStub(write_raw), debug_(debug) {}

// -----------------------------------------------------------------------------
void Encoder::FlowIn(core::record::Chunk&& t, const util::Section& id) {
  core::record::Chunk d = std::move(t);

  // Class N is not supported in local assembly because of the following
  // situation: AANAAAAAAAAA  <- Old reads  // NOLINT
  //  ANAAAAAAAAAAA                         // NOLINT
  //
  //   AAAAAAANAAAAA  <- New read           // NOLINT
  //   ^ Needs to patch N with A (not possible in class N), even though class
  //   of individual reads is correct
  // Downgrade to class M!
  for (auto& r : d.GetData()) {
    if (r.GetClassId() == core::record::ClassType::kClassN) {
      r.SetClassType(core::record::ClassType::kClassM);
    }
  }

  EncoderStub::FlowIn(std::move(d), id);
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::localassembly

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
