/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "meta-alignment.h"
#include <util/bitreader.h>
#include <util/bitwriter.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace format {
namespace mpegg_rec {

// ---------------------------------------------------------------------------------------------------------------------

MetaAlignment::MetaAlignment() : seq_ID(0), as_depth(0) {}

// ---------------------------------------------------------------------------------------------------------------------

MetaAlignment::MetaAlignment(uint16_t _seq_ID, uint8_t _as_depth) : seq_ID(_seq_ID), as_depth(_as_depth) {}

// ---------------------------------------------------------------------------------------------------------------------

void MetaAlignment::write(util::BitWriter &writer) const {
    writer.write(seq_ID, 16);
    writer.write(as_depth, 8);
}

// ---------------------------------------------------------------------------------------------------------------------

MetaAlignment::MetaAlignment(util::BitReader &reader) {
    seq_ID = reader.read<uint16_t>();
    as_depth = reader.read<uint8_t>();
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t MetaAlignment::getSeqID() const { return seq_ID; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t MetaAlignment::getAsDepth() const { return as_depth; }

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_rec
}  // namespace format

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------