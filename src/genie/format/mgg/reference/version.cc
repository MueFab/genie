/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/mgg/reference/version.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgg::reference {

// -----------------------------------------------------------------------------
bool Version::operator==(const Version& other) const {
  return v_major_ == other.v_major_ && v_minor_ == other.v_minor_ &&
         v_patch_ == other.v_patch_;
}

// -----------------------------------------------------------------------------
Version::Version(const uint16_t v_major, const uint16_t v_minor,
                 const uint16_t v_patch)
    : v_major_(v_major), v_minor_(20), v_patch_(v_patch) {
  (void)v_minor;  // Ref software requires 20?
}

// -----------------------------------------------------------------------------
Version::Version(util::BitReader& reader) {
  v_major_ = reader.ReadAlignedInt<uint16_t>();
  v_minor_ = reader.ReadAlignedInt<uint16_t>();
  v_patch_ = reader.ReadAlignedInt<uint16_t>();
}

// -----------------------------------------------------------------------------
void Version::Write(util::BitWriter& writer) const {
  writer.WriteAlignedInt(v_major_);
  writer.WriteAlignedInt(v_minor_);
  writer.WriteAlignedInt(v_patch_);
}

// -----------------------------------------------------------------------------
uint16_t Version::GetMajor() const { return v_major_; }

// -----------------------------------------------------------------------------
uint16_t Version::GetMinor() const { return v_minor_; }

// -----------------------------------------------------------------------------
uint16_t Version::GetPatch() const { return v_patch_; }

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgg::reference

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
