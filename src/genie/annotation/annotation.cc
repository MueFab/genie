/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include <chrono>

#include <codecs/include/mpegg-codecs.h>
#include "annotation.h"
#include "genie/util/runtime-exception.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

class MeasureTime {
 public:
    MeasureTime(std::string _message) : message(_message) { start = std::chrono::high_resolution_clock::now(); }
    ~MeasureTime() {
        using namespace std::chrono_literals;
        auto end = std::chrono::high_resolution_clock::now();
        auto diff = end - start;
        auto sec = diff / 1s;
        auto milli = diff / 1ms;
        auto minutes = diff / 1min;
        auto millisec = milli - sec * 1000;
        auto secmin = sec - minutes * 60;
        std::cerr << message << " Time(ns): " << minutes << " min " << secmin << " sec " << millisec << " millisec"
                  << '\n';
    }

 private:
    std::chrono::steady_clock::time_point start;
    std::string message;
};

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
    MeasureTime timeToFile("\t\tToFile: ");
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

    // std::vector<genie::genotype::EncodingBlock> genotypeDatablock;
    //  std::vector<genie::likelihood::EncodingBlock> likelihoodDatablock;
    genie::genotype::GenotypeParameters genotypeParameters;
    genie::likelihood::LikelihoodParameters likelihoodParameters;
    // std::vector<uint32_t> numSamples;
    // std::vector<uint8_t> formatCount;
    std::vector<RecData> recData;
    size_t rowsRead = 0;
    uint32_t defaultTileWidth = 2500;  // 2 * defaultTileSize;// 1050;
    rowsRead =
        readBlocks(inputfile, defaultTileSize, defaultTileWidth, genotypeParameters, likelihoodParameters, recData);
    std::cerr << "recData size: " << std::to_string(recData.size()) << std::endl;
    std::cerr << "rowsRead: " << std::to_string(rowsRead) << '\n';
    //--------------------------------------------------

    genie::genotype::ParameterSetComposer parameterSetComposer;
    {
        MeasureTime timeParametersetComposer("ParameterSetComposer: ");
        parameterSetComposer.setGenotypeParameters(genotypeParameters);
        parameterSetComposer.setLikelihoodParameters(likelihoodParameters);
        parameterSetComposer.setCompressors(compressors);
        annotationParameterSet =
            parameterSetComposer.Build(AT_ID, recData.at(0).genotypeDatablock.attributeInfo, rowsRead);
    }
    annotationAccessUnit.resize(recData.size());
    for (auto i = 0u; i < recData.size(); ++i) {
        //       MeasureTime timeAAUtile("\tper tile: " + std::to_string(i) + " ");
        std::map<std::string, genie::core::record::annotation_access_unit::TypedData> attributeTDStream;

        for (auto formatdata : recData.at(i).genotypeDatablock.attributeData) {
            auto& info = recData.at(i).genotypeDatablock.attributeInfo[formatdata.first];
            std::vector<uint32_t> arrayDims;
            arrayDims.push_back(std::min(likelihood_opt.block_size, static_cast<uint32_t>(rowsRead)));
            arrayDims.push_back(recData.at(i).numSamples);
            arrayDims.push_back(recData.at(i).formatCount);
            attributeTDStream[formatdata.first].set(info.getAttributeType(), static_cast<uint8_t>(arrayDims.size()),
                                                    arrayDims);
            attributeTDStream[formatdata.first].convertToTypedData(formatdata.second);
        }

        std::map<genie::core::AnnotDesc, std::stringstream> descriptorStream;
        descriptorStream[genie::core::AnnotDesc::GENOTYPE];
        {
            genie::genotype::GenotypePayload genotypePayload(recData.at(i).genotypeDatablock, genotypeParameters);
            genie::core::Writer writer(&descriptorStream[genie::core::AnnotDesc::GENOTYPE]);
            genotypePayload.write(writer);
        }

        if (recData.at(i).likelihoodDatablock.nrows > 0 && recData.at(i).likelihoodDatablock.ncols > 0) {
            descriptorStream[genie::core::AnnotDesc::LIKELIHOOD];
            genie::likelihood::LikelihoodPayload likelihoodPayload(likelihoodParameters,
                                                                   recData.at(i).likelihoodDatablock);
            genie::core::Writer writer(&descriptorStream[genie::core::AnnotDesc::LIKELIHOOD]);
            likelihoodPayload.write(writer);
        }

        // add LINK_ID default values
        for (auto j = 0u; i < genotype_opt.block_size && j < rowsRead; ++j) {
            char val = static_cast<char>(0xFF);
            descriptorStream[genie::core::AnnotDesc::LINKID].write(&val, 1);
        }

        accessUnitcomposer.setCompressors(compressors);
        accessUnitcomposer.setAccessUnit(descriptorStream, attributeTDStream,
                                         recData.at(i).genotypeDatablock.attributeInfo, annotationParameterSet,
                                         annotationAccessUnit.at(i), AG_class, AT_ID, recData.at(i).rowStart);
    }
    std::cerr << "nr of aaus: " << std::to_string(annotationAccessUnit.size()) << '\n';
}

