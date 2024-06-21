/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "annotation.h"
#include <codecs/include/mpegg-codecs.h>
#include "genie/util/runtime-exception.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

void genie::annotation::Annotation::startStream(RecType recType, std::string recordInputFileName,
                                                std::string outputFileName)

{
    std::ifstream inputfile;
    inputfile.open(recordInputFileName, std::ios::in | std::ios::binary);

    if (!inputfile.is_open())  // failed
        return;

    // read file - determine type?
    if (recType == RecType::GENO_FILE) {
        parseGenotype(inputfile);

    } else {
        parseInfoTags(recordInputFileName);
        parseSite(inputfile);
    }
    if (inputfile.is_open()) inputfile.close();

    writeToFile(outputFileName);
}

void Annotation::writeToFile(std::string& outputFileName) {
    std::ofstream testfile;
    std::string filename = outputFileName;
    testfile.open(filename + ".bin", std::ios::binary | std::ios::out);
    genie::core::Writer testwriter(&testfile);
    std::ofstream txtfile;
    txtfile.open(filename + ".txt", std::ios::out);
    genie::core::Writer txtwriter(&txtfile, true);

    genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);
    auto sizeSofar = APS_dataUnit.write(testwriter);

    APS_dataUnit.write(txtwriter, sizeSofar);

    for (auto& aau : annotationAccessUnit) {
        genie::core::record::data_unit::Record AAU_dataUnit(aau);
        sizeSofar = AAU_dataUnit.write(testwriter);
        AAU_dataUnit.write(txtwriter, sizeSofar);
    }
    testfile.close();
    txtfile.close();
}

void Annotation::parseInfoTags(std::string& recordInputFileName) {
    std::ifstream readForTags;
    readForTags.open(recordInputFileName, std::ios::in | std::ios::binary);
    genie::util::BitReader bitreader(readForTags);
    std::vector<genie::core::record::variant_site::InfoFields::Field> infoTag;
    genie::core::record::variant_site::Record recs;
    while (recs.read(bitreader)) {
        infoTag = recs.getInfoTag();
        for (auto tag : infoTag) {
            InfoField infoField(tag.tag, tag.type, static_cast<uint8_t>(tag.values.size()));
            genie::core::record::variant_site::Info_tag infotag{static_cast<uint8_t>(tag.tag.size()), tag.tag, tag.type,
                                                                static_cast<uint8_t>(tag.values.size()), tag.values};
            infoTags[tag.tag] = infotag;
            attributeInfo[tag.tag] = infoField;
        }
    }
    readForTags.close();
    for (auto info : infoTags)
        infoFields.emplace_back(info.second.info_tag, info.second.info_type, info.second.info_array_len);
}

