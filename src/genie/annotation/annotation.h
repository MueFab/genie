/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_ANNOTATION_ANNOTATION_H
#define GENIE_ANNOTATION_ANNOTATION_H

#include <cstdint>
#include <fstream>
#include <list>
#include <map>
#include <optional>
#include <sstream>
#include <tuple>
#include "genie/annotation/JsonAttributeParser.h"
#include "genie/core/constants.h"
#include "genie/variantsite/AccessUnitComposer.h"
#include "genie/variantsite/ParameterSetComposer.h"
#include "genie/variantsite/VariantSiteParser.h"

#include "geno_annotation.h"
#include "site_annotation.h"

#include "genie/annotation/Compressors.h"
#include "genie/core/record/data_unit/record.h"
// -----------------------------------------------------------------------------

namespace genie {
namespace annotation {
// ---------------------------------------------------------------------------------------------------------------------

enum class RecType { SITE_FILE = 0, GENO_FILE };

// ---------------------------------------------------------------------------------------------------------------------

class Annotation {
 public:
    void setCompressorConfig(std::stringstream& config) { compressors.parseConfig(config); }
    void setTileSize(uint32_t _defaultTileSizeHeight, uint32_t _defaultTileSizeWidth) {
        defaultTileSizeHeight = _defaultTileSizeHeight;
        defaultTileSizeWidth = _defaultTileSizeWidth;
        genoAnnotation.setTileSize(_defaultTileSizeHeight, defaultTileSizeWidth);
        siteAnnotation.setTileSize(_defaultTileSizeHeight);
    }

  //  void setInfoFields(std::string jsonFileName);
    void startStream(RecType recType, std::string recordInputFileName, std::string outputFileName);

    void writeToFile(std::string& outputFileName);

    void setLikelihoodOptions(genie::likelihood::EncodingOptions opt) { genoAnnotation.setLikelihoodOptions(opt); }
    void setGenotypeOptions(genie::genotype::EncodingOptions opt) { genoAnnotation.setGenotypeOptions(opt); }

 private:
    std::ifstream recordInput;
    genie::annotation::Compressor compressors;
    std::map<std::string, InfoField> attributeInfo;

    genie::variant_site::AccessUnitComposer accessUnitcomposer;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet;
    std::vector<genie::core::record::annotation_access_unit::Record> annotationAccessUnit;

    GenoAnnotation genoAnnotation;
    SiteAnnotation siteAnnotation;

    uint32_t defaultTileSizeHeight{0};
    uint32_t defaultTileSizeWidth{0};
};

}  // namespace annotation
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // GENIE_ANNOTATION_ANNOTATION_H
