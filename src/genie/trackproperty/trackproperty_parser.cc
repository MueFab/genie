/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/trackproperty/trackproperty_parser.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace track_property {

// ---------------------------------------------------------------------------------------------------------------------

TrackPropertyParser::TrackPropertyParser(std::ifstream& inputfile, std::vector<annotation::InfoField> infofields,
                                         uint64_t _defaultTileSizeHeight)
    : trackPropertyMGrecs(inputfile),
      rowsPerTile(_defaultTileSizeHeight),
      numberOfRows(0),
      infoFields(infofields),
      descriptors(_defaultTileSizeHeight) {
    init();
    descriptors.init();

    util::BitReader reader(trackPropertyMGrecs);
    while (fillRecord(reader)) {
        descriptors.write(trackPropertyRecord);
        
        // Track properties are stored as attributes in the annotation parameter set
        const auto& props = trackPropertyRecord.GetProperties();
        if (!props.empty()) {
            attributes.add(props);
        }
        numberOfRows++;
    }
    
    descriptors.writeDanglingBits();
    numberOfTiles = descriptors.getTiles()[core::AnnotDesc::LINKNAME].getNrOfTiles();
}

void TrackPropertyParser::init() {
    // Initialize attributes if needed based on infoFields
    // This can be expanded if attributes need initialization similar to other parsers
}

bool TrackPropertyParser::fillRecord(util::BitReader reader) {
    if (!trackPropertyRecord.Read(reader)) return false;
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace track_property
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