void Annotation::parseGenotype(std::ifstream& inputfile) {
    const uint32_t defaultTileSize = 1000;
    genotype_opt.block_size = defaultTileSize;
    likelihood_opt.block_size = defaultTileSize;
    uint8_t AT_ID = 1;
    uint8_t AG_class = 0;

    std::vector<genie::core::record::VariantGenotype> recs;
    genie::util::BitReader bitreader(inputfile);
    std::vector<genie::genotype::EncodingBlock> genotypeDatablock;
    genie::genotype::GenotypeParameters genotypeParameters;
    genie::likelihood::LikelihoodParameters likelihoodParameters;
    std::vector<genie::likelihood::EncodingBlock> likelihoodDatablock;
    bool start = true;
    while (bitreader.isGood()) {
        uint32_t rowCount = 0;
        while (bitreader.isGood() && rowCount < defaultTileSize) {
            recs.emplace_back(bitreader);
            rowCount++;
        }
        if (!bitreader.isGood()) recs.pop_back();

        std::tuple<genie::genotype::GenotypeParameters, genie::genotype::EncodingBlock> genotypeData =
            genie::genotype::encode_block(genotype_opt, recs);
        genotypeDatablock.push_back(std::get<genie::genotype::EncodingBlock>(genotypeData));
        std::tuple<genie::likelihood::LikelihoodParameters, genie::likelihood::EncodingBlock> likelihoodData =
            genie::likelihood::encode_block(likelihood_opt, recs);
        likelihoodDatablock.push_back(std::get<genie::likelihood::EncodingBlock>(likelihoodData));
        if (start) {
            genotypeParameters = std::get<genie::genotype::GenotypeParameters>(genotypeData);
            likelihoodParameters = std::get<genie::likelihood::LikelihoodParameters>(likelihoodData);
            start = false;
        }
    }

    //--------------------------------------------------

    genie::genotype::ParameterSetComposer parameterSetComposer;
    parameterSetComposer.setGenotypeParameters(genotypeParameters);
    parameterSetComposer.setLikelihoodParameters(likelihoodParameters);
    parameterSetComposer.setCompressors(compressors);
    annotationParameterSet = parameterSetComposer.Build(AT_ID, genotypeDatablock.at(0).attributeInfo, recs.size());

    annotationAccessUnit.resize(genotypeDatablock.size());
    for (auto i = 0u; i < genotypeDatablock.size();++i)
    {
        std::map<std::string, genie::core::record::annotation_access_unit::TypedData> attributeTDStream;

        for (auto formatdata : genotypeDatablock.at(i).attributeData) {
            auto& info = genotypeDatablock.at(i).attributeInfo[formatdata.first];
            std::vector<uint32_t> arrayDims;
            arrayDims.push_back(std::min(likelihood_opt.block_size, static_cast<uint32_t>(recs.size())));
            arrayDims.push_back(recs.at(i).getNumSamples());
            arrayDims.push_back(recs.at(i).getFormatCount());
            attributeTDStream[formatdata.first].set(info.getAttributeType(), static_cast<uint8_t>(arrayDims.size()),
                                                    arrayDims);
            attributeTDStream[formatdata.first].convertToTypedData(formatdata.second);
        }

        std::map<genie::core::AnnotDesc, std::stringstream> descriptorStream;
        descriptorStream[genie::core::AnnotDesc::GENOTYPE];
        {
            genie::genotype::GenotypePayload genotypePayload(genotypeDatablock.at(i), genotypeParameters);
            genie::core::Writer writer(&descriptorStream[genie::core::AnnotDesc::GENOTYPE]);
            genotypePayload.write(writer);
        }

        if (likelihoodDatablock.at(i).nrows > 0 && likelihoodDatablock.at(i).ncols > 0) {
            descriptorStream[genie::core::AnnotDesc::LIKELIHOOD];
            genie::likelihood::LikelihoodPayload likelihoodPayload(likelihoodParameters, likelihoodDatablock.at(i));
            genie::core::Writer writer(&descriptorStream[genie::core::AnnotDesc::LIKELIHOOD]);
            likelihoodPayload.write(writer);
        }

        // add LINK_ID default values
        for (auto j = 0u; i < genotype_opt.block_size && j < recs.size(); ++j) {
            char val = static_cast<char>(0xFF);
            descriptorStream[genie::core::AnnotDesc::LINKID].write(&val, 1);
        }

        accessUnitcomposer.setCompressors(compressors);
        accessUnitcomposer.setAccessUnit(descriptorStream, attributeTDStream, genotypeDatablock.at(i).attributeInfo,
                                         annotationParameterSet, annotationAccessUnit.at(i), AG_class, AT_ID, 0);
    }
}

void Annotation::parseSite(std::ifstream& inputfile) {
    std::vector<genie::core::AnnotDesc> descrList;
    uint64_t defaultTileSize = 1000;
    genie::variant_site::VariantSiteParser parser(inputfile, infoFields, defaultTileSize);

    for (auto infoField : infoFields) attributeInfo[infoField.ID] = infoField;

    descrList.push_back(genie::core::AnnotDesc::SEQUENCEID);
    descrList.push_back(genie::core::AnnotDesc::STARTPOS);
    descrList.push_back(genie::core::AnnotDesc::STRAND);
    descrList.push_back(genie::core::AnnotDesc::NAME);
    descrList.push_back(genie::core::AnnotDesc::DESCRIPTION);
    descrList.push_back(genie::core::AnnotDesc::LINKNAME);
    descrList.push_back(genie::core::AnnotDesc::LINKID);
    descrList.push_back(genie::core::AnnotDesc::DEPTH);
    descrList.push_back(genie::core::AnnotDesc::SEQQUALITY);
    descrList.push_back(genie::core::AnnotDesc::MAPQUALITY);
    descrList.push_back(genie::core::AnnotDesc::MAPNUMQUALITY0);
    descrList.push_back(genie::core::AnnotDesc::REFERENCE);
    descrList.push_back(genie::core::AnnotDesc::ALTERN);
    descrList.push_back(genie::core::AnnotDesc::FILTER);
    genie::variant_site::ParameterSetComposer encodeParameters;
    annotationParameterSet = encodeParameters.setParameterSet(descrList, parser.getAttributes().getInfo(),
                                                              compressors.getCompressorParameters(), defaultTileSize);

    uint8_t AG_class = 1;
    uint8_t AT_ID = 0;
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
        rowIndex += defaultTileSize;
    }
}

void Annotation::setInfoFields(std::string jsonFileName) {
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
