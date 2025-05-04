/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/alignment_split.h"

#include <memory>

#include "genie/core/record/alignment_split/other_rec.h"
#include "genie/core/record/alignment_split/same_rec.h"
#include "genie/core/record/alignment_split/unpaired.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::core::record {

// -----------------------------------------------------------------------------

AlignmentSplit::AlignmentSplit(const Type split_alignment)
    : split_alignment_(split_alignment) {}

// -----------------------------------------------------------------------------

void AlignmentSplit::Write(util::BitWriter& writer) const {
  writer.WriteAlignedInt(split_alignment_);
}

// -----------------------------------------------------------------------------

std::unique_ptr<AlignmentSplit> AlignmentSplit::Factory(
    const uint8_t as_depth, const bool extended_alignment,
    util::BitReader& reader) {
  switch (reader.ReadAlignedInt<Type>()) {
    case Type::kSameRec:
      return std::make_unique<alignment_split::SameRec>(
          as_depth, extended_alignment, reader);
    case Type::kOtherRec:
      return std::make_unique<alignment_split::OtherRec>(extended_alignment,
                                                         reader);
    case Type::kUnpaired:
      return std::make_unique<alignment_split::Unpaired>();
    default:
      UTILS_DIE("Invalid SplitAlignmentType");
  }
}

// -----------------------------------------------------------------------------

AlignmentSplit::Type AlignmentSplit::GetType() const {
  return split_alignment_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
