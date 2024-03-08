/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "VariantSiteParser.h"
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
    genie::annotation::JsonAttributeParser InfoFieldParser(_jsonInfoFields);
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

VariantSiteParser::VariantSiteParser(std::istream& _site_MGrecs, std::vector<genie::annotation::InfoField>& _fields,
                                     uint64_t _rowsPerTile)
    : siteMGrecs(_site_MGrecs),
      rowsPerTile(_rowsPerTile),
      numberOfRows(0),
      infoFields(_fields),
      fieldWriter{},
      numberOfAttributes(0),
      startPos(0) {
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
VariantSiteParser::VariantSiteParser(std::istream& _site_MGrecs,
                                     std::map<std::string, genie::core::record::variant_site::Info_tag>& _fields,
                                     uint64_t _rowsPerTile)
    : siteMGrecs(_site_MGrecs),
      rowsPerTile(_rowsPerTile),
      numberOfRows(0),
      tags(_fields),
      fieldWriter{},
      numberOfAttributes(0),
      startPos(0) {
    init();
    descriptors.setTileSize(rowsPerTile);

    util::BitReader reader(siteMGrecs);
    while (fillRecord(reader)) {
        descriptors.write(variantSite);
        numberOfRows++;
        attributes.add(tags);
    }
    descriptors.writeDanglingBits();
}

void VariantSiteParser::init() {
    uint16_t attributeID = 0;
    if (!infoFields.empty()) {
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
    else {
        std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData> info;
            std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData> attributetags;
        for (auto tag : tags) {
            AttributeData tagdata(static_cast<uint8_t>(tag.first.length()), tag.first, tag.second.info_type, tag.second.info_array_len, attributeID);
            attributetags [tag.first] = tagdata;
            attributeID++;
        }
        Attributes attr(rowsPerTile, attributetags);
        attributes = attr;
        numberOfAttributes = (uint16_t)attributeID;

    }
}

bool VariantSiteParser::fillRecord(util::BitReader reader) {
    if (!variantSite.read(reader)) return false;
    for (auto field : variantSite.getInfoTag()) tags[field.info_tag].infoValue = field.infoValue;
    return true;
}

}  // namespace variant_site
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
