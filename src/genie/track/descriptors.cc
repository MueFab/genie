/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/track/descriptors.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/arrayType.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace track {

void Descriptors::write(genie::core::record::track::Record trackRecord) {
    // Write descriptors: SEQUENCEID, STARTPOS, ENDPOS, STRAND
    tiles[genie::core::AnnotDesc::SEQUENCEID].write(trackRecord.GetSeqId(), 16);
    tiles[genie::core::AnnotDesc::STARTPOS].write(trackRecord.GetStartPos(), 64);
    tiles[genie::core::AnnotDesc::ENDPOS].write(trackRecord.GetEndPos(), 64);
    tiles[genie::core::AnnotDesc::STRAND].write(trackRecord.GetStrand(), 2);

    // Write link information if this is a linked record
    if (trackRecord.IsLinkedRecord()) {
        tiles[genie::core::AnnotDesc::LINKNAME].write(trackRecord.GetLinkName());
        tiles[genie::core::AnnotDesc::LINKID].write(trackRecord.GetReferenceBoxID(), 8);
    } else {
        tiles[genie::core::AnnotDesc::LINKNAME].emptyForRow();
        tiles[genie::core::AnnotDesc::LINKID].write((uint8_t)255, 8);
    }
}

void Descriptors::writeDanglingBits() {
    tiles[genie::core::AnnotDesc::SEQUENCEID].wrapUp();  // 1
    tiles[genie::core::AnnotDesc::STARTPOS].wrapUp();    // 2
    tiles[genie::core::AnnotDesc::ENDPOS].wrapUp();      // 3
    tiles[genie::core::AnnotDesc::STRAND].wrapUp();      // 4
    tiles[genie::core::AnnotDesc::LINKNAME].wrapUp();    // 7
    tiles[genie::core::AnnotDesc::LINKID].wrapUp();      // 8
}

void Descriptors::init() {
    tiles[genie::core::AnnotDesc::SEQUENCEID].setRowsPerTile(rowsPerTile);  // 1
    tiles[genie::core::AnnotDesc::STARTPOS].setRowsPerTile(rowsPerTile);    // 2
    tiles[genie::core::AnnotDesc::ENDPOS].setRowsPerTile(rowsPerTile);      // 3
    tiles[genie::core::AnnotDesc::STRAND].setRowsPerTile(rowsPerTile);      // 4
    tiles[genie::core::AnnotDesc::LINKNAME].setRowsPerTile(rowsPerTile);    // 7
    tiles[genie::core::AnnotDesc::LINKID].setRowsPerTile(rowsPerTile);      // 8
}

}  // namespace track
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
