/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include <chrono>

#include <codecs/include/mpegg-codecs.h>

#include "genie/core/record/annotation_access_unit/TypedData.h"
#include "genie/variantsite/AccessUnitComposer.h"

#include "genie/core/arrayType.h"
#include "genie/util/runtime-exception.h"
#include "site_annotation.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

void SiteAnnotation::parseInfoTags(std::string& recordInputFileName) {
    std::ifstream readForTags;
    readForTags.open(recordInputFileName, std::ios::in | std::ios::binary);
    genie::util::BitReader bitreader(readForTags);
    std::vector<genie::core::record::variant_site::InfoFields::Field> infoTag;
    genie::core::record::variant_site::Record recs;
    while (recs.read(bitreader)) {
        infoTag = recs.getInfoTag();
        for (const auto& tag : infoTag) {
            InfoField infoField(tag.tag, tag.type, static_cast<uint8_t>(tag.values.size()));
            genie::core::record::variant_site::Info_tag infotag{static_cast<uint8_t>(tag.tag.size()), tag.tag, tag.type,
                                                                static_cast<uint8_t>(tag.values.size()), tag.values};
            infoTags[tag.tag] = infotag;
            attributeInfo[tag.tag] = infoField;
        }
    }
    readForTags.close();
    for (const auto& info : infoTags)
        infoFields.emplace_back(info.second.info_tag, info.second.info_type, info.second.info_array_len);
}

SiteUnits SiteAnnotation::parseSite(std::ifstream& inputfile) {
    genie::variant_site::VariantSiteParser parser(inputfile, infoFields, defaultTileSizeHeight);
    uint8_t AG_class = 1;
    uint8_t AT_ID = 1;

    for (const auto& infoField : infoFields) attributeInfo[infoField.ID] = infoField;

    genie::variant_site::ParameterSetComposer encodeParameters;
    annotationParameterSet =
        encodeParameters.setParameterSet(descrList, parser.getAttributes().getInfo(),
                                         compressors.getCompressorParameters(), defaultTileSizeHeight, AT_ID);

    genie::variant_site::AccessUnitComposer accessUnit;
    accessUnit.setCompressors(compressors);
    annotationAccessUnit.resize(parser.getNrOfTiles());
    uint64_t rowIndex = 0;

    auto& descrStream = parser.getDescriptors().getTiles();
    std::map<std::string, genie::core::record::annotation_access_unit::TypedData> attr;
    for (auto& tile : descrStream) descrList.push_back(tile.first);

    for (uint64_t i = 0; i < parser.getNrOfTiles(); ++i) {
        std::map<genie::core::AnnotDesc, std::stringstream> desc;
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

    return SiteUnits{annotationParameterSet, annotationAccessUnit};
}

void SiteAnnotation::setInfoFields(std::string jsonFileName) {
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
