/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_ANNOTATION_PARAMETERSETCOMPOSER_H_
#define SRC_GENIE_ANNOTATION_PARAMETERSETCOMPOSER_H_

// ---------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "genie/core/arrayType.h"
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bit_reader.h"

#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/jbig/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"

#include "genie/annotation/compressors.h"
#include "genie/contact/contact_matrix_parameters.h"
#include "genie/genotype/genotype_coder.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/genotype/genotype_payload.h"
#include "genie/likelihood/likelihood_parameters.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace genotype {

class ParameterSetComposer {
 public:
    genie::core::record::annotation_parameter_set::Record Build(
        uint8_t _AT_ID, std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData>& info,
        std::vector<uint64_t> defaultTileSize = {200, 3000});

 
    void setGenotypeParameters(genie::genotype::GenotypeParameters _parameters) { genotypeParameters = _parameters; }
    [[maybe_unused]] void setContactParameters(genie::contact::ContactMatrixParameters _parameters) {
        contactMatrixParameters = _parameters;
    }
    [[maybe_unused]] void SetSubContactParameters(genie::contact::SubcontactMatrixParameters _parameters) {
        subContactMatrixParameters = _parameters;
    }
    void setLikelihoodParameters(genie::likelihood::LikelihoodParameters _parameters) {
        likelihoodParameters = _parameters;
    }
    /* void setContactMatrixParameters(genie::contact::ContactMatrixParameters _parameters) {
        contactMatrixParameters = _parameters;
    }
    */
    [[maybe_unused]] void setCompressors(genie::annotation::Compressor _compressors) {
        compressors = _compressors;
        useCompressors = true;
    }

 private:

    genie::genotype::GenotypeParameters genotypeParameters;
    genie::likelihood::LikelihoodParameters likelihoodParameters;
    genie::contact::ContactMatrixParameters contactMatrixParameters;
    genie::contact::SubcontactMatrixParameters subContactMatrixParameters;

    genie::annotation::Compressor compressors;
    bool useCompressors{ false };

    uint8_t parameter_set_ID;
    uint8_t AT_ID;
    genie::core::AlphabetID AT_alphabet_ID;
    uint8_t AT_coord_size;
    bool AT_pos_40_bits_flag;
    uint8_t n_aux_attribute_groups;
    bool two_dimensional;
    [[maybe_unused]] uint64_t n_tiles;
    uint8_t AG_class;

    std::vector<genie::core::record::annotation_parameter_set::TileConfiguration> tile_configuration;

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

}  // namespace genotype
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_ANNOTATION_PARAMETERSETCOMPOSER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
