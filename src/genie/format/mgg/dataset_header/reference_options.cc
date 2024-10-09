/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mgg/dataset_header/reference_options.h"
#include <limits>
#include <vector>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::mgg::dataset_header {

// ---------------------------------------------------------------------------------------------------------------------

bool ReferenceOptions::operator==(const ReferenceOptions& other) const {
    return reference_ID == other.reference_ID && seq_ID == other.seq_ID && seq_blocks == other.seq_blocks;
}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceOptions::ReferenceOptions(genie::util::BitReader& reader) {
    auto seq_count = reader.read<uint16_t>();
    if (!seq_count) {
        reference_ID = std::numeric_limits<uint8_t>::max();
        return;
    }
    reference_ID = reader.read<uint8_t>();
    for (uint16_t i = 0; i < seq_count; ++i) {
        seq_ID.emplace_back(reader.read<uint16_t>());
    }
    for (uint16_t i = 0; i < seq_count; ++i) {
        seq_blocks.emplace_back(reader.read<uint32_t>());
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceOptions::write(genie::util::BitWriter& writer) const {
    writer.writeBits(seq_ID.size(), 16);
    if (seq_ID.empty()) {
        return;
    }
    writer.writeBits(reference_ID, 8);

    for (auto& i : seq_ID) {
        writer.writeBits(i, 16);
    }

    for (auto& b : seq_blocks) {
        writer.writeBits(b, 32);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

ReferenceOptions::ReferenceOptions() : reference_ID(std::numeric_limits<uint8_t>::max()) {}

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceOptions::addSeq(uint8_t _reference_ID, uint16_t _seq_id, uint32_t blocks) {
    UTILS_DIE_IF(_reference_ID != reference_ID && !seq_ID.empty(), "Unmatching ref id");
    reference_ID = _reference_ID;
    seq_ID.push_back(_seq_id);
    seq_blocks.push_back(blocks);
}

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint16_t>& ReferenceOptions::getSeqIDs() const { return seq_ID; }

// ---------------------------------------------------------------------------------------------------------------------

const std::vector<uint32_t>& ReferenceOptions::getSeqBlocks() const { return seq_blocks; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t ReferenceOptions::getReferenceID() const { return reference_ID; }

// ---------------------------------------------------------------------------------------------------------------------

void ReferenceOptions::patchRefID(uint8_t _old, uint8_t _new) {
    if (reference_ID == _old || reference_ID == 255) {
        reference_ID = _new;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::mgg::dataset_header

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
