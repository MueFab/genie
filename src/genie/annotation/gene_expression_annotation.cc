/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/annotation/gene_expression_annotation.h"
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>
#include "genie/annotation/annotation_encoder.h"
#include "genie/annotation/parameterset_composer.h"
#include "genie/variantsite/accessunit_composer.h"

// "genie/util/runtime_exception.h"
// ---------------------------------------------------------------------------------------------------------------------
namespace genie {
namespace annotation {
std::vector<GeneExpressionUnits> GeneExpressionAnnotation::parseGeneExpression(
    std::ifstream& inputfile) {

    uint8_t AG_class = 0;
    uint8_t AT_ID = 1;

    std::vector<GeneExpressionUnits> dataunits;

    std::vector<ParsBlocks> blocksWPars;
    auto numberofRows = readBlocks(inputfile, defaultTileSizeHeight, blocksWPars);
    GeneExpressionUnits dataunit;
    ParsBlocks combined;
    combined = blocksWPars.at(0);
    //--------------

    for (auto i = 1; i < blocksWPars.size(); ++i) {
      combined.blocks.push_back(blocksWPars.at(i).blocks.at(0));
    }
    std::map<std::string, core::record::annotation_parameter_set::AttributeData> attributeInfo;
    for (auto& attr : combined.blocks.at(0).attributes)
      attributeInfo[attr.first] = std::get<0>(attr.second);

    AnnotationEncoder encodingPars;
    entropy::bsc::BSCParameters bscParameters;
    auto BSCalgorithmParameters = bscParameters.convertToAlgorithmParameters();

    encodingPars.setDescriptorParameters(core::AnnotDesc::LINKID, core::AlgoID::BSC,
                                         BSCalgorithmParameters);
    encodingPars.setCompressors(compressors);
    encodingPars.setAttributes(attributeInfo);
    auto annotationEncodingParameters = encodingPars.Compose();

    ParameterSetComposer parameterset;

    dataunit.annotationParameterSet =
        parameterset.Compose(AT_ID, AG_class, {defaultTileSizeHeight, defaultTileSizeWidth},
                             annotationEncodingParameters);

    dataunit.annotationAccessUnit.resize(combined.blocks.size());

    //----------------
    uint32_t blockIndex = 0;
    for (auto& parWBlocks : blocksWPars) {
      std::cerr << " blockIndex: " << std::to_string(blockIndex) << std::endl;

      size_t linkIdRowCnt = 0;
      std::map<std::string, core::record::annotation_access_unit::TypedData>
          attributeTDStream;
      std::cerr << " attributeTDStream... " << std::endl;
      for (auto& formatdata : combined.blocks.at(blockIndex).attributes) {
        auto& info =
            std::get<core::record::annotation_parameter_set::AttributeData>(formatdata.second);
        auto& values = std::get<1>(formatdata.second);
        // .genotypeDatablock.attributeInfo[formatdata.first];
        std::vector<uint32_t> arrayDims;
        arrayDims.push_back(static_cast<uint32_t>(values.size()));
        arrayDims.push_back(combined.blocks.at(blockIndex).numSamples);
        arrayDims.push_back(info.getArrayLength());

        attributeTDStream[formatdata.first].set(info.getAttributeType(),
                                                static_cast<uint8_t>(arrayDims.size()), arrayDims);
        attributeTDStream[formatdata.first].convertToTypedData(
            std::get<std::vector<std::vector<std::vector<AttrType>>>>(formatdata.second));
      }

      std::map<core::AnnotDesc, std::stringstream> descriptorStream;

      variant_site::AccessUnitComposer accessUnitcomposer;
      accessUnitcomposer.setATtype(core::record::annotation_access_unit::AnnotationType::GENE_EXPRESSION,
          core::record::annotation_access_unit::AnnotationSubtype::GENE_EXPRESSION);

      accessUnitcomposer.setCompressors(compressors);

      // add LINK_ID default values
      std::cerr << " add link values... " << std::endl;
      for (auto j = 0u; j < defaultTileSizeHeight && linkIdRowCnt < parWBlocks.rows;
           ++j, ++linkIdRowCnt) {
        const char val = '\xFF';
        descriptorStream[core::AnnotDesc::LINKID].write(&val, 1);
      }

      accessUnitcomposer.setAccessUnit(descriptorStream, attributeTDStream, attributeInfo,
                                       dataunit.annotationParameterSet,
                                       dataunit.annotationAccessUnit.at(blockIndex), AG_class,
                                       AT_ID, blockIndex, combined.blocks.at(blockIndex).colStart);
      blockIndex++;
    }
    dataunits.push_back(dataunit);

    return dataunits;
}

size_t GeneExpressionAnnotation::readBlocks(std::ifstream& inputfile, const uint32_t& rowTileSize,
                                  std::vector<ParsBlocks>& blocksWPars) {
  size_t TotalnumberOfRows = 0;

  util::BitReader bitreader(inputfile);

  while (bitreader.IsStreamGood()) {
    RecData oneTileData;
    auto rowsInBlock =
        readOneBlock(bitreader, rowTileSize, oneTileData);
    if (rowsInBlock == 0)
      return TotalnumberOfRows;

    ParsBlocks parWBlock;
    parWBlock.blocks.push_back(oneTileData);
    parWBlock.rows = static_cast<uint32_t>(rowsInBlock);
    blocksWPars.emplace_back(parWBlock);

    TotalnumberOfRows += rowsInBlock;

    if (rowsInBlock < rowTileSize)
      break;
  }
  return TotalnumberOfRows;
}

size_t GeneExpressionAnnotation::readOneBlock(
    util::BitReader& reader, const uint32_t& rowTileSize,
    RecData& recData) {
  // read rowTileSize of rows
  std::vector<core::record::gene_expression::Record> varGenoType;
  while (reader.IsStreamGood() && varGenoType.size() < rowTileSize) {
    varGenoType.emplace_back(reader);
    if (!reader.IsStreamGood())
      varGenoType.pop_back();
  }
  if (varGenoType.empty())
    return 0;
  // extract format fields
  std::map<std::string, core::record::gene_expression::ExpressionAttribute> geneExpressionAttributeList;
  for (auto& rec : varGenoType)
    for (const auto& field : rec.GetExpressionAttributes()) {
      geneExpressionAttributeList[field.GetAttrName()] = field;
      core::ArrayType convertArray;
      auto defaultValue =
          convertArray.toArray(field.GetAttrType(), convertArray.getDefaultValue(field.GetAttrType()));
      std::vector<std::vector<std::vector<uint8_t>>> exprAttrValue(
          rec.GetSampleCount(),
          std::vector<std::vector<uint8_t>>(field.GetAttrArrayLen(), defaultValue));
      geneExpressionAttributeList[field.GetAttrName()].SetAttrValues(exprAttrValue);
    }

  // fill every missing value of format field
  for (auto& rec : varGenoType) {
    auto& exprAttrs = rec.GetExpressionAttributes();
    for (const auto& availableFormats : geneExpressionAttributeList) {
      bool available = false;
      for (auto& currentExprAttr : exprAttrs)
        if (currentExprAttr.GetAttrName() == availableFormats.first) {
          available = true;
          break;
        }
      if (!available)
        exprAttrs.push_back(availableFormats.second);
    }
  }

  genotype::GenotypeParameters pars;

  uint32_t _numSamples = varGenoType.front().GetSampleCount();
  uint8_t _formatCount = varGenoType.front().GetExprAttrCount();
  uint32_t rowStart = static_cast<uint32_t>(varGenoType.front().GetFeatureIndex());

  sort_format(varGenoType);
  std::map<std::string, std::tuple<core::record::annotation_parameter_set::AttributeData,
                                   std::vector<std::vector<std::vector<AttrType>>>>>
      attributes;
  for (auto& attr : attrInfo)
    attributes[attr.first] = std::make_tuple(attr.second, attrValues[attr.first]);

  recData.set(rowStart, 0,
              _numSamples, _formatCount,
              attributes);

  return varGenoType.size();
}

void GeneExpressionAnnotation::sort_format(
    std::vector<core::record::gene_expression::Record>& recs) {
  // starting number
  uint8_t AttributeID = 25;

  uint32_t genotypeBlockSize = static_cast<uint32_t>(recs.size());
  // fill all attribute data
  for (const auto& format : recs.at(0).GetExpressionAttributes()) {
    const auto& formatName = format.GetAttrName();
    core::record::annotation_parameter_set::AttributeData attrData(
        formatName.size(), formatName, format.GetAttrType(), format.GetAttrArrayLen(), AttributeID);
    attrInfo[formatName] = attrData;
    AttributeID++;
  }

  // add values
  for (auto i_rec = 0u; i_rec < genotypeBlockSize; i_rec++) {
    auto& rec = recs[i_rec];
    for (const auto& format : rec.GetExpressionAttributes()) {
      auto formatName = format.GetAttrName();
      attrValues[formatName].resize(genotypeBlockSize);
      std::vector<std::vector<AttrType>> formatValue = format.GetAttrValues();
      attrValues[formatName].at(i_rec) = formatValue;
    }
  }
}

GeneExpressionAnnotation::RecData::RecData()
    : rowStart(0),
      colStart(0),
      attributes{},
      numSamples(0),
      formatCount(0) {}

GeneExpressionAnnotation::RecData::RecData(
    uint32_t _rowStart, uint32_t _colStart,
    uint32_t _numSamples,
    uint8_t _formatCount,
    std::map<std::string, std::tuple<core::record::annotation_parameter_set::AttributeData,
                                     std::vector<std::vector<std::vector<AttrType>>>>>
        attributes)
    : rowStart(_rowStart),
      colStart(_colStart),
      numSamples(_numSamples),
      formatCount(_formatCount) {}

GeneExpressionAnnotation::RecData& GeneExpressionAnnotation::RecData::operator=(
    const RecData& other) {
  rowStart = other.rowStart;
  colStart = other.colStart;
  pars = other.pars;
  attributes = other.attributes;
  numSamples = other.numSamples;
  return *this;
}

void GeneExpressionAnnotation::RecData::set(
    uint32_t _rowStart, uint32_t _colStart,
    uint32_t _numSamples,
    uint8_t _formatCount,
    std::map<std::string, std::tuple<core::record::annotation_parameter_set::AttributeData,
                                     std::vector<std::vector<std::vector<AttrType>>>>>
        _attributes) {
  rowStart = _rowStart;
  colStart = _colStart;
  numSamples = _numSamples;
  formatCount = _formatCount;
  attributes = _attributes;
}

}  // namespace annotation
}  // namespace genie
// ---------------------------------------------------------------------------------------------------------------------
