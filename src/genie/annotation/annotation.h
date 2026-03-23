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
#include "genie/core/data_unit_record/record.h"
#include "genie/core/record/annotation_parameter_set/AttributeData.h"
#include "genie/annotation/geno_annotation.h"
#include "genie/annotation/site_annotation.h"
#include "genie/annotation/sample_annotation.h"
#include "genie/annotation/gene_expression_annotation.h"
#include "genie/annotation/feature_annotation.h"
#include "genie/annotation/functional_annotation.h"
#include "genie/annotation/track_annotation.h"
// -----------------------------------------------------------------------------

namespace genie {
namespace annotation {
// ---------------------------------------------------------------------------------------------------------------------

enum class RecType { SITE_FILE = 0, GENO_FILE, SAMPLE_FILE, FEATURE_FILE, GENE_EXPRESSION_FILE, CM_FILE, FUNCTIONAL_ANNOTATIONS_FILE, TRACK_FILE };

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
        geneExpressionAnnotation.setTileSize(_defaultTileSizeHeight, defaultTileSizeWidth);
        featureAnnotation.setTileSize(_defaultTileSizeHeight);
        functionalAnnotation.setTileSize(_defaultTileSizeHeight);
        trackAnnotation.setTileSize(_defaultTileSizeHeight);
    }

    void setATType(core::record::annotation_access_unit::AnnotationType ATtype,
                   core::record::annotation_access_unit::AnnotationSubtype ATsubtype) {
      annotationType_ = ATtype;
      annotationSubtype_ = ATsubtype;
    }

    void startStream(RecType recType, std::string recordInputFileName,
                     std::string outputFileName);

    void writeToFile(std::string& outputFileName);

    void setLikelihoodOptions(likelihood::EncodingOptions opt) {
        genoAnnotation.setLikelihoodOptions(opt);
    }
    void setGenotypeOptions(genotype::EncodingOptions opt) {
        genoAnnotation.setGenotypeOptions(opt);
    }

    void setContactOptions(ContactMatrixParameters options) {
        cmAnnotation.setContactOptions(options);
    }

 private:
    std::ifstream recordInput;
    Compressor compressors;
    std::map<std::string, InfoField> attributeInfo;

    std::vector<core::record::annotation_parameter_set::Record>
        annotationParameterSet;
    std::vector<core::record::annotation_access_unit::Record>
        annotationAccessUnit;

    GenoAnnotation genoAnnotation;
    SiteAnnotation siteAnnotation;
    SampleAnnotation sampleAnnotation;
    GeneExpressionAnnotation geneExpressionAnnotation;
    FeatureAnnotation featureAnnotation;
    TrackAnnotation trackAnnotation;
    FunctionalAnnotation functionalAnnotation;
    CMAnnotation cmAnnotation;

    uint32_t defaultTileSizeHeight{0};
    uint32_t defaultTileSizeWidth{0};
    core::record::annotation_access_unit::AnnotationType annotationType_{
        core::record::annotation_access_unit::AnnotationType::VARIANTS};
    core::record::annotation_access_unit::AnnotationSubtype annotationSubtype_{
        core::record::annotation_access_unit::AnnotationSubtype::VCF};
};

}  // namespace annotation
}  // namespace genie

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_ANNOTATION_H_
