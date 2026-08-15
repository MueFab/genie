/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/functionalannotation/functionalannotation_parser.h"

#include <map>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::functional_annotation {

FunctionalAnnotationParser::FunctionalAnnotationParser(std::istream& _annotation_MGrecs, std::vector<genie::annotation::InfoField>& _fields,
                                     const std::vector<std::string>& _featureNames,
                                     const std::vector<std::string>& _ontologyNames,
                                     uint8_t _maxOntologiesPerRecord,
                                     uint64_t _rowsPerTile)
    : annotationMGrecs(_annotation_MGrecs),
      rowsPerTile(_rowsPerTile),
      numberOfRows(0),
      infoFields(_fields),
      fieldWriter{},
      numberOfAttributes(0) {
    init();

    // Build feature name to index mapping
    std::map<std::string, uint32_t> featureNameMapping;
    for (size_t i = 0; i < _featureNames.size(); ++i) {
        featureNameMapping[_featureNames[i]] = static_cast<uint32_t>(i);
    }
    descriptors.setFeatureNameMapping(featureNameMapping);

    // Build ontology name to index mapping
    std::map<std::string, uint32_t> ontologyNameMapping;
    for (size_t i = 0; i < _ontologyNames.size(); ++i) {
        ontologyNameMapping[_ontologyNames[i]] = static_cast<uint32_t>(i);
    }
    descriptors.setOntologyNameMapping(ontologyNameMapping);
    descriptors.setMaxOntologiesPerRecord(_maxOntologiesPerRecord);

    descriptors.setTileSize(rowsPerTile);

    util::BitReader reader(annotationMGrecs);
    while (fillRecord(reader)) {
        descriptors.write(functionalAnnotation);
        attributes.add(functionalAnnotation.GetFields());
        numberOfRows++;
    }
    descriptors.writeDanglingBits();
}

void FunctionalAnnotationParser::init() {
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

bool FunctionalAnnotationParser::fillRecord(util::BitReader reader) {
    if (!functionalAnnotation.Read(reader)) return false;
    return true;
}

}  // namespace genie::functional_annotation

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
