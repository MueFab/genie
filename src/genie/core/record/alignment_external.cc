/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/alignment_external.h"

#include <memory>

#include "genie/core/record/alignment_external/none.h"
#include "genie/core/record/alignment_external/other_rec.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::core::record {

// -----------------------------------------------------------------------------
AlignmentExternal::AlignmentExternal(const Type more_alignment_info_type)
    : more_alignment_info_type_(more_alignment_info_type) {}

// -----------------------------------------------------------------------------
void AlignmentExternal::Write(util::BitWriter& writer) const {
  writer.WriteAlignedInt(more_alignment_info_type_);
}

// -----------------------------------------------------------------------------
std::unique_ptr<AlignmentExternal> AlignmentExternal::Factory(
    util::BitReader& reader) {
  switch (reader.ReadAlignedInt<Type>()) {
    case Type::kNone:
      return std::make_unique<alignment_external::None>();
    case Type::kOtherRec:
      return std::make_unique<alignment_external::OtherRec>(reader);
    default:
      UTILS_DIE("Unknown MoreAlignmentInfoType");
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
