#include <codecs/include/mpegg-codecs.h>
#include "annotation.h"
#include "genie/core/arrayType.h"
#include "genie/util/runtime-exception.h"

#include "genotype_annotation.h"
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

    // read file - determine type

    if (recType == RecType::GENO_FILE) {
        genotypeAnnotation.setCompressors(compressors);
        DataUnits dataunits = genotypeAnnotation.parseGenotype(inputfile);
        annotationParameterSet = dataunits.annotationParameterSet;
        annotationAccessUnit = dataunits.annotationAccessUnit;

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

DataUnits GenotypeAnnotation::parseGenotype(std::ifstream& inputfile) {
    DataUnits dataunits;
    const uint32_t defaultTileSize = genotype_opt.block_size;  //    10000;
    const uint32_t defaultTileWidth = 3000;
    likelihood_opt.block_size = defaultTileSize;

    constexpr uint8_t AT_ID = 1;
    constexpr uint8_t AG_class = 0;

    genie::genotype::GenotypeParameters genotypeParameters;
    genie::likelihood::LikelihoodParameters likelihoodParameters;
    std::vector<RecData> recData;

    inputfile;
    size_t rowsRead =
        readBlocks(inputfile, defaultTileSize, defaultTileWidth, genotypeParameters, likelihoodParameters, recData);

    genie::genotype::ParameterSetComposer parameterSetComposer;
    parameterSetComposer.setGenotypeParameters(genotypeParameters);
    parameterSetComposer.setLikelihoodParameters(likelihoodParameters);
    parameterSetComposer.setCompressors(compressors);
    dataunits.annotationParameterSet =
        parameterSetComposer.Build(AT_ID, recData.at(0).genotypeDatablock.attributeInfo, defaultTileSize);

    dataunits.annotationAccessUnit.resize(recData.size());

    for (auto i = 0u; i < recData.size(); ++i) {
        std::map<std::string, genie::core::record::annotation_access_unit::TypedData> attributeTDStream;

        for (auto formatdata : recData.at(i).genotypeDatablock.attributeData) {
            auto& info = recData.at(i).genotypeDatablock.attributeInfo[formatdata.first];
            std::vector<uint32_t> arrayDims;
            arrayDims.push_back(std::min(likelihood_opt.block_size, static_cast<uint32_t>(rowsRead)));
            arrayDims.push_back(
                recData.at(i).numSamples);  // static_cast<uint32_t>(formatdata.second.at(0).size()));  //
            arrayDims.push_back(
                recData.at(i).formatCount);  // static_cast<uint32_t>(formatdata.second.at(0).at(0).size()));  //

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
        for (auto j = 0u; i < genotype_opt.block_size && j < defaultTileSize && j < rowsRead; ++j) {
            char val = static_cast<char>(0xFF);
            descriptorStream[genie::core::AnnotDesc::LINKID].write(&val, 1);
        }

        variant_site::AccessUnitComposer accessUnitcomposer;
        accessUnitcomposer.setCompressors(compressors);
        accessUnitcomposer.setAccessUnit(descriptorStream, attributeTDStream,
                                         recData.at(i).genotypeDatablock.attributeInfo,
                                         dataunits.annotationParameterSet, dataunits.annotationAccessUnit.at(i),
                                         AG_class, AT_ID, recData.at(i).rowStart);
    }
    return dataunits;
}

size_t GenotypeAnnotation::readBlocks(std::ifstream& inputfile, const uint32_t& rowTileSize,
                                      const uint32_t& colTilesize,
                                      genie::genotype::GenotypeParameters& genotypeParameters,
                                      genie::likelihood::LikelihoodParameters& likelihoodParameters,
                                      std::vector<RecData>& recData) {
    size_t TotalnumberOfRows = 0;

    genie::util::BitReader bitreader(inputfile);
    uint32_t _rowStart = 0;
    bool start = true;
    while (bitreader.isGood()) {
        std::vector<Tile> horizontalTiles;

        for (uint32_t rowCount = 0; rowCount < rowTileSize && bitreader.isGood(); ++rowCount) {
            genie::core::record::VariantGenotype varGenoType(bitreader);
            if (bitreader.isGood()) {
                TotalnumberOfRows++;
                auto recSplitOverCols = splitOnRows(varGenoType, colTilesize);
                if (horizontalTiles.empty()) horizontalTiles.resize(recSplitOverCols.size());
                for (size_t coltile = 0; coltile < recSplitOverCols.size(); ++coltile)
                    horizontalTiles.at(coltile).push_back(recSplitOverCols.at(coltile));
            }
        }

        std::map<std::string, genie::core::record::format_field> formatList;
        //  uint16_t attributeId = 0;
        for (auto& hortile : horizontalTiles) {
            for (auto& rec : hortile) {
                for (auto& field : rec.getFormats()) {
                    if (formatList[field.getFormat()].getFormat() == "") {
                        formatList[field.getFormat()] = field;
                        genie::core::ArrayType convertArray;
                        auto defaultValue =
                            convertArray.toArray(field.getType(), convertArray.getDefaultValue(field.getType()));

                        std::vector<std::vector<std::vector<uint8_t>>> formatvalue(
                            field.getSampleCount(),
                            std::vector<std::vector<uint8_t>>(field.getArrayLength(), defaultValue));
                        formatList[field.getFormat()].setValue(formatvalue);
                    }
                }
            }
        }

        for (auto& hortile : horizontalTiles) {
            for (auto& rec : hortile) {
                auto formats = rec.getFormats();
                for (auto& availableFormats : formatList) {
                    bool available = false;
                    for (auto& currentFormat : formats)
                        if (currentFormat.getFormat() == availableFormats.first) {
                            available = true;
                            break;
                        }
                    if (!available) formats.push_back(availableFormats.second);
                }
                rec.setFormats(formats);
            }
        }

        uint32_t _colStart = 0;
        for (auto& hortile : horizontalTiles) {
            std::tuple<genie::genotype::GenotypeParameters, genie::genotype::EncodingBlock> genotypeData;

            genotypeData = genie::genotype::encode_block(genotype_opt, hortile);

            std::tuple<genie::likelihood::LikelihoodParameters, genie::likelihood::EncodingBlock> likelihoodData =
                genie::likelihood::encode_block(likelihood_opt, hortile);

            uint32_t _numSamples = hortile.front().getNumSamples();
            uint8_t _formatCount = hortile.front().getFormatCount();
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
    }

    return TotalnumberOfRows;
}

std::vector<genie::core::record::VariantGenotype> GenotypeAnnotation::splitOnRows(
    genie::core::record::VariantGenotype& rec, uint32_t colWidth) {
    std::vector<genie::core::record::VariantGenotype> splitRecs;

    if (rec.getNumSamples() <= colWidth) {
        splitRecs.push_back(rec);
    } else {
        auto likelihoods = rec.getLikelihoods();
        auto phasings = rec.getPhasing();
        auto alleles = rec.getAlleles();
        auto formats = rec.getFormats();

        auto totSampleSize = rec.getNumSamples();

        for (uint32_t colpos = 0; colpos < totSampleSize; colpos += colWidth) {
            genie::core::record::VariantGenotype variantGenotype(rec.getVariantIndex(), rec.getStartSampleIndex());
            uint32_t subSampleSize = std::min(colWidth, (totSampleSize - colpos));

            if (likelihoods.size() > 0) {
                auto LiklihoodStart = likelihoods.begin() + colpos;
                auto LiklihoodEnd = likelihoods.begin() + colpos + subSampleSize;
                std::vector<std::vector<uint32_t>> subLikelihoods(LiklihoodStart, LiklihoodEnd);
                variantGenotype.setLikelihood(subLikelihoods);
            }
            auto phasingStart = phasings.begin() + colpos;
            auto phasingEnd = phasings.begin() + colpos + subSampleSize;
            auto allelesStart = alleles.begin() + colpos;
            auto allelesEnd = alleles.begin() + colpos + subSampleSize;
            std::vector<std::vector<uint8_t>> subPhasings(phasingStart, phasingEnd);
            std::vector<std::vector<int8_t>> subAlleles(allelesStart, allelesEnd);

            variantGenotype.setPhasings(subPhasings);
            variantGenotype.setAlleles(subAlleles);
            variantGenotype.setNumberOfSamples(subSampleSize);

            //--------------- formats ----------------//
            if (formats.size() > 0) {
                std::vector<genie::core::record::format_field> format;
                for (auto& form : formats) {
                    genie::core::record::format_field oneField(form.getFormat(), form.getType(), form.getArrayLength());
                    oneField.setSampleCount(subSampleSize);
                    auto FormatStart = form.getValue().begin() + colpos;
                    auto FormatEnd = form.getValue().begin() + colpos + subSampleSize;
                    std::vector<std::vector<std::vector<uint8_t>>> format_value(FormatStart, FormatEnd);
                    oneField.setValue(format_value);
                    format.push_back(oneField);
                }
                variantGenotype.setFormats(format);
            }
            //--------------- formats ----------------//

            splitRecs.push_back(variantGenotype);
        }
    }
    return splitRecs;
}

void Annotation::parseSite(std::ifstream& inputfile) {
    std::vector<genie::core::AnnotDesc> descrList;
    uint64_t defaultTileSize = 10000;
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
        rowIndex++;
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
