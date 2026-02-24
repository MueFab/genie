/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/annotation/feature_annotation.h"

#include <codecs/include/mpegg-codecs.h>

// #include <chrono>
#include <map>
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

void FeatureAnnotation::parseInfoTags(std::string& recordInputFileName) {
    std::ifstream readForTags;
    readForTags.open(recordInputFileName, std::ios::in | std::ios::binary);
    util::BitReader bitreader(readForTags);
    std::vector<core::record::feature::FeatureFields::Field> infoTag;
    core::record::feature::Record recs;
    while (recs.Read(bitreader)) {
        infoTag = recs.GetFeatureAttributes().GetFields();
        for (const auto& tag : infoTag) {
            InfoField infoField(tag.attr, tag.attr_type, static_cast<uint8_t>(tag.attr_values.size()));
            core::record::feature::Info_tag infotag{static_cast<uint8_t>(tag.attr.size()), tag.attr, tag.attr_type,
                                                    static_cast<uint8_t>(tag.attr_values.size()), tag.attr_values};
            infoTags[tag.attr] = infotag;
            attributeInfo[tag.attr] = infoField;
        }
    }
    readForTags.close();
    for (const auto& info : infoTags)
        infoFields.emplace_back(info.second.info_tag, info.second.info_type, info.second.info_array_len);
}

FeatureUnits FeatureAnnotation::parseFeature(std::ifstream& inputfile) {
    feature::FeatureParser parser(inputfile, infoFields, defaultTileSizeHeight);
    uint8_t AG_class = 1;
    uint8_t AT_ID = 1;

    for (const auto& infoField : infoFields) attributeInfo[infoField.ID] = infoField;

    AnnotationEncoder encodingPars;
    entropy::bsc::BSCParameters bscParameters;
    auto BSCalgorithmParameters = bscParameters.convertToAlgorithmParameters();

    encodingPars.setDescriptorParameters(core::AnnotDesc::LINKID, core::AlgoID::BSC,
                                         BSCalgorithmParameters);
    encodingPars.setCompressors(compressors);
    encodingPars.setAttributes(parser.getAttributes().getInfo());
    auto annotationEncodingParameters = encodingPars.Compose();
    ParameterSetComposer parameterset;

    annotationParameterSet =
        parameterset.Compose(AT_ID, AG_class, {defaultTileSizeHeight, 0}, annotationEncodingParameters);

    variant_site::ParameterSetComposer encodeParameters;

    variant_site::AccessUnitComposer accessUnit;
    accessUnit.setATtype(core::record::annotation_access_unit::AnnotationType::GENIE_EXPRESSION, 9);
    accessUnit.setCompressors(compressors);
    annotationAccessUnit.resize(parser.getNrOfTiles());
    uint64_t rowIndex = 0;

    std::map<std::string, core::record::annotation_access_unit::TypedData> attr;
    for (uint64_t i = 0; i < parser.getNrOfTiles(); ++i) {
        std::map<core::AnnotDesc, std::stringstream> desc;
        for (auto& attrtile : parser.getAttributes().getTiles()) {
            attr[attrtile.first] = attrtile.second.getTypedTile(i);
        }

        if (i == 0) {
            // add LINK_ID default values
            std::cerr << " add link values... " << std::endl;
            for (auto j = 0u; j < defaultTileSizeHeight; ++j) {
                const char val = '\xFF';
                desc[core::AnnotDesc::LINKID].write(&val, 1);
            }
        }

        accessUnit.setAccessUnit(desc, attr, parser.getAttributes().getInfo(), annotationParameterSet,
                                 annotationAccessUnit.at(i), AG_class, AT_ID, rowIndex);
        rowIndex++;
    }

    return FeatureUnits{annotationParameterSet, annotationAccessUnit};
}

void FeatureAnnotation::setInfoFields(std::string jsonFileName) {
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
