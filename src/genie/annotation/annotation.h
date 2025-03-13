/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ANNOTATION_ANNOTATION_H_
#define SRC_GENIE_ANNOTATION_ANNOTATION_H_

#include <cstdint>
#include <fstream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "genie/annotation/contact_annotation.h"
#include "genie/annotation/compressors.h"
#include "genie/core/constants.h"
#include "genie/core/record/data_unit_record/record.h"
#include "genie/annotation/geno_annotation.h"
#include "genie/annotation/site_annotation.h"
// -----------------------------------------------------------------------------

namespace genie {
namespace annotation {
// ---------------------------------------------------------------------------------------------------------------------

enum class RecType { SITE_FILE = 0, GENO_FILE, CM_FILE };



// ---------------------------------------------------------------------------------------------------------------------

class Annotation {
 public:
  void setCompressorConfig(std::stringstream& config) {
    compressors.parseConfig(config);
  }
  void setTileSize(uint32_t _defaultTileSizeHeight,
                   uint32_t _defaultTileSizeWidth) {
    defaultTileSizeHeight = _defaultTileSizeHeight;
    defaultTileSizeWidth = _defaultTileSizeWidth;
    genoAnnotation.setTileSize(_defaultTileSizeHeight, defaultTileSizeWidth);
    siteAnnotation.setTileSize(_defaultTileSizeHeight);
  }

  void startStream(RecType recType, std::string recordInputFileName,
                   std::string outputFileName);

  void writeToFile(std::string& outputFileName);

  void setLikelihoodOptions(genie::likelihood::EncodingOptions opt) {
    genoAnnotation.setLikelihoodOptions(opt);
  }
  void setGenotypeOptions(genie::genotype::EncodingOptions opt) {
    genoAnnotation.setGenotypeOptions(opt);
  }

  void setContactOptions(ContactMatrixParameters options) {
    cmAnnotation.setContactOptions(options);
  }

 private:
  std::ifstream recordInput;
  genie::annotation::Compressor compressors;
  std::map<std::string, InfoField> attributeInfo;

  std::vector<genie::core::record::annotation_parameter_set::Record>
      annotationParameterSet;
  std::vector<genie::core::record::annotation_access_unit::Record>
      annotationAccessUnit;

  GenoAnnotation genoAnnotation;
  SiteAnnotation siteAnnotation;
  CMAnnotation cmAnnotation;

  uint32_t defaultTileSizeHeight{0};
  uint32_t defaultTileSizeWidth{0};
};

}  // namespace annotation
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_ANNOTATION_H_
