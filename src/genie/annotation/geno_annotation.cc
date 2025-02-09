/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "geno_annotation.h"

#include <codecs/include/mpegg-codecs.h>

#include "AnnotationEncoder.h"
#include "ParameterSetComposer.h"
#include "genie/core/arrayType.h"
#include "genie/core/constants.h"
#include "genie/core/record/annotation_access_unit/TypedData.h"
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/jbig/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"
#include "genie/genotype/genotype_payload.h"
#include "genie/util/runtime_exception.h"
#include "genie/variantsite/accessunit_composer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

std::vector<GenoUnits> GenoAnnotation::parseGenotype(
    std::ifstream& inputfile,
    std::vector<std::pair<uint64_t, uint8_t>>& numBitPlanes) {
  uint8_t AT_ID = 1;
  constexpr uint8_t AG_class = 0;

  // workaround for different num_bit_plane
  std::vector<GenoUnits> dataunits;
  likelihood_opt.block_size = static_cast<uint32_t>(defaultTileSizeHeight);

  std::vector<ParsBlocks> blocksWPars;
  readBlocks(inputfile, defaultTileSizeHeight, blocksWPars);
  GenoUnits dataunit;
  ParsBlocks combined;
  {
    combined = blocksWPars.at(0);
    for (auto i = 1; i < blocksWPars.size(); ++i) {
      combined.blocks.push_back(blocksWPars.at(i).blocks.at(0));
    }
    AnnotationEncoder encodingPars;

    genie::entropy::bsc::BSCParameters bscParameters;
    auto BSCalgorithmParameters = bscParameters.convertToAlgorithmParameters();

    encodingPars.setDescriptorParameters(genie::core::AnnotDesc::LINKID,
                                         genie::core::AlgoID::BSC,
                                         BSCalgorithmParameters);
    encodingPars.setCompressors(compressors);
    encodingPars.setGenotypeParameters(combined.genotypePars);
    encodingPars.setLikelihoodParameters(combined.likelihoodPars);
    encodingPars.setAttributes(
        combined.blocks.at(0).genotypeDatablock.attributeInfo);
    auto annotationEncodingParameters = encodingPars.Compose();

    ParameterSetComposer parameterset;

    dataunit.annotationParameterSet = parameterset.Compose(
        AT_ID, AG_class, {defaultTileSizeHeight, defaultTileSizeWidth},
        annotationEncodingParameters);

    dataunit.annotationAccessUnit.resize(combined.blocks.size());
  }

  uint32_t blockIndex = 0;
  for (auto& parWBlocks : blocksWPars) {
    std::cerr << " blockIndex: " << std::to_string(blockIndex) << std::endl;

    size_t linkIdRowCnt = 0;
    std::map<std::string,
             genie::core::record::annotation_access_unit::TypedData>
        attributeTDStream;
    std::cerr << " attributeTDStream... " << std::endl;
    for (auto& formatdata :
         combined.blocks.at(blockIndex).genotypeDatablock.attributeData) {
      auto& info = combined.blocks.at(blockIndex)
                       .genotypeDatablock.attributeInfo[formatdata.first];
      std::vector<uint32_t> arrayDims;
      arrayDims.push_back(static_cast<uint32_t>(formatdata.second.size()));
      arrayDims.push_back(combined.blocks.at(blockIndex).numSamples);
      arrayDims.push_back(info.getArrayLength());

      attributeTDStream[formatdata.first].set(
          info.getAttributeType(), static_cast<uint8_t>(arrayDims.size()),
          arrayDims);
      attributeTDStream[formatdata.first].convertToTypedData(formatdata.second);
    }

    std::map<genie::core::AnnotDesc, std::stringstream> descriptorStream;
    descriptorStream[genie::core::AnnotDesc::GENOTYPE];
    {
      genie::genotype::GenotypePayload genotypePayload(
          combined.blocks.at(blockIndex).genotypeDatablock,
          parWBlocks.genotypePars);
      genie::core::Writer writer(
          &descriptorStream[genie::core::AnnotDesc::GENOTYPE]);
      genotypePayload.write(writer);
    }
    variant_site::AccessUnitComposer accessUnitcomposer;
    accessUnitcomposer.setATtype(
        core::record::annotation_access_unit::AnnotationType::VARIANTS, 1);

    accessUnitcomposer.setCompressors(compressors);

    if (combined.blocks.at(blockIndex).likelihoodDatablock.nrows > 0 &&
        combined.blocks.at(blockIndex).likelihoodDatablock.ncols > 0) {
      descriptorStream[genie::core::AnnotDesc::LIKELIHOOD];
      genie::likelihood::LikelihoodPayload likelihoodPayload(
          parWBlocks.likelihoodPars,
          combined.blocks.at(blockIndex).likelihoodDatablock);
      genie::core::Writer writer(
          &descriptorStream[genie::core::AnnotDesc::LIKELIHOOD]);
      likelihoodPayload.write(writer);
    }
    // add LINK_ID default values
    std::cerr << " add link values... " << std::endl;
    for (auto j = 0u;
         j < defaultTileSizeHeight && linkIdRowCnt < parWBlocks.rows;
         ++j, ++linkIdRowCnt) {
      const char val = '\xFF';
      descriptorStream[genie::core::AnnotDesc::LINKID].write(&val, 1);
    }
    std::cerr << " compose accessUnit... " << std::endl;
    accessUnitcomposer.setAccessUnit(
        descriptorStream, attributeTDStream,
        combined.blocks.at(blockIndex).genotypeDatablock.attributeInfo,
        dataunit.annotationParameterSet,
        dataunit.annotationAccessUnit.at(blockIndex), AG_class, AT_ID,
        blockIndex, combined.blocks.at(blockIndex).colStart);
    blockIndex++;
  }
  dataunits.push_back(dataunit);

  return dataunits;
}

