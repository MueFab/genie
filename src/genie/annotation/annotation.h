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

#include "genotype_annotation.h"

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
    } 

    void setInfoFields(std::string jsonFileName);
    void startStream(RecType recType, std::string recordInputFileName, std::string outputFileName);

    void writeToFile(std::string& outputFileName);

    void parseInfoTags(std::string& recordInputFileName);

    void setLikelihoodOptions(genie::likelihood::EncodingOptions opt) { genotypeAnnotation.setLikelihoodOptions(opt); }
    void setGenotypeOptions(genie::genotype::EncodingOptions opt) { genotypeAnnotation.setGenotypeOptions(opt); }

 private:
    std::ifstream recordInput;
    genie::annotation::Compressor compressors;
    std::map<std::string, genie::core::record::variant_site::Info_tag> infoTags;
    std::map<std::string, InfoField> attributeInfo;
    std::vector<InfoField> infoFields;

    genie::variant_site::AccessUnitComposer accessUnitcomposer;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet;
    std::vector<genie::core::record::annotation_access_unit::Record> annotationAccessUnit;

    GenotypeAnnotation genotypeAnnotation;

    uint32_t defaultTileSizeHeight;
    uint32_t defaultTileSizeWidth;
    void parseSite(std::ifstream& inputfile);
};

}  // namespace annotation
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // GENIE_ANNOTATION_ANNOTATION_H
