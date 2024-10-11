/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/record/alignment-shared-data.h"
#include "genie/util/bit-reader.h"
#include "genie/util/bit-writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

// ---------------------------------------------------------------------------------------------------------------------

AlignmentSharedData::AlignmentSharedData() : seq_ID(0), as_depth(0) {}

// ---------------------------------------------------------------------------------------------------------------------

AlignmentSharedData::AlignmentSharedData(const uint16_t _seq_ID, const uint8_t _as_depth)
    : seq_ID(_seq_ID), as_depth(_as_depth) {}

// ---------------------------------------------------------------------------------------------------------------------

void AlignmentSharedData::write(util::BitWriter &writer) const {
    writer.writeAlignedInt(seq_ID);
    writer.writeAlignedInt(as_depth);
}

// ---------------------------------------------------------------------------------------------------------------------

AlignmentSharedData::AlignmentSharedData(util::BitReader &reader)
    : seq_ID(reader.readAlignedInt<uint16_t>()), as_depth(reader.readAlignedInt<uint8_t>()) {}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t AlignmentSharedData::getSeqID() const { return seq_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t AlignmentSharedData::getAsDepth() const { return as_depth; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
