/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/parameter/descriptor_present/decoder.h"

// -----------------------------------------------------------------------------

namespace genie::core::parameter::desc_pres {

// -----------------------------------------------------------------------------

void Decoder::Write(util::BitWriter& writer) const {
  writer.WriteBits(encoding_mode_id_, 8);
}

// -----------------------------------------------------------------------------

uint8_t Decoder::GetMode() const { return encoding_mode_id_; }

// -----------------------------------------------------------------------------

Decoder::Decoder(const uint8_t encoding_mode_id)
    : encoding_mode_id_(encoding_mode_id) {}

// -----------------------------------------------------------------------------

bool Decoder::Equals(const Decoder* dec) const {
  return encoding_mode_id_ == dec->encoding_mode_id_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::parameter::desc_pres

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
