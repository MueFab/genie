/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/feature/feature_parser.h"
#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/arrayType.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace feature {

// FeatureParser::FeatureParser(std::istream& _feature_MGrecs, std::stringstream& _jsonInfoFields,
//                                     uint64_t _rowsPerTile)
//    : featureMGrecs(_feature_MGrecs),
//      rowsPerTile(_rowsPerTile),
//      numberOfRows(0),
//      fieldWriter{},
//      numberOfAttributes(0),
//      startPos(0) {
//    annotation::JsonAttributeParser InfoFieldParser(_jsonInfoFields);
//    infoFields = InfoFieldParser.getInfoFields();
//    init();
//    descriptors.setTileSize(rowsPerTile);
//
//    util::BitReader reader(featureMGrecs);
//    while (fillRecord(reader)) {
//        descriptors.write(feature);
//        attributes.add(feature.GetFeatureAttributes().GetFields());
//        numberOfRows++;
//    }
//    descriptors.writeDanglingBits();
//}

FeatureParser::FeatureParser(std::istream& _feature_MGrecs, std::vector<annotation::InfoField>& _fields,
                                     uint64_t _rowsPerTile)
    : featureMGrecs(_feature_MGrecs),
      rowsPerTile(_rowsPerTile),
      numberOfRows(0),
      infoFields(_fields),
      fieldWriter{},
      numberOfAttributes(0),
      startPos(0) {
    init();

    util::BitReader reader(featureMGrecs);
    while (fillRecord(reader)) {
        attributes.add(feature.GetFeatureAttributes().GetFields());
        numberOfRows++;
    }
}



void FeatureParser::init() {
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

        numberOfAttributes = (uint16_t)infoFields.size();
    } else {
        std::map<std::string, core::record::annotation_parameter_set::AttributeData> info;
            std::map<std::string, core::record::annotation_parameter_set::AttributeData> attributetags;
        for (auto tag : tags) {
            AttributeData tagdata(static_cast<uint8_t>(tag.first.length()), tag.first, tag.second.info_type, tag.second.info_array_len, attributeID);
            attributetags[tag.first] = tagdata;
            attributeID++;
        }
        variant_site::Attributes attr(rowsPerTile, attributetags);
        attributes = attr;
        numberOfAttributes = (uint16_t)attributeID;
    }
}

bool FeatureParser::fillRecord(util::BitReader reader) {
    if (!feature.Read(reader)) return false;
    // for (auto field : feature.getInfoTag()) tags[field.info_tag].infoValue = field.infoValue;
    return true;
}

}  // namespace feature
}  // namespace genie
