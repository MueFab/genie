/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ANNOTATION_GENE_EXPRESSION_ANNOTATION_H_
#define SRC_GENIE_ANNOTATION_GENE_EXPRESSION_ANNOTATION_H_
// #include <fstream>
#include <map>
#include <string>
#include <tuple>
#include <vector>
#include "genie/annotation/compressors.h"
#include "genie/core/gene_expression_record/record.h"
#include "genie/core/parameter/annotation/attribute_data.h"
#include "genie/core/parameter/annotation/record.h"
#include "genie/core/access_unit/annotation/record.h"
#include "genie/core/access_unit/annotation/typed_data.h"
// #include "genie/core/data_unit_record/record.h"
// #include "genie/annotation/json_attribute_parser.h"
// #include "genie/gene_expression/gene_expression_parser.h"
// -----------------------------------------------------------------------------
namespace genie {
namespace annotation {
// ---------------------------------------------------------------------------------------------------------------------
struct GeneExpressionUnits {
    core::parameter::annotation::Record annotationParameterSet;
    std::vector<core::access_unit::annotation::Record> annotationAccessUnit;
};
class GeneExpressionAnnotation {
 public:
  using AttrType = std::vector<uint8_t>;

  class RecData {
   public:
    uint32_t rowStart;
    uint32_t colStart;
    genotype::GenotypeParameters pars;
    std::map<std::string, std::tuple<core::parameter::annotation::AttributeData,
                                     std::vector<std::vector<std::vector<AttrType>>>>>
        attributes;
    uint32_t numSamples;
    uint8_t formatCount;

    RecData();
    RecData(uint32_t _rowStart, uint32_t _colStart,
            uint32_t _numSamples,
            uint8_t _formatCount,
            std::map<std::string, std::tuple<core::parameter::annotation::AttributeData,
                                             std::vector<std::vector<std::vector<AttrType>>>>>
                attributes);

    RecData& operator=(const RecData& other);

    void set(uint32_t _rowStart, uint32_t _colStart,
             uint32_t _numSamples,
             uint8_t _formatCount,
             std::map<std::string, std::tuple<core::parameter::annotation::AttributeData,
                                              std::vector<std::vector<std::vector<AttrType>>>>>
                 _attributes);
  };

  void setCompressorConfig(std::stringstream& config) { compressors.parseConfig(config); }

  std::vector<GeneExpressionUnits> parseGeneExpression(std::ifstream& inputfile);
  void setCompressors(Compressor& _compressors) { compressors = _compressors; }
  void setTileSize(uint32_t _defaultTileSizeHeight, uint32_t _defaultTileSizeWidth) {
      defaultTileSizeHeight = _defaultTileSizeHeight;
      defaultTileSizeWidth = _defaultTileSizeWidth;
  }

 private:
    Compressor compressors;
    core::parameter::annotation::Record annotationParameterSet;
    std::vector<core::access_unit::annotation::Record> annotationAccessUnit;

    uint32_t defaultTileSizeHeight;
    uint32_t defaultTileSizeWidth;
    // gene_expression::GeneExpressionParser geneExpressionParser;

    std::map<std::string, core::parameter::annotation::AttributeData> attrInfo;
    std::map<std::string, std::vector<std::vector<std::vector<AttrType>>>> attrValues;

    void sort_format(std::vector<core::record::gene_expression::Record>& recs);
    struct ParsBlocks {
      std::vector<RecData> blocks;
      uint32_t rows;
    };
    size_t readBlocks(std::ifstream& inputfile, const uint32_t& rowTileSize,
                      std::vector<ParsBlocks>& blocksWPars);

    size_t readOneBlock(util::BitReader& reader, const uint32_t& rowTileSize,
                        RecData& recData);
};
}  // namespace annotation
}  // namespace genie
// -----------------------------------------------------------------------------
#endif  // SRC_GENIE_ANNOTATION_GENE_EXPRESSION_ANNOTATION_H_
