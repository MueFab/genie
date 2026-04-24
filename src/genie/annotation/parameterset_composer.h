/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ANNOTATION_PARAMETERSET_COMPOSER_H_
#define SRC_GENIE_ANNOTATION_PARAMETERSET_COMPOSER_H_

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
#include "genie/core/parameter/annotation/record.h"
#include "genie/core/parameter/annotation/attribute_data.h"
#include "genie/core/parameter/annotation/annotation_encoding_parameters.h"
#include "genie/annotation/compressors.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace annotation {

class ParameterSetComposer {
 public:
    genie::core::parameter::annotation::Record Compose(
        uint8_t AT_ID, uint8_t AG_class, std::pair<uint64_t, uint64_t> tile_Size,
        core::parameter::annotation::AnnotationEncodingParameters& annotationEncodingpars);

    genie::core::parameter::annotation::Record Build(
        uint8_t _AT_ID, std::map<std::string, genie::core::parameter::annotation::AttributeData>& info,
        std::vector<uint64_t> defaultTileSize);

    void setGenotypeParameters(genotype::GenotypeParameters _parameters) { genotypeParameters = _parameters; }
    void setLikelihoodParameters(likelihood::LikelihoodParameters _parameters) { likelihoodParameters = _parameters; }

 private:
    uint8_t parameter_set_ID;
    uint8_t AT_ID;
    core::AlphabetId AT_alphabet_ID;
    uint8_t AT_coord_size;
    bool AT_pos_40_bits_flag;
    uint8_t n_aux_attribute_groups;
    bool two_dimensional;
    uint16_t n_tiles;
    uint8_t AG_class;
    std::vector<genie::core::parameter::annotation::TileConfiguration> tile_configuration;
    bool useCompressors = false;
    Compressor compressors;
    genotype::GenotypeParameters genotypeParameters;
    likelihood::LikelihoodParameters likelihoodParameters;

    uint8_t n_filter;
    std::vector<uint8_t> filter_ID_len;
    std::vector<std::string> filter_ID;
    std::vector<uint16_t> desc_len;
    std::vector<std::string> description;

    uint8_t n_features_names;
    std::vector<uint8_t> feature_name_len;
    std::vector<std::string> feature_name;

    uint8_t n_ontology_terms;
    std::vector<uint8_t> ontology_term_name_len;
    std::vector<std::string> ontology_term_name;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace annotation
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_PARAMETERSET_COMPOSER_H_
