/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder.cc
 *
 * @brief Implementation of the reference-based encoder for the Genie framework.
 *
 * This file contains the implementation of the Encoder class in the `refcoder`
 * namespace. The Encoder facilitates reference-based encoding for sequencing
 * data, extracting reference sequences based on alignment information and
 * managing encoding states.
 *
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/refcoder/encoder.h"

#include <memory>
#include <string>
#include <utility>

#include "genie/read/basecoder/encoder.h"

// -----------------------------------------------------------------------------

namespace genie::read::refcoder {

// -----------------------------------------------------------------------------

Encoder::RefEncodingState::RefEncodingState(const core::record::Chunk& data)
    : EncodingState(data) {
  excerpt = data.GetRef();
}

// -----------------------------------------------------------------------------

std::pair<std::string, std::string> Encoder::GetReferences(
    const core::record::Record& r, EncodingState& state) {
  std::pair<std::string, std::string> ret;
  {
    const auto begin = r.GetAlignments().front().GetPosition();
    const auto length = core::record::Record::GetLengthOfCigar(
        r.GetAlignments().front().GetAlignment().GetECigar());
    const auto end = begin + length;
    ret.first =
        dynamic_cast<RefEncodingState&>(state).excerpt.GetString(begin, end);
  }

  if (r.GetSegments().size() > 1) {
    const auto& second_record =
        *reinterpret_cast<const core::record::alignment_split::SameRec*>(
            r.GetAlignments().front().GetAlignmentSplits().front().get());
    const auto begin =
        r.GetAlignments().front().GetPosition() + second_record.GetDelta();
    const auto length = core::record::Record::GetLengthOfCigar(
        second_record.GetAlignment().GetECigar());
    const auto end = begin + length;
    ret.second =
        dynamic_cast<RefEncodingState&>(state).excerpt.GetString(begin, end);
  }

  return ret;
}

// -----------------------------------------------------------------------------

std::unique_ptr<Encoder::EncodingState> Encoder::CreateState(
    const core::record::Chunk& data) const {
  return std::make_unique<RefEncodingState>(data);
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::refcoder

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
