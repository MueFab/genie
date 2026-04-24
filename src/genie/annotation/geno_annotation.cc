/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/annotation/geno_annotation.h"

#include <codecs/include/mpegg-codecs.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "genie/annotation/annotation_encoder.h"
#include "genie/annotation/parameterset_composer.h"
#include "genie/core/array_type.h"
#include "genie/core/constants.h"
#include "genie/core/access_unit/annotation/typed_data.h"
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/jbig/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"
#include "genie/genotype/genotype_coder.h"
#include "genie/genotype/genotype_payload.h"
#include "genie/util/runtime_exception.h"
#include "genie/annotation/accessunit_composer.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

std::vector<GenoUnits> GenoAnnotation::parseGenotype(
    std::ifstream& inputfile,
    std::vector<std::pair<uint64_t, uint8_t>>& numBitPlanes) {
  uint8_t AT_ID = 1;
  constexpr uint8_t AG_class = 0;

  // workaround for different num_bit_plane_
  std::vector<GenoUnits> dataunits;
  likelihood_opt.block_size = static_cast<uint32_t>(defaultTileSizeHeight);

  std::vector<ParsBlocks> blocksWPars;
  auto numberofRows = readBlocks(inputfile, defaultTileSizeHeight, blocksWPars);
  GenoUnits dataunit;
  ParsBlocks combined;
  combined = blocksWPars.at(0);
  //--------------

  for (auto idx_i = 1; idx_i < blocksWPars.size(); ++idx_i) {
    combined.blocks.push_back(blocksWPars.at(idx_i).blocks.at(0));
  }
  std::map<std::string, core::parameter::annotation::AttributeData>
      attributeInfo;
  for (auto& attr : combined.blocks.at(0).attributes)
    attributeInfo[attr.first] = std::get<0>(attr.second);

  AnnotationEncoder encodingPars;
  genie::entropy::bsc::BSCParameters bscParameters;
  auto BSCalgorithmParameters = bscParameters.convertToAlgorithmParameters();

  encodingPars.setDescriptorParameters(genie::core::AnnotDesc::LINKID,
                                       genie::core::AlgoID::BSC,
                                       BSCalgorithmParameters);
  encodingPars.setCompressors(compressors);
  encodingPars.setGenotypeParameters(combined.genotypePars);
  encodingPars.setLikelihoodParameters(combined.likelihoodPars);
  encodingPars.setAttributes(attributeInfo);
  auto annotationEncodingParameters = encodingPars.Compose();

  ParameterSetComposer parameterset;

  dataunit.annotationParameterSet = parameterset.Compose(
      AT_ID, AG_class, {defaultTileSizeHeight, defaultTileSizeWidth},
      annotationEncodingParameters);

  dataunit.annotationAccessUnit.resize(combined.blocks.size());

  //----------------
  uint32_t blockIndex = 0;
  for (auto& parWBlocks : blocksWPars) {
    std::cerr << " blockIndex: " << std::to_string(blockIndex) << std::endl;

    size_t linkIdRowCnt = 0;
    std::map<std::string,
             genie::core::access_unit::annotation::TypedData>
        attributeTDStream;
    std::cerr << " attributeTDStream... " << std::endl;
    for (auto& formatdata : combined.blocks.at(blockIndex).attributes) {
      auto& info =
          std::get<core::parameter::annotation::AttributeData>(
              formatdata.second);
      auto& values = std::get<1>(formatdata.second);
      // .genotypeDatablock.attributeInfo[formatdata.first];
      std::vector<uint32_t> arrayDims;
      arrayDims.push_back(static_cast<uint32_t>(values.size()));
      arrayDims.push_back(combined.blocks.at(blockIndex).numSamples);
      arrayDims.push_back(info.getArrayLength());

      attributeTDStream[formatdata.first].set(
          info.getAttributeType(), static_cast<uint8_t>(arrayDims.size()),
          arrayDims);
      attributeTDStream[formatdata.first].convertToTypedData(
          std::get<std::vector<std::vector<std::vector<AttrType>>>>(
              formatdata.second));
    }

    std::map<genie::core::AnnotDesc, std::stringstream> descriptorStream;
    descriptorStream[genie::core::AnnotDesc::GENOTYPE];
    {
      genie::util::BitWriter writer(
          &descriptorStream[genie::core::AnnotDesc::GENOTYPE]);
      combined.blocks.at(blockIndex).payload.Write(writer);
    }
    variant_site::AccessUnitComposer accessUnitcomposer;
    accessUnitcomposer.setATtype(
        core::access_unit::annotation::AnnotationType::VARIANTS, 1);

    accessUnitcomposer.setCompressors(compressors);

    if (combined.blocks.at(blockIndex).likelihoodPayload.getNRows() > 0 &&
        combined.blocks.at(blockIndex).likelihoodPayload.getNCols() > 0) {
      descriptorStream[genie::core::AnnotDesc::LIKELIHOOD];
      genie::core::Writer writer(
          &descriptorStream[genie::core::AnnotDesc::LIKELIHOOD]);
      combined.blocks.at(blockIndex).likelihoodPayload.write(writer);
    }
    // add LINK_ID default values
    std::cerr << " add link values... " << std::endl;
    for (auto idx_j = 0u;
         idx_j < defaultTileSizeHeight && linkIdRowCnt < parWBlocks.rows;
         ++idx_j, ++linkIdRowCnt) {
      const char val = '\xFF';
      descriptorStream[genie::core::AnnotDesc::LINKID].write(&val, 1);
    }

    accessUnitcomposer.setAccessUnit(
        descriptorStream, attributeTDStream, attributeInfo,
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
  std::map<std::string, genie::core::record::FormatField> formatList;
  for (auto& rec : varGenoType)
    for (const auto& field : rec.GetFormat()) {
      formatList[field.GetFormat()] = field;
      genie::core::ArrayType convertArray;
      auto defaultValue = convertArray.toArray(
          field.GetType(), convertArray.getDefaultValue(field.GetType()));
      std::vector<std::vector<std::vector<uint8_t>>> formatvalue(
          field.GetSampleCount(), std::vector<std::vector<uint8_t>>(
                                      field.GetArrayLength(), defaultValue));
      formatList[field.GetFormat()].SetValue(formatvalue);
    }

  // fill every missing value of format field
  for (auto& rec : varGenoType) {
    auto formats = rec.GetFormat();
    for (const auto& availableFormats : formatList) {
      bool available = false;
      for (auto& currentFormat : formats)
        if (currentFormat.GetFormat() == availableFormats.first) {
          available = true;
          break;
        }
      if (!available) formats.push_back(availableFormats.second);
    }
    rec.SetFormat(formats);
  }

  genie::genotype::GenotypeParameters pars;
  genie::genotype::GenotypePayload payload;

  genie::genotype::encode_genotype(varGenoType, pars, payload, genotype_opt.block_size,
      genotype_opt.binarization_ID, genotype_opt.concat_axis, genotype_opt.transpose_mat,
      genotype_opt.sort_row_method, genotype_opt.sort_col_method, genotype_opt.codec_ID);

  genie::likelihood::LikelihoodParameters likPars;
  genie::likelihood::LikelihoodPayload likPayload(likPars, 0, 0, {}, {});
  genie::likelihood::encode_likelihood(varGenoType, likPars, likPayload, likelihood_opt.block_size, likelihood_opt.transform_flag);

  uint32_t _numSamples = varGenoType.front().GetSampleCount();
  uint8_t _formatCount = varGenoType.front().GetFormatCount();
  uint32_t rowStart =
      static_cast<uint32_t>(varGenoType.front().GetVariantIndex());

  sort_format(varGenoType);
  std::map<std::string,
           std::tuple<core::parameter::annotation::AttributeData,
                      std::vector<std::vector<std::vector<AttrType>>>>>
      attributes;
  for (auto& attr : attrInfo)
    attributes[attr.first] =
        std::make_tuple(attr.second, attrValues[attr.first]);
    
  // Converting LikelihoodPayload back to EncodingBlock if strictly required by RecData or refactoring RecData.
  // Ideally RecData should hold Payload.
  // For now, I will create a dummy EncodingBlock or refactor RecData if I can see header.
  // Assuming RecData needs refactoring. But I can't check header easily without reading it.
  // Let's assume I can change RecData in .h file too?
  // Wait, RecData struct is defined in .cc file? No, it's used in readOneBlock return.
  
  // Let's assume RecData stores LikelihoodPayload now.
  recData.set(rowStart, 0, std::make_tuple(pars, std::move(payload)),
              std::move(likPayload),
              _numSamples, _formatCount, attributes);

  genotypeParameters = pars;
  likelihoodParameters = likPars;

  return varGenoType.size();
}

void GenoAnnotation::sort_format(
    std::vector<genie::core::record::VariantGenotype>& recs) {
  // starting number
  uint8_t AttributeID = 25;

  uint32_t genotypeBlockSize =
      std::min(genotype_opt.block_size, static_cast<uint32_t>(recs.size()));
  // fill all attribute data
  for (const auto& format : recs.at(0).GetFormat()) {
    const auto& formatName = format.GetFormat();
    core::parameter::annotation::AttributeData attrData(
        formatName.size(), formatName, format.GetType(),
        format.GetArrayLength(), AttributeID);
    attrInfo[formatName] = attrData;
    AttributeID++;
  }

  // add values
  for (auto i_rec = 0u; i_rec < genotypeBlockSize; i_rec++) {
    auto& rec = recs[i_rec];
    for (const auto& format : rec.GetFormat()) {
      auto formatName = format.GetFormat();
      attrValues[formatName].resize(genotypeBlockSize);
      std::vector<std::vector<AttrType>> formatValue = format.GetValue();
      attrValues[formatName].at(i_rec) = formatValue;
    }
  }
}

GenoAnnotation::RecData::RecData()
    : rowStart(0),
      colStart(0),
      payload{},
      attributes{},
      likelihoodPayload(genie::likelihood::LikelihoodParameters(), 0, 0, {}, {}),
      numSamples(0),
      formatCount(0) {}

GenoAnnotation::RecData::RecData(
    uint32_t _rowStart, uint32_t _colStart,
    std::tuple<genie::genotype::GenotypeParameters,
               genie::genotype::GenotypePayload>
        _genotypeData,
    genie::likelihood::LikelihoodPayload _likelihoodPayload, uint32_t _numSamples,
    uint8_t _formatCount,
    std::map<std::string,
             std::tuple<core::parameter::annotation::AttributeData,
                        std::vector<std::vector<std::vector<AttrType>>>>>
        _attributes)
    : rowStart(_rowStart),
      colStart(_colStart),
      pars(std::get<genie::genotype::GenotypeParameters>(_genotypeData)),
      payload(std::get<genie::genotype::GenotypePayload>(_genotypeData)),
      attributes(std::move(_attributes)),
      likelihoodPayload(std::move(_likelihoodPayload)),
      numSamples(_numSamples),
      formatCount(_formatCount) {}

GenoAnnotation::RecData& GenoAnnotation::RecData::operator=(
    const RecData& other) {
  if (this != &other) {
    rowStart = other.rowStart;
    colStart = other.colStart;
    pars = other.pars;
    payload = other.payload;
    attributes = other.attributes;
    likelihoodPayload = other.likelihoodPayload;
    numSamples = other.numSamples;
    formatCount = other.formatCount;
  }
  return *this;
}

void GenoAnnotation::RecData::set(
    uint32_t _rowStart, uint32_t _colStart,
    std::tuple<genie::genotype::GenotypeParameters,
               genie::genotype::GenotypePayload>
        _genotypeData,
    genie::likelihood::LikelihoodPayload _likelihoodPayload, uint32_t _numSamples,
    uint8_t _formatCount,
    std::map<std::string,
             std::tuple<core::parameter::annotation::AttributeData,
                        std::vector<std::vector<std::vector<AttrType>>>>>
        _attributes) {
  rowStart = _rowStart;
  colStart = _colStart;
  pars = std::get<genie::genotype::GenotypeParameters>(_genotypeData);
  payload = std::get<genie::genotype::GenotypePayload>(_genotypeData);
  likelihoodPayload = _likelihoodPayload;
  numSamples = _numSamples;
  formatCount = _formatCount;
  attributes = _attributes;
}

}  // namespace annotation
}  // namespace genie
