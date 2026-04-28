/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/trackproperty/trackproperty_parser.h"

#include <algorithm>
#include <map>
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
      infoFields(std::move(infofields)),
      numberOfAttributes(0) {
    init();
    descriptors.setTileSize(rowsPerTile);

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
    uint16_t attributeID = 0;

    if (!infoFields.empty()) {
        std::map<std::string, genie::core::record::parameter::annotation::AttributeData> attributeData;

        for (const auto& infoField : infoFields) {
            AttributeData attribute(
                static_cast<uint8_t>(infoField.ID.length()),
                infoField.ID,
                infoField.Type,
                infoField.Number,
                attributeID);
            attributeData[infoField.ID] = attribute;
            attributeID++;
        }

        variant_site::Attributes attr(rowsPerTile, attributeData);
        attributes = attr;
        numberOfAttributes = static_cast<uint16_t>(infoFields.size());
    }
}

bool TrackPropertyParser::fillRecord(util::BitReader reader) {
    return trackPropertyRecord.Read(reader);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace track_property
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