size_t GenoAnnotation::readBlocks(std::ifstream& inputfile,
                                  const uint32_t& rowTileSize,
                                  std::vector<ParsBlocks>& blocksWPars) {
  size_t TotalnumberOfRows = 0;

  genie::util::BitReader bitreader(inputfile);

  while (bitreader.IsStreamGood()) {
    genie::genotype::GenotypeParameters genotypeParameters;
    genie::likelihood::LikelihoodParameters likelihoodParameters;
    RecData oneTileData;
    auto rowsInBlock = readOneBlock(bitreader, rowTileSize, genotypeParameters,
                                    likelihoodParameters, oneTileData);
    if (rowsInBlock == 0) return TotalnumberOfRows;

    ParsBlocks parWBlock;
    parWBlock.genotypePars = genotypeParameters;
    parWBlock.likelihoodPars = likelihoodParameters;
    parWBlock.blocks.push_back(oneTileData);
    parWBlock.rows = static_cast<uint32_t>(rowsInBlock);
    blocksWPars.emplace_back(parWBlock);

    TotalnumberOfRows += rowsInBlock;

    if (rowsInBlock < rowTileSize) break;
  }
  return TotalnumberOfRows;
}

size_t genie::annotation::GenoAnnotation::readOneBlock(
    genie::util::BitReader& reader, const uint32_t& rowTileSize,
    genie::genotype::GenotypeParameters& genotypeParameters,
    genie::likelihood::LikelihoodParameters& likelihoodParameters,
    RecData& recData) {
  // read rowTileSize of rows
  std::vector<genie::core::record::VariantGenotype> varGenoType;
  while (reader.IsStreamGood() && varGenoType.size() < rowTileSize) {
    varGenoType.emplace_back(reader);
    if (!reader.IsStreamGood()) varGenoType.pop_back();
  }
  if (varGenoType.empty()) return 0;
  // extract format fields
  std::map<std::string, genie::core::record::format_field> formatList;
  for (auto& rec : varGenoType)
    for (const auto& field : rec.getFormats()) {
      formatList[field.getFormat()] = field;
      genie::core::ArrayType convertArray;
      auto defaultValue = convertArray.toArray(
          field.getType(), convertArray.getDefaultValue(field.getType()));
      std::vector<std::vector<std::vector<uint8_t>>> formatvalue(
          field.getSampleCount(), std::vector<std::vector<uint8_t>>(
                                      field.getArrayLength(), defaultValue));
      formatList[field.getFormat()].setValue(formatvalue);
    }

  // fill every missing value of format field
  for (auto& rec : varGenoType) {
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

  std::tuple<genie::genotype::GenotypeParameters,
             genie::genotype::EncodingBlock>
      genotypeData;

  genotypeData = genie::genotype::encode_block(genotype_opt, varGenoType);

  std::tuple<genie::likelihood::LikelihoodParameters,
             genie::likelihood::EncodingBlock>
      likelihoodData =
          genie::likelihood::encode_block(likelihood_opt, varGenoType);

  uint32_t _numSamples = varGenoType.front().getNumSamples();
  uint8_t _formatCount = varGenoType.front().getFormatCount();
  uint32_t rowStart =
      static_cast<uint32_t>(varGenoType.front().getVariantIndex());
  recData.set(rowStart, 0,
              std::get<genie::genotype::EncodingBlock>(genotypeData),
              std::get<genie::likelihood::EncodingBlock>(likelihoodData),
              _numSamples, _formatCount);

  genotypeParameters =
      std::get<genie::genotype::GenotypeParameters>(genotypeData);
  likelihoodParameters =
      std::get<genie::likelihood::LikelihoodParameters>(likelihoodData);
  return varGenoType.size();
}

}  // namespace annotation
}  // namespace genie
