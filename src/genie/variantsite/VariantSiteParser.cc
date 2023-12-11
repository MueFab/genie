#include "VariantSiteParser.h"
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>

#include "genie/core/arrayType.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
#include "genie/variantsite/VariantSiteParser.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {

VariantSiteParser::VariantSiteParser(std::istream& _site_MGrecs, std::stringstream& _jsonInfoFields,
                                     uint64_t _rowsPerTile)
    : siteMGrecs(_site_MGrecs),
      rowsPerTile(_rowsPerTile),
      numberOfRows(0),
      fieldWriter{},
      numberOfAttributes(0),
      startPos(0) {
    JsonInfoFieldParser InfoFieldParser(_jsonInfoFields);
    infoFields = InfoFieldParser.getInfoFields();
    init();
    descriptors.setTileSize(rowsPerTile);

    util::BitReader reader(siteMGrecs);
    while (fillRecord(reader)) {
        descriptors.write(variantSite);
        attributes.add(variantSite.getInfoTag());
        numberOfRows++;
    }
    descriptors.writeDanglingBits();
}

void VariantSiteParser::init() {
    uint16_t attributeID = 0;
    for (const auto& infoField : infoFields) {
        AttributeData attribute(static_cast<uint8_t>(infoField.ID.length()), infoField.ID, infoField.Type,
                                infoField.Number, attributeID);
        attributeData[infoField.ID] = attribute;
        attributeID++;
    }

    Attributes attr(rowsPerTile, attributeData);
    attributes = attr;

    numberOfAttributes = (uint16_t)infoFields.size();
}

bool VariantSiteParser::fillRecord(util::BitReader reader) {
    if (!variantSite.read(reader)) return false;
    return true;
}

}  // namespace variant_site
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
