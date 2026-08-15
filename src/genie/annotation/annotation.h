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
#include "genie/core/record/data_unit/record.h"
#include "genie/annotation/geno_annotation.h"
#include "genie/annotation/site_annotation.h"
#include "genie/annotation/sample_annotation.h"
#include "genie/annotation/feature_annotation.h"
#include "genie/annotation/gene_expression_annotation.h"
#include "genie/annotation/functional_annotation.h"
#include "genie/annotation/track_data_annotation.h"
#include "genie/annotation/trackproperty_annotation.h"
// -----------------------------------------------------------------------------

namespace genie::annotation {
// ---------------------------------------------------------------------------------------------------------------------

enum class RecType { VARIANT_SITE_FILE = 0, VARIANT_GENO_FILE, SAMPLE_FILE, FUNCTIONAL_ANNOTATION_FILE, TRACK_PROPERTY_FILE, TRACK_DATA_FILE, EXPRESSION_FILE, FEATURE_FILE, CONTACT_MATRIX_FILE };

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
        siteAnnotation.setTileSize(_defaultTileSizeHeight);
        genoAnnotation.setTileSize(_defaultTileSizeHeight, defaultTileSizeWidth);
        sampleAnnotation.setTileSize(_defaultTileSizeWidth);
        functionalAnnotation.setTileSize(_defaultTileSizeHeight);
        trackPropertyAnnotation.setTileSize(_defaultTileSizeHeight);
        trackDataAnnotation.setTileSize(_defaultTileSizeHeight);
        geneExpressionAnnotation.setTileSize(_defaultTileSizeHeight, _defaultTileSizeWidth);
        featureAnnotation.setTileSize(_defaultTileSizeHeight);
    }

    void setATType(core::access_unit::annotation::AnnotationType ATtype,
                   core::access_unit::annotation::AnnotationSubtype ATsubtype) {
        annotationType_ = ATtype;
        annotationSubtype_ = ATsubtype;
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

    std::vector<genie::core::parameter::annotation::Record>
        annotationParameterSet;
    std::vector<genie::core::access_unit::annotation::Record>
        annotationAccessUnit;

    SiteAnnotation siteAnnotation;
    GenoAnnotation genoAnnotation;
    SampleAnnotation sampleAnnotation;
    FunctionalAnnotation functionalAnnotation;
    TrackPropertyAnnotation trackPropertyAnnotation;
    TrackDataAnnotation trackDataAnnotation;
    GeneExpressionAnnotation geneExpressionAnnotation;
    FeatureAnnotation featureAnnotation;
    CMAnnotation cmAnnotation;

    uint32_t defaultTileSizeHeight{0};
    uint32_t defaultTileSizeWidth{0};
    core::access_unit::annotation::AnnotationType annotationType_{
        core::access_unit::annotation::AnnotationType::VARIANTS};
    core::access_unit::annotation::AnnotationSubtype annotationSubtype_{
        core::access_unit::annotation::AnnotationSubtype::VCF};
};

}  // namespace genie::annotation

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_ANNOTATION_H_