size_t Annotation::readBlocks(std::ifstream& inputfile, const uint32_t& rowTileSize, const uint32_t& colTilesize,
                              genie::genotype::GenotypeParameters& genotypeParameters,
                              genie::likelihood::LikelihoodParameters& likelihoodParameters,
                              std::vector<RecData>& recData) {
    //   MeasureTime measureReadBlocks("\treadBlocks: ");
    genie::util::BitReader bitreader(inputfile);
    size_t TotalnumberOfRows = 0;
    bool start = true;
    std::vector<uint32_t> colTilesPerRow;
    uint32_t _rowStart = 0;
    (void)rowTileSize;
    while (bitreader.isGood()) {  //&& TotalnumberOfRows < 100) {
        uint32_t rowCount = 0;
        std::vector<std::vector<genie::core::record::VariantGenotype>> recsTiled;
        {
            MeasureTime measurerowTile("\t\tReading1rowTile: ");

            while (bitreader.isGood() && rowCount < rowTileSize) {  // && rowCount < 100) {
                genie::core::record::VariantGenotype varGenoType(bitreader);
                auto recSplitOverCols = splitOnRows(varGenoType, colTilesize);
                colTilesPerRow.push_back((uint32_t)recSplitOverCols.size());
                if (recsTiled.size() == 0) recsTiled.resize(recSplitOverCols.size());

                for (auto i = 0u; i < recSplitOverCols.size(); ++i)
                    recsTiled.at(i).emplace_back(recSplitOverCols.at(i));
                rowCount++;
            }
            std::cerr << "rowCount: " << std::to_string(rowCount) << ", ";
        }
        std::cerr << "\n recsTiled: " << std::to_string(recsTiled.size()) << std::endl;

        if (!bitreader.isGood()) recsTiled.pop_back();
        if (recsTiled.size() == 0) return TotalnumberOfRows;

        uint32_t _colStart = 0;
        for (auto& recs : recsTiled) {
            std::tuple<genie::genotype::GenotypeParameters, genie::genotype::EncodingBlock> genotypeData;
            {
                MeasureTime timeGenotype("\tencodeBlockGenotype: (" + std::to_string(TotalnumberOfRows) + ")");
                genotypeData = genie::genotype::encode_block(genotype_opt, recs);
            }
            std::tuple<genie::likelihood::LikelihoodParameters, genie::likelihood::EncodingBlock> likelihoodData =
                genie::likelihood::encode_block(likelihood_opt, recs);

            genie::genotype::EncodingBlock _genotypeDatablock;
            genie::likelihood::EncodingBlock _likelihoodDatablock;
            uint32_t _numSamples = recs.front().getNumSamples();
            uint8_t _formatCount = recs.front().getFormatCount();
            recData.emplace_back(_rowStart, _colStart, std::get<genie::genotype::EncodingBlock>(genotypeData),
                                 std::get<genie::likelihood::EncodingBlock>(likelihoodData), _numSamples, _formatCount);

            if (start) {
                genotypeParameters = std::get<genie::genotype::GenotypeParameters>(genotypeData);
                likelihoodParameters = std::get<genie::likelihood::LikelihoodParameters>(likelihoodData);
                start = false;
            }
            _colStart++;
        }
        _rowStart++;
        TotalnumberOfRows += rowCount;
    }
    return TotalnumberOfRows;
}

std::vector<genie::core::record::VariantGenotype> Annotation::splitOnRows(genie::core::record::VariantGenotype& rec,
                                                                          uint32_t colWidth) {
    std::vector<genie::core::record::VariantGenotype> splitRecs;

    if (rec.getNumSamples() <= colWidth) {
        splitRecs.push_back(rec);
    } else {
        auto likelihoods = rec.getLikelihoods();
        auto phasings = rec.getPhasing();
        auto alleles = rec.getAlleles();

        auto totSampleSize = rec.getNumSamples();

        for (uint32_t colpos = 0; colpos < totSampleSize; colpos += colWidth) {
            genie::core::record::VariantGenotype variantGenotype(rec.getVariantIndex(), rec.getStartSampleIndex());
            uint32_t subSampleSize = std::min(colWidth, (totSampleSize - colpos));
            auto LiklihoodStart = likelihoods.begin() + colpos;
            auto LiklihoodEnd = likelihoods.begin() + colpos + subSampleSize;
            auto phasingStart = phasings.begin() + colpos;
            auto phasingEnd = phasings.begin() + colpos + subSampleSize;
            auto allelesStart = alleles.begin() + colpos;
            auto allelesEnd = alleles.begin() + colpos + subSampleSize;

            std::vector<std::vector<uint32_t>> subLikelihoods(LiklihoodStart, LiklihoodEnd);
            std::vector<std::vector<uint8_t>> subPhasings(phasingStart, phasingEnd);
            std::vector<std::vector<int8_t>> subAlleles(allelesStart, allelesEnd);

            variantGenotype.setLikelihood(subLikelihoods);
            variantGenotype.setPhasings(subPhasings);
            variantGenotype.setAlleles(subAlleles);
            variantGenotype.setNumberOfSamples(subSampleSize);
            splitRecs.push_back(variantGenotype);
        }
    }
    return splitRecs;
}

void Annotation::parseSite(std::ifstream& inputfile) {
    std::vector<genie::core::AnnotDesc> descrList;
    uint64_t defaultTileSize = 1000;
    genie::variant_site::VariantSiteParser parser(inputfile, infoFields, defaultTileSize);
    uint8_t AG_class = 1;
    uint8_t AT_ID = 1;

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
    annotationParameterSet = encodeParameters.setParameterSet(
        descrList, parser.getAttributes().getInfo(), compressors.getCompressorParameters(), defaultTileSize, AT_ID);

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
