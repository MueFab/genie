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
#include "genie/annotation/accessunit_composer.h"
#include "genie/variantsite/parameterset_composer.h"

#include "genie/annotation/compressors.h"
#include "genie/core/record/data_unit/record.h"

namespace genie {
namespace annotation {

struct SampleUnits {
  core::parameter::annotation::Record annotationParameterSet;
  std::vector<core::access_unit::annotation::Record> annotationAccessUnit;
};

class SampleAnnotation {
 public:
  void setTileSize(uint32_t _defaultTileSizeWidth) {
    defaultTileSizeWidth = _defaultTileSizeWidth;
  }

  void setATtype(core::access_unit::annotation::AnnotationType annotationType, uint8_t annotationSubtype) {
    annotationType_ = annotationType;
    annotationSubtype_ = annotationSubtype;
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

  variant_site::  AccessUnitComposer accessUnitcomposer;
  core::parameter::annotation::Record annotationParameterSet;
  std::vector<core::access_unit::annotation::Record> annotationAccessUnits;

  uint32_t defaultTileSizeWidth;
  uint64_t colIndex = 0;
  core::access_unit::annotation::AnnotationType annotationType_ =
      core::access_unit::annotation::AnnotationType::VARIANTS;
  uint8_t annotationSubtype_{1};
};

}  // namespace annotation
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_SAMPLE_ANNOTATION_H_
