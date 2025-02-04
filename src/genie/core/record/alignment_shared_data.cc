/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "alignment_shared_data.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace record {

// ---------------------------------------------------------------------------------------------------------------------

AlignmentSharedData::AlignmentSharedData() : seq_ID(0), as_depth(0) {}

// ---------------------------------------------------------------------------------------------------------------------

AlignmentSharedData::AlignmentSharedData(uint16_t _seq_ID, uint8_t _as_depth) : seq_ID(_seq_ID), as_depth(_as_depth) {}

// ---------------------------------------------------------------------------------------------------------------------

void AlignmentSharedData::write(util::BitWriter &writer) const {
    writer.writeBypassBE(seq_ID);
    writer.writeBypassBE(as_depth);
}

// ---------------------------------------------------------------------------------------------------------------------

AlignmentSharedData::AlignmentSharedData(util::BitReader &reader)
    : seq_ID(reader.ReadAlignedInt<uint16_t>()), as_depth(reader.ReadAlignedInt<uint8_t>()) {}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t AlignmentSharedData::getSeqID() const { return seq_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t AlignmentSharedData::getAsDepth() const { return as_depth; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace record
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
