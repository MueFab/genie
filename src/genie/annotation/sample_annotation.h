/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ANNOTATION_SAMPLE_ANNOTATION_H_
#define SRC_GENIE_ANNOTATION_SAMPLE_ANNOTATION_H_

#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "genie/annotation/json_attribute_parser.h"
#include "genie/core/constants.h"
#include "genie/variantsample/variantsample_parser.h"
#include "genie/variantsite/accessunit_composer.h"
#include "genie/variantsite/parameterset_composer.h"

#include "genie/annotation/compressors.h"
#include "genie/core/data_unit_record/record.h"

namespace genie {
namespace annotation {

struct SampleUnits {
  core::record::annotation_parameter_set::Record annotationParameterSet;
  std::vector<core::record::annotation_access_unit::Record> annotationAccessUnit;
};

class SampleAnnotation {
 public:
  void setTileSize(uint32_t _defaultTileSizeWidth) {
    defaultTileSizeWidth = _defaultTileSizeWidth;
  }

  void parseInfoTags(std::string& recordInputFileName);
  SampleUnits parseSample(std::ifstream& inputfile);
  void setCompressors(annotation::Compressor& _compressors) {
    compressors = _compressors;
  }

 private:
  variant_site::Attributes attributes;
  Compressor compressors;
  std::map<std::string, core::record::sample::Info_tag> infoTags;
  std::map<std::string, InfoField> attributeInfo;
  std::vector<InfoField> infoFields;

  variant_site::AccessUnitComposer accessUnitcomposer;
  core::record::annotation_parameter_set::Record annotationParameterSet;
  std::vector<core::record::annotation_access_unit::Record> annotationAccessUnits;

  uint32_t defaultTileSizeWidth;
  uint64_t colIndex = 0;
};

}  // namespace annotation
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_SAMPLE_ANNOTATION_H_
