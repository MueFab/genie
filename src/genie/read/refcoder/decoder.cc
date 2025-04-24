/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.cc
 *
 * @brief Implementation of the reference-based decoder for the Genie framework.
 *
 * This file contains the implementation of the Decoder class in the `refcoder`
 * namespace. The Decoder is responsible for extracting reference substrings
 * from sequencing data during decoding, supporting both single-segment and
 * multi-segment reads.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/refcoder/decoder.h"

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "genie/read/basecoder/decoder.h"

// -----------------------------------------------------------------------------

namespace genie::read::refcoder {

// -----------------------------------------------------------------------------

std::vector<std::string> Decoder::GetReferences(
    const basecoder::Decoder::SegmentMeta& meta, DecodingState& state) {
  std::vector<std::string> ret;
  const auto& excerpt = dynamic_cast<RefDecodingState&>(state).ref_excerpt;
  {
    const auto begin = static_cast<uint32_t>(meta.position[0]);
    const auto end =
        std::min<uint32_t>(begin + static_cast<uint32_t>(meta.length[0]),
                           static_cast<uint32_t>(excerpt.GetGlobalEnd()));
    const auto clip_length =
        begin + static_cast<uint32_t>(meta.length[0]) - end;
    ret.emplace_back(excerpt.GetString(begin, end) +
                     std::string(clip_length, 'N'));
  }
  if (dynamic_cast<RefDecodingState&>(state).class_type ==
      core::record::ClassType::kClassHm) {
    ret.emplace_back(std::string(meta.length[1], '\0'));
  } else if (meta.num_segments == 2) {
    const auto begin = static_cast<uint32_t>(meta.position[1]);
    const auto end =
        std::min<uint32_t>(begin + static_cast<uint32_t>(meta.length[1]),
                           static_cast<uint32_t>(excerpt.GetGlobalEnd()));
    const auto clip_length =
        begin + static_cast<uint32_t>(meta.length[1]) - end;
    ret.emplace_back(excerpt.GetString(begin, end) +
                     std::string(clip_length, 'N'));
  }
  return ret;
}

// -----------------------------------------------------------------------------

std::unique_ptr<basecoder::DecoderStub::DecodingState>
Decoder::CreateDecodingState(core::AccessUnit& t) {
  return std::make_unique<RefDecodingState>(t);
}

// -----------------------------------------------------------------------------

Decoder::RefDecodingState::RefDecodingState(core::AccessUnit& t_data)
    : DecodingState(t_data),
      ref_excerpt(t_data.GetReferenceExcerpt()),
      class_type(t_data.GetClassType()) {}

// -----------------------------------------------------------------------------

}  // namespace genie::read::refcoder

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
