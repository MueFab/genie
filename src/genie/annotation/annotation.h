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
#include "genie/contact/contact_coder.h"
#include "genie/likelihood/likelihood_coder.h"
#include "genie/likelihood/likelihood_payload.h"
#include "genie/contact/contact_parameters.h"
#include "genie/contact/contact_scm_payload.h"


#include "genie/annotation/Compressors.h"
#include "genie/core/record/data_unit/record.h"
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
    void startStream(RecType recType, std::string recordInputFileName, std::string attributeJsonFileName,
                     std::string outputFileName);

    void setLikelihoodOptions(genie::likelihood::EncodingOptions opt) { likelihood_opt = opt; }
    void setGenotypeOptions(genie::genotype::EncodingOptions opt) { genotype_opt = opt; }

 private:
    std::ifstream recordInput;
    genie::annotation::Compressor compressors;
    std::map<std::string, genie::core::record::variant_site::Info_tag> infoTags;
    std::map<std::string, InfoField> attributeInfo;
    std::vector<InfoField> infoFields;
    ;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet;
    genie::variant_site::AccessUnitComposer accessUnitcomposer;
    std::vector<genie::core::record::annotation_access_unit::Record> annotationAccessUnit;

    genie::likelihood::EncodingOptions likelihood_opt{200, true};
    genie::genotype::EncodingOptions genotype_opt{200,                                         // block size
                                                  genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
                                                  genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
                                                  false,                                       // transpose_mat;
                                                  genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
                                                  genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
                                                  genie::core::AlgoID::JBIG};

    void parseGenotype(std::ifstream& inputfile);
    void parseSite(std::ifstream& inputfile);
};

}  // namespace annotation
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // GENIE_ANNOTATION_ANNOTATION_H
