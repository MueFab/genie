/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.cc
 *
 * @brief Implementation of the LocalAssembly decoder for the Genie framework.
 *
 * This file contains the implementation of the `Decoder` class within the
 * `localassembly` namespace. The decoder reconstructs sequencing reads by
 * utilizing local reference sequences generated during decoding.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/localassembly/decoder.h"

#include <memory>
#include <string>
#include <vector>

#include "genie/read/basecoder/decoder.h"

// -----------------------------------------------------------------------------

namespace genie::read::localassembly {

// -----------------------------------------------------------------------------
std::vector<std::string> Decoder::GetReferences(
    const basecoder::Decoder::SegmentMeta& meta, DecodingState& state) {
  std::vector<std::string> ret;
  ret.emplace_back(
      dynamic_cast<LaDecodingState&>(state).ref_encoder.GetReference(
          static_cast<uint32_t>(meta.position[0]),
          static_cast<uint32_t>(meta.length[0])));
  if (meta.num_segments == 2) {
    ret.emplace_back(
        dynamic_cast<LaDecodingState&>(state).ref_encoder.GetReference(
            static_cast<uint32_t>(meta.position[1]),
            static_cast<uint32_t>(meta.length[1])));
  }
  return ret;
}

// -----------------------------------------------------------------------------
std::unique_ptr<basecoder::DecoderStub::DecodingState>
Decoder::CreateDecodingState(core::AccessUnit& t) {
  return std::make_unique<LaDecodingState>(t);
}

// -----------------------------------------------------------------------------
void Decoder::RecordDecodedHook(DecodingState& state,
                                const core::record::Record& rec) {
  dynamic_cast<LaDecodingState&>(state).ref_encoder.AddRead(rec);
}

// -----------------------------------------------------------------------------
Decoder::LaDecodingState::LaDecodingState(core::AccessUnit& t_data)
    : DecodingState(t_data),
      ref_encoder(t_data.GetParameters()
                      .GetComputedRef()
                      .GetExtension()
                      .GetBufMaxSize()) {}

// -----------------------------------------------------------------------------

}  // namespace genie::read::localassembly

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
