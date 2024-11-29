/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/offset.h"

#include <string>

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------
Offset::Offset(util::BitReader& bit_reader) : sub_key_(4, '\0') {
  bit_reader.ReadAlignedBytes(sub_key_.data(), sub_key_.length());
  offset_ = bit_reader.ReadAlignedInt<uint64_t>();
}

// -----------------------------------------------------------------------------
void Offset::Write(util::BitWriter& writer) const {
  writer.WriteAlignedBytes("offs", 4);
  writer.WriteAlignedBytes(sub_key_.data(), 4);
  writer.WriteAlignedInt(offset_);
}

// -----------------------------------------------------------------------------
const std::string& Offset::GetSubKey() const { return sub_key_; }

// -----------------------------------------------------------------------------
uint64_t Offset::GetOffset() const { return offset_; }

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
