/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FEATURE_VARIANTSAMPLE_PARSER_H_
#define SRC_GENIE_FEATURE_VARIANTSAMPLE_PARSER_H_

// ---------------------------------------------------------------------------------------------------------------------
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/constants.h"
#include "genie/variantsite/attributes.h"
#include "genie/variantsite/descriptors.h"

#include "genie/annotation/json_attribute_parser.h"
#include "genie/core/sample_record/record.h"
#include "genie/core/record/annotation_parameter_set/AttributeData.h"
#include "genie/core/record/annotation_parameter_set/AttributeParameterSet.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

namespace genie {
namespace variant_sample {

class VariantSampleParser {
 public:
  using AttributeData = core::record::annotation_parameter_set::AttributeData;
  using InfoField = annotation::InfoField;

  VariantSampleParser(std::istream& _samples, std::vector<annotation::InfoField>& _fields, uint32_t _rowsPerTile);

  size_t getNumberOfColumns() const {
    return numberOfColumns;
  }

  variant_site::Attributes& getAttributes() {
    return attributes;
  }

  uint64_t getNrOfTiles() {
    return attributes.getTiles().begin()->second.getNrOfTiles();
  }

 private:
  core::record::sample::Record sample;
  std::istream& sampleMGrecs;
  uint64_t rowsPerTile;
  size_t numberOfColumns;
  std::map<std::string, core::record::feature::Info_tag> tags;
  std::vector<InfoField> infoFields;
  std::vector<std::string> testAltern;
  std::map<std::string, uint8_t> infoFieldType;

  std::map<std::string, AttributeData> attributeData;
  variant_site::Attributes attributes;

  uint16_t numberOfAttributes;

  const std::string SAMPLE_NAME = "sample_ids";
  uint64_t startPos = 0;

  void init();
  bool fillRecord(util::BitReader reader);
};

}  // namespace variant_sample
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FEATURE_FEATURE_PARSER_H_

// ---------------------------------------------------------------------------------------------------------------------
