#include <codecs/include/mpegg-codecs.h>
#include "genie/core/arrayType.h"
#include "genie/util/runtime-exception.h"

#include "genie/core/record/annotation_access_unit/TypedData.h"
#include "genie/variantsite/AccessUnitComposer.h"

#include "geno_annotation.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

GenoUnits GenoAnnotation::parseGenotype(std::ifstream& inputfile) {
    GenoUnits dataunits;
    likelihood_opt.block_size = static_cast<uint32_t>(defaultTileSizeHeight);

    constexpr uint8_t AT_ID = 1;
    constexpr uint8_t AG_class = 0;

    genie::genotype::GenotypeParameters genotypeParameters;
    genie::likelihood::LikelihoodParameters likelihoodParameters;
    std::vector<RecData> recData;

    size_t rowsRead = readBlocks(inputfile, defaultTileSizeHeight, defaultTileSizeWidth, genotypeParameters,
                                 likelihoodParameters, recData);
    genie::genotype::ParameterSetComposer parameterSetComposer;
    parameterSetComposer.setGenotypeParameters(genotypeParameters);
    parameterSetComposer.setLikelihoodParameters(likelihoodParameters);
    parameterSetComposer.setCompressors(compressors);
    dataunits.annotationParameterSet = parameterSetComposer.Build(AT_ID, recData.at(0).genotypeDatablock.attributeInfo,
                                                                  {defaultTileSizeHeight, defaultTileSizeWidth});

    dataunits.annotationAccessUnit.resize(recData.size());
    size_t linkIdRowCnt = 0;
    for (auto i = 0u; i < recData.size(); ++i) {
        std::map<std::string, genie::core::record::annotation_access_unit::TypedData> attributeTDStream;

        for (auto& formatdata : recData.at(i).genotypeDatablock.attributeData) {
            auto& info = recData.at(i).genotypeDatablock.attributeInfo[formatdata.first];
            std::vector<uint32_t> arrayDims;
            arrayDims.push_back(static_cast<uint32_t>(formatdata.second.size()));
            arrayDims.push_back(
                recData.at(i).numSamples);  // static_cast<uint32_t>(formatdata.second.at(0).size()));  //
            arrayDims.push_back(
                info.getArrayLength());  // static_cast<uint32_t>(formatdata.second.at(0).at(0).size()));  //

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
        variant_site::AccessUnitComposer accessUnitcomposer;
        accessUnitcomposer.setCompressors(compressors);
        if (true) {
            if (recData.at(i).likelihoodDatablock.nrows > 0 && recData.at(i).likelihoodDatablock.ncols > 0) {
                descriptorStream[genie::core::AnnotDesc::LIKELIHOOD];
                genie::likelihood::LikelihoodPayload likelihoodPayload(likelihoodParameters,
                                                                       recData.at(i).likelihoodDatablock);
                genie::core::Writer writer(&descriptorStream[genie::core::AnnotDesc::LIKELIHOOD]);
                likelihoodPayload.write(writer);
            }
            // add LINK_ID default values
            for (auto j = 0u; j < defaultTileSizeHeight && linkIdRowCnt < rowsRead; ++j, ++linkIdRowCnt) {
                const char val = '\xFF';
                descriptorStream[genie::core::AnnotDesc::LINKID].write(&val, 1);
            }

            accessUnitcomposer.setAccessUnit(descriptorStream, attributeTDStream,
                                             recData.at(i).genotypeDatablock.attributeInfo,
                                             dataunits.annotationParameterSet, dataunits.annotationAccessUnit.at(i),
                                             AG_class, AT_ID, recData.at(i).rowStart, recData.at(i).colStart);
        }
    }
    return dataunits;
}

size_t GenoAnnotation::readBlocks(std::ifstream& inputfile, const uint32_t& rowTileSize, const uint32_t& colTilesize,
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
        for (const auto& hortile : horizontalTiles) {
            for (const auto& rec : hortile) {
                for (const auto& field : rec.getFormats()) {
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
                for (const auto& availableFormats : formatList) {
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

std::vector<genie::core::record::VariantGenotype> GenoAnnotation::splitOnRows(genie::core::record::VariantGenotype& rec,
                                                                              uint32_t colWidth) {
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

}  // namespace annotation
}  // namespace genie
