/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/trackproperty/descriptors.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/array_type.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace track_property {

void Descriptors::write(genie::core::record::track_property::Record trackPropertyRecord) {
    // Track properties only store LINKNAME and LINKID as descriptors
    // Properties themselves are stored as attributes
    // Write link information if this is a linked record
    if (trackPropertyRecord.IsLinkedRecord()) {
        tiles[genie::core::AnnotDesc::LINKNAME].write(trackPropertyRecord.GetLinkName());
        tiles[genie::core::AnnotDesc::LINKID].write(trackPropertyRecord.GetReferenceBoxID(), 8);
    } else {
        tiles[genie::core::AnnotDesc::LINKNAME].emptyForRow();
        tiles[genie::core::AnnotDesc::LINKID].write((uint8_t)255, 8);
    }
}

void Descriptors::writeDanglingBits() {
    tiles[genie::core::AnnotDesc::LINKNAME].wrapUp();    // 7
    tiles[genie::core::AnnotDesc::LINKID].wrapUp();      // 8
}

void Descriptors::init() {
    tiles[genie::core::AnnotDesc::LINKNAME].setRowsPerTile(rowsPerTile);    // 7
    tiles[genie::core::AnnotDesc::LINKID].setRowsPerTile(rowsPerTile);      // 8
}

}  // namespace track_property
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
