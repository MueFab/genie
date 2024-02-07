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
                                                std::string attributeJsonFileName, std::string outputFileName)

{
    // read attributes info from json file
    std::ifstream AttributeFieldsFile;
    AttributeFieldsFile.open(attributeJsonFileName, std::ios::in);
    std::stringstream attributeFields;
    UTILS_DIE_IF(!AttributeFieldsFile.is_open(), "unable to open json file");

    if (AttributeFieldsFile.is_open()) {
        attributeFields << AttributeFieldsFile.rdbuf();
        AttributeFieldsFile.close();
    }
    JsonAttributeParser attributeParser(attributeFields);
    // determine the compressors

    std::ifstream inputfile;
    inputfile.open(recordInputFileName, std::ios::in | std::ios::binary);

    if (!inputfile.is_open())  // failed
        return;

    // read file - determine type?

    if (recType == RecType::GENO_FILE) {
        parseGenotype(inputfile);

    } else {
        infoFields = attributeParser.getInfoFields();
        parseSite(inputfile);
    }
    if (inputfile.is_open()) inputfile.close();

    // determine and write the annotation parameter set to output(stream)
    // parse n rows for complete tile or until end of file
    // determine and write the annotation access unit to output(stream)

    std::ofstream testfile;
    std::string filename = outputFileName;
    testfile.open(filename + ".bin", std::ios::binary | std::ios::out);
    genie::core::Writer testwriter(&testfile);
    std::ofstream txtfile;
    txtfile.open(filename + ".txt", std::ios::out);
    genie::core::Writer txtwriter(&txtfile, true);

    genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);
    APS_dataUnit.write(testwriter);
    APS_dataUnit.write(txtwriter);

    for (auto& aau : annotationAccessUnit) {
        genie::core::record::data_unit::Record AAU_dataUnit(aau);
        AAU_dataUnit.write(testwriter);
        AAU_dataUnit.write(txtwriter);
    }
    testfile.close();
    txtfile.close();
}

void Annotation::parseGenotype(std::ifstream& inputfile) {
    std::vector<genie::core::AnnotDesc> descrList;
    annotationAccessUnit.resize(1);
    descrList.push_back(genie::core::AnnotDesc::GENOTYPE);
    descrList.push_back(genie::core::AnnotDesc::LIKELIHOOD);
    descrList.push_back(genie::core::AnnotDesc::LINKID);

    std::vector<genie::core::record::VariantGenotype> recs;
    genie::util::BitReader bitreader(inputfile);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    recs.pop_back();

    uint32_t BLOCK_SIZE = 200;
    bool TRANSFORM_MODE = true;

    genie::likelihood::EncodingOptions likelihood_opt = {
        BLOCK_SIZE,      // block_size
        TRANSFORM_MODE,  // transform_flag;
    };

    genie::genotype::EncodingOptions genotype_opt = {
        BLOCK_SIZE,                                  // block_size;
        genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
        false,                                       // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::core::AlgoID::JBIG                    // codec_ID;
    };

    auto genotypeData = genie::genotype::encode_block(genotype_opt, recs);
    auto likelihoodData = genie::likelihood::encode_block(likelihood_opt, recs);
    //--------------------------------------------------
    uint8_t AT_ID = 1;
    uint8_t AG_class = 0;
    genie::genotype::GenotypeParameters genotypeParameters =
        std::get<genie::genotype::GenotypeParameters>(genotypeData);
    auto datablock = std::get<genie::genotype::EncodingBlock>(genotypeData);
    genie::genotype::ParameterSetComposer genotypeParameterSet;
    annotationParameterSet =
        genotypeParameterSet.Build(AT_ID, datablock.attributeInfo, genotypeParameters,
                                   std::get<genie::likelihood::LikelihoodParameters>(likelihoodData), recs.size());
    std::map<std::string, genie::core::record::annotation_access_unit::TypedData> attributeTDStream;

    for (auto formatdata : datablock.attributeData) {
        auto& info = datablock.attributeInfo[formatdata.first];
        std::vector<uint32_t> arrayDims;
        arrayDims.push_back(std::min(BLOCK_SIZE, static_cast<uint32_t>(recs.size())));
        arrayDims.push_back(recs.at(0).getNumSamples());
        arrayDims.push_back(recs.at(0).getFormatCount());
        attributeTDStream[formatdata.first].set(info.getAttributeType(), static_cast<uint8_t>(arrayDims.size()),
                                                arrayDims);
        attributeTDStream[formatdata.first].convertToTypedData(formatdata.second);
    }

    std::map<genie::core::AnnotDesc, std::stringstream> descriptorStream;
    descriptorStream[genie::core::AnnotDesc::GENOTYPE];
    {
        genie::genotype::GenotypePayload genotypePayload(datablock, genotypeParameters);
        genie::core::Writer writer(&descriptorStream[genie::core::AnnotDesc::GENOTYPE]);
        genotypePayload.write(writer);
    }

    descriptorStream[genie::core::AnnotDesc::LIKELIHOOD];
    {
        genie::likelihood::LikelihoodPayload payload(std::get<genie::likelihood::LikelihoodParameters>(likelihoodData),
                                                     std::get<genie::likelihood::EncodingBlock>(likelihoodData));
        genie::core::Writer writer(&descriptorStream[genie::core::AnnotDesc::LIKELIHOOD]);
        payload.write(writer);
    }

    // add LINK_ID default values
    for (auto i = 0u; i < BLOCK_SIZE && i < recs.size(); ++i) {
        char val = static_cast<char>(0xFF);
        descriptorStream[genie::core::AnnotDesc::LINKID].write(&val, 1);
    }
    accessUnitcomposer.setAccessUnit(descriptorStream, attributeTDStream, datablock.attributeInfo,
                                     annotationParameterSet, annotationAccessUnit.at(0), AG_class, AT_ID, 0);
}

void Annotation::parseSite(std::ifstream& inputfile) {
    std::vector<genie::core::AnnotDesc> descrList;
    uint64_t defaultTileSize = 1000;
    genie::variant_site::VariantSiteParser parser(inputfile, infoFields, defaultTileSize);
    std::map<std::string, InfoField> attributeInfo;
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
    annotationParameterSet =
        encodeParameters.setParameterSet(descrList, parser.getAttributes().getInfo(), defaultTileSize);

    uint8_t AG_class = 1;
    uint8_t AT_ID = 0;
    genie::variant_site::AccessUnitComposer accessUnit;
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
    infoFields = attributeParser.getInfoFields();
}

}  // namespace annotation
}  // namespace genie
