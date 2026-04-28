/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/annotation/trackproperty_annotation.h"

#include <codecs/include/mpegg-codecs.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#include "genie/core/record/annotation_access_unit/TypedData.h"
#include "genie/annotation/accessunit_composer.h"

#include "genie/core/array_type.h"
#include "genie/util/runtime_exception.h"

#include "genie/annotation/annotation_encoder.h"
#include "genie/annotation/parameterset_composer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

void TrackPropertyAnnotation::parseInfoTags(std::string& recordInputFileName) {
    std::ifstream readForTags;
    readForTags.open(recordInputFileName, std::ios::in | std::ios::binary);
    util::BitReader bitreader(readForTags);

    // Read all track property records to collect all possible info fields
    while (bitreader.IsStreamGood()) {
        core::record::track_property::Record rec;
        if (!rec.Read(bitreader)) {
            break;
        }

        // Store track_type from the first record to determine annotation subtype
        if (trackType_ == 0) {
            trackType_ = rec.GetTrackType();
        }

        const auto& props = rec.GetProperties();
        for (const auto& prop : props) {
            // Only add if not already present
            if (attributeInfo.find(prop.track_property) == attributeInfo.end()) {
                InfoField infoField(prop.track_property, static_cast<core::DataType>(prop.track_property_type),
                                   prop.track_property_array_len);
                attributeInfo[prop.track_property] = infoField;
            }
        }
    }
    readForTags.close();

    for (const auto& info : attributeInfo)
        infoFields.emplace_back(info.second.ID, info.second.Type, info.second.Number);
}

TrackPropertyUnits TrackPropertyAnnotation::parseTrackProperty(std::ifstream& inputfile) {
    track_property::TrackPropertyParser parser(inputfile, infoFields, defaultTileSizeHeight);
    uint8_t AG_class = 5;  // Track properties use AG class 5
    uint8_t AT_ID = 1;

    for (const auto& infoField : infoFields) attributeInfo[infoField.ID] = infoField;

    AnnotationEncoder encodingPars;
    encodingPars.setDescriptors(descrList);
    encodingPars.setCompressors(compressors);
    encodingPars.setAttributes(parser.getAttributes().getInfo());
    auto annotationEncodingParameters = encodingPars.Compose();
    ParameterSetComposer parameterset;

    annotationParameterSet =
        parameterset.Compose(AT_ID, AG_class, {defaultTileSizeHeight, 0}, annotationEncodingParameters);

    // Convert track_type to subtype
    // track_type values: GTF=2, GFF=3, BED=4, BEDGRAPH=5, WIG=6, BIGWIG=7, GENBANK=8
    uint8_t subtype = static_cast<uint8_t>(trackType_);

    variant_site::AccessUnitComposer accessUnit;
    // Track properties use TRACK_PROPERTY type with subtype from track_type
    accessUnit.setATtype(core::access_unit::annotation::AnnotationType::TRACKS,
                         subtype);
    accessUnit.setCompressors(compressors);
    annotationAccessUnit.resize(parser.getNrOfTiles());
    uint64_t rowIndex = 0;
    auto& descrStream = parser.getDescriptors().getTiles();

    std::map<std::string, core::access_unit::annotation::TypedData> attr;
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

    return TrackPropertyUnits{annotationParameterSet, annotationAccessUnit};
}

void TrackPropertyAnnotation::setInfoFields(std::string jsonFileName) {
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
