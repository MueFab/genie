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

#include "genie/core/record/variant_genotype/record.h"
#include "genie/genotype/ParameterSetComposer.h"
#include "genie/genotype/genotype_coder.h"
#include "genie/likelihood/likelihood_coder.h"
#include "genie/likelihood/likelihood_payload.h"

#include "genie/core/record/data_unit/record.h"
#include "genie/annotation/Compressors.h"
 // -----------------------------------------------------------------------------

namespace genie {
namespace annotation {
// ---------------------------------------------------------------------------------------------------------------------

enum class RecType { SITE_FILE = 0, GENO_FILE };

class Descriptors {};

// ---------------------------------------------------------------------------------------------------------------------

class Annotation {
 public:

     void setCompressorConfig(std::stringstream& config) { compressors.parseConfig(config); }
     void setInfoFields(std::string jsonFileName);
     void startStream(RecType recType, std::string recordInputFileName, std::string attributeJsonFileName, std::string outputFileName);

 private:
    std::ifstream recordInput;
    genie::annotation::Compressor compressors;
    std::vector<genie::annotation::InfoField> infoFields;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet;
    genie::variant_site::AccessUnitComposer accessUnitcomposer;
    std::vector<genie::core::record::annotation_access_unit::Record> annotationAccessUnit;


    void parseGenotype(std::ifstream& inputfile);
    void parseSite(std::ifstream& inputfile);
};

}  // namespace annotation
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // GENIE_ANNOTATION_ANNOTATION_H
