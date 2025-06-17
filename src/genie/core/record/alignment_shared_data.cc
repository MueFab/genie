/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/alignment_shared_data.h"

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::core::record {

// -----------------------------------------------------------------------------

AlignmentSharedData::AlignmentSharedData() : seq_id_(0), as_depth_(0) {}

// -----------------------------------------------------------------------------

AlignmentSharedData::AlignmentSharedData(const uint16_t seq_id,
                                         const uint8_t as_depth)
    : seq_id_(seq_id), as_depth_(as_depth) {}

// -----------------------------------------------------------------------------

void AlignmentSharedData::Write(util::BitWriter& writer) const {
  writer.WriteBypassBE(seq_id_);
  writer.WriteBypassBE(as_depth_);
}

// -----------------------------------------------------------------------------

AlignmentSharedData::AlignmentSharedData(util::BitReader& reader)
    : seq_id_(reader.ReadAlignedInt<uint16_t>()),
      as_depth_(reader.ReadAlignedInt<uint8_t>()) {}

// -----------------------------------------------------------------------------

uint16_t AlignmentSharedData::GetSeqId() const { return seq_id_; }

// -----------------------------------------------------------------------------

uint8_t AlignmentSharedData::GetAsDepth() const { return as_depth_; }

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
