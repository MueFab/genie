/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/annotation/functional_annotation.h"

#include <codecs/include/mpegg-codecs.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#include "genie/core/record/annotation_access_unit/TypedData.h"
#include "genie/variantsite/accessunit_composer.h"

#include "genie/core/arrayType.h"
#include "genie/util/runtime_exception.h"

#include "genie/annotation/annotation_encoder.h"
#include "genie/annotation/parameterset_composer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

void FunctionalAnnotation::parseInfoTags(std::string& recordInputFileName) {
    std::ifstream readForTags;
    readForTags.open(recordInputFileName, std::ios::in | std::ios::binary);
    util::BitReader bitreader(readForTags);
    core::record::functional_annotation::Record recs;
    std::set<std::string> uniqueFeatureNames;
    std::set<std::string> uniqueOntologyNames;
    maxOntologiesPerRecord = 0;
    
    while (recs.Read(bitreader)) {
        const auto& attrs = recs.GetAttributes();
        for (const auto& attr : attrs) {
            InfoField infoField(attr.attr_tag, static_cast<core::DataType>(attr.attr_type), 1);
            attributeInfo[attr.attr_tag] = infoField;
        }
        // Collect unique feature names
        if (!recs.GetFeatureName().empty()) {
            uniqueFeatureNames.insert(recs.GetFeatureName());
        }
        // Collect unique ontology names and track maximum count per record
        const auto& ontologies = recs.GetOntologies();
        if (ontologies.size() > maxOntologiesPerRecord) {
            maxOntologiesPerRecord = static_cast<uint8_t>(ontologies.size());
        }
        for (const auto& ontology : ontologies) {
            if (!ontology.ontology_name.empty()) {
                uniqueOntologyNames.insert(ontology.ontology_name);
            }
        }
    }
    readForTags.close();
    for (const auto& info : attributeInfo)
        infoFields.emplace_back(info.second.ID, info.second.Type, info.second.Number);
    // Convert sets to vectors
    featureNames.assign(uniqueFeatureNames.begin(), uniqueFeatureNames.end());
    ontologyNames.assign(uniqueOntologyNames.begin(), uniqueOntologyNames.end());
}

FunctionalAnnotationUnits FunctionalAnnotation::parseFunctionalAnnotation(std::ifstream& inputfile) {
    functional_annotation::FunctionalAnnotationParser parser(inputfile, infoFields, featureNames, ontologyNames, maxOntologiesPerRecord, defaultTileSizeHeight);
    uint8_t AG_class = 1;
    uint8_t AT_ID = 1;

    for (const auto& infoField : infoFields) attributeInfo[infoField.ID] = infoField;

    AnnotationEncoder encodingPars;
    encodingPars.setDescriptors(descrList);
    encodingPars.setCompressors(compressors);
    encodingPars.setAttributes(parser.getAttributes().getInfo());
    encodingPars.setFeatures(featureNames);
    encodingPars.setOntologyTerms(ontologyNames);
    auto annotationEncodingParameters = encodingPars.Compose();
    ParameterSetComposer parameterset;

    annotationParameterSet =
        parameterset.Compose(AT_ID, AG_class, {defaultTileSizeHeight, 0}, annotationEncodingParameters);

    variant_site::AccessUnitComposer accessUnit;
    accessUnit.setATtype(core::record::annotation_access_unit::AnnotationType::FUNCTIONAL_ANNOTATIONS,
                         core::record::annotation_access_unit::AnnotationSubtype::GFF);
    accessUnit.setCompressors(compressors);
    annotationAccessUnit.resize(parser.getNrOfTiles());
    uint64_t rowIndex = 0;
    auto& descrStream = parser.getDescriptors().getTiles();

    std::map<std::string, core::record::annotation_access_unit::TypedData> attr;
    for (uint64_t i = 0; i < parser.getNrOfTiles(); ++i) {
        std::map<core::AnnotDesc, std::stringstream> desc;
        for (auto& desctile : descrStream) {
            desc[desctile.first] << desctile.second.getTile(i).rdbuf();
        }
        for (auto& attrtile : parser.getAttributes().getTiles()) {
            attr[attrtile.first] = attrtile.second.getTypedTile(i);
        }

        accessUnit.setAccessUnit(desc, attr, parser.getAttributes().getInfo(), annotationParameterSet,
                                 annotationAccessUnit.at(i), AG_class, AT_ID, rowIndex);
        rowIndex++;
    }

    return FunctionalAnnotationUnits{annotationParameterSet, annotationAccessUnit};
}

void FunctionalAnnotation::setInfoFields(std::string jsonFileName) {
    // read attributes info from json file
    std::ifstream AttributeFieldsFile;
    AttributeFieldsFile.open(jsonFileName, std::ios::in);
    std::stringstream attributeFields;
    UTILS_DIE_IF(!AttributeFieldsFile.is_open(), "unable to open json file");

    if (AttributeFieldsFile.is_open()) {
        attributeFields << AttributeFieldsFile.rdbuf();
        AttributeFieldsFile.close();
    }
    JsonAttributeParser attributeParser(attributeFields);
    for (auto field : attributeParser.getInfoFields()) {
        attributeInfo[field.ID] = field;
    }
    infoFields = attributeParser.getInfoFields();
}

}  // namespace annotation
}  // namespace genie
