/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/gen_info.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg {

// -----------------------------------------------------------------------------
bool GenInfo::operator==(const GenInfo& info) const {
  return GetKey() == info.GetKey();
}

// -----------------------------------------------------------------------------
uint64_t GenInfo::GetHeaderLength() {
  return sizeof(uint64_t) + sizeof(char) * 4;
}

// -----------------------------------------------------------------------------
void GenInfo::Write(util::BitWriter& bit_writer) const {
  const int64_t begin = bit_writer.GetStreamPosition();
  bit_writer.WriteAlignedBytes(GetKey().data(), GetKey().length());
  const int64_t size_pos = bit_writer.GetStreamPosition();
  bit_writer.WriteAlignedInt<uint64_t>(0);
  BoxWrite(bit_writer);
  const int64_t end = bit_writer.GetStreamPosition();
  bit_writer.SetStreamPosition(size_pos);
  bit_writer.WriteAlignedInt<uint64_t>(end - begin);
  bit_writer.SetStreamPosition(end);
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
