/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/read_decoder.h"

#include <utility>

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------

void ReadDecoder::SetQvCoder(qv_selector* coder) { qvcoder_ = coder; }

// -----------------------------------------------------------------------------

void ReadDecoder::SetNameCoder(name_selector* coder) { namecoder_ = coder; }

// -----------------------------------------------------------------------------

void ReadDecoder::SetEntropyCoder(entropy_selector* coder) {
  entropycoder_ = coder;
}

// -----------------------------------------------------------------------------

AccessUnit ReadDecoder::EntropyCodeAu(entropy_selector* select, AccessUnit&& a,
                                      const bool mm_coder_enabled) {
  AccessUnit au = std::move(a);
  for (auto& d : au) {
    auto enc = select->Process(au.GetParameters().GetDescriptor(d.GetId()), d,
                               mm_coder_enabled);
    au.Set(d.GetId(), std::move(std::get<0>(enc)));
    au.GetStats().Add(std::get<1>(enc));
  }
  return au;
}

// -----------------------------------------------------------------------------

AccessUnit ReadDecoder::EntropyCodeAu(AccessUnit&& a,
                                      const bool mm_coder_enabled) const {
  return EntropyCodeAu(entropycoder_, std::move(a), mm_coder_enabled);
}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
