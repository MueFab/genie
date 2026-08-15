/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/track/track_parser.h"

#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::track {

TrackParser::TrackParser(std::istream& _track_MGrecs, std::vector<genie::annotation::InfoField>& _fields,
                                     uint64_t _rowsPerTile)
    : trackMGrecs(_track_MGrecs),
      rowsPerTile(_rowsPerTile),
      numberOfRows(0),
      infoFields(_fields),
      fieldWriter{},
      numberOfAttributes(0) {
    init();
    descriptors.setTileSize(rowsPerTile);

    util::BitReader reader(trackMGrecs);
    while (fillRecord(reader)) {
        descriptors.write(trackRecord);
        attributes.add(trackRecord.GetFields());
        numberOfRows++;
    }
    descriptors.writeDanglingBits();
}

void TrackParser::init() {
    uint16_t attributeID = 0;
    if (!infoFields.empty()) {
        for (const auto& infoField : infoFields) {
            AttributeData attribute(static_cast<uint8_t>(infoField.ID.length()), infoField.ID, infoField.Type,
                infoField.Number, attributeID);
            attributeData[infoField.ID] = attribute;
            attributeID++;
        }

        variant_site::Attributes attr(rowsPerTile, attributeData);
        attributes = attr;

        numberOfAttributes = static_cast<uint16_t>(infoFields.size());
    }
}

bool TrackParser::fillRecord(util::BitReader reader) {
    if (!trackRecord.Read(reader)) return false;
    return true;
}

}  // namespace genie::track

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
