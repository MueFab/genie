/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_GENOTYPE_PARAMETERSETCOMPOSER_H_
#define SRC_GENIE_GENOTYPE_PARAMETERSETCOMPOSER_H_

// ---------------------------------------------------------------------------------------------------------------------
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/arrayType.h"
#include "genie/core/constants.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"

#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/jbig/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"

#include "genie/genotype/genotype_coder.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/genotype/genotype_payload.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace genotype {

class ParameterSetComposer {
 public:
    genie::core::record::annotation_parameter_set::Record Build(genie::genotype::GenotypeParameters& genotypeParameters,
                                                                genie::genotype::EncodingOptions opt,
                                                                uint8_t _AT_ID,
                                                                uint64_t defaultTileSize = 100) {
        (void)opt;
        uint8_t parameter_set_ID = 1;
        uint8_t AT_ID = _AT_ID;
        genie::core::AlphabetID AT_alphabet_ID = genie::core::AlphabetID::ACGTN;
        genie::core::AnnotDesc descriptor_ID = genie::core::AnnotDesc::GENOTYPE;
        uint8_t AT_coord_size = 3;
        bool AT_pos_40_bits_flag = false;
        uint8_t n_aux_attribute_groups = 0;
        bool two_dimensional = false;
        bool variable_size_tiles = false;
        uint64_t n_tiles = 1;
        std::vector<std::vector<uint64_t>> start_index{0};
        std::vector<std::vector<uint64_t>> end_index{0};
        std::vector<uint64_t> tile_size{defaultTileSize};
        uint8_t AG_class = 0;
        bool attribute_contiguity = false;
        bool column_major_tile_order = false;
        uint8_t symmetry_mode = 0;
        bool symmetry_minor_diagonal = false;
        bool attribute_dependent_tiles = false;
        uint16_t n_add_tile_structures = 0;
        std::vector<uint16_t> n_attributes = {0};
        std::vector<std::vector<uint16_t>> attributeIDs{};
        std::vector<uint8_t> n_descriptors{1};
        std::vector<std::vector<uint8_t>> descriptorIDs{};

        genie::core::record::annotation_parameter_set::TileStructure defaultTileStructure(
            AT_coord_size, two_dimensional, variable_size_tiles, n_tiles, start_index, end_index, tile_size);

        std::vector<genie::core::record::annotation_parameter_set::TileStructure> additional_tile_structure{
            defaultTileStructure};

        std::vector<genie::core::record::annotation_parameter_set::TileConfiguration> tile_configuration{
            genie::core::record::annotation_parameter_set::TileConfiguration(
                AT_coord_size, AG_class, attribute_contiguity, two_dimensional, column_major_tile_order, symmetry_mode,
                symmetry_minor_diagonal, attribute_dependent_tiles, defaultTileStructure, n_add_tile_structures,
                n_attributes, attributeIDs, n_descriptors, descriptorIDs, additional_tile_structure)};

        //-------------------
        genie::entropy::lzma::LZMAParameters lzmaParameters;
        auto LZMAalgorithmParameters = lzmaParameters.convertToAlgorithmParameters();
        genie::entropy::jbig::JBIGparameters jbigParameters;
        auto JBIGalgorithmParameters = jbigParameters.convertToAlgorithmParameters();

        //-------------------
 
        std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet> compressor_parameter_set{
            addLZMACompressorParameterSet(1), addLZMACompressorParameterSet(2)};

        uint8_t n_filter = 0;
        std::vector<uint8_t> filter_ID_len;
        std::vector<std::string> filter_ID;
        std::vector<uint16_t> desc_len;
        std::vector<std::string> description;

        uint8_t n_features_names = 0;
        std::vector<uint8_t> feature_name_len;
        std::vector<std::string> feature_name;

        uint8_t n_ontology_terms = 0;
        std::vector<uint8_t> ontology_term_name_len;
        std::vector<std::string> ontology_term_name;

        uint8_t ndescriptors = 1;
        std::vector<genie::core::record::annotation_parameter_set::DescriptorConfiguration> descriptor_configuration{
            genie::core::record::annotation_parameter_set::DescriptorConfiguration(
                descriptor_ID, genie::core::AlgoID::LZMA, genotypeParameters, LZMAalgorithmParameters)};

        uint8_t n_compressors = static_cast<uint8_t>(compressor_parameter_set.size());

        uint8_t nattributes = 0;
        std::vector<genie::core::record::annotation_parameter_set::AttributeParameterSet> attribute_parameter_set;

        genie::core::record::annotation_parameter_set::AnnotationEncodingParameters annotation_encoding_parameters(
            n_filter, filter_ID_len, filter_ID, desc_len, description, n_features_names, feature_name_len, feature_name,
            n_ontology_terms, ontology_term_name_len, ontology_term_name, ndescriptors, descriptor_configuration,
            n_compressors, compressor_parameter_set, nattributes, attribute_parameter_set);
        // ------------------------------------
        genie::core::record::annotation_parameter_set::Record annotationParameterSet(
            parameter_set_ID, AT_ID, AT_alphabet_ID, AT_coord_size, AT_pos_40_bits_flag, n_aux_attribute_groups,
            tile_configuration, annotation_encoding_parameters);
        // ----------------------
        return annotationParameterSet;
    }

    genie::core::record::annotation_parameter_set::CompressorParameterSet addLZMACompressorParameterSet(
        uint8_t compressor_ID) {
        genie::entropy::lzma::LZMAParameters lzmaParameters;
        auto LZMAalgorithmParameters = lzmaParameters.convertToAlgorithmParameters();

        //-------------------
        std::vector<genie::core::AlgoID> LZMAalgorithm_ID{genie::core::AlgoID::LZMA};
        uint8_t n_compressor_steps = 1;
        std::vector<uint8_t> compressor_step_ID{0};
        std::vector<bool> use_default_pars{true};
        std::vector<genie::core::record::annotation_parameter_set::AlgorithmParameters> algorithm_parameters;
        std::vector<uint8_t> n_in_vars{0};
        std::vector<std::vector<uint8_t>> in_var_ID{{0}};
        std::vector<std::vector<uint8_t>> prev_step_ID;
        std::vector<std::vector<uint8_t>> prev_out_var_ID;
        std::vector<uint8_t> n_completed_out_vars{0};
        std::vector<std::vector<uint8_t>> completed_out_var_ID;

        return genie::core::record::annotation_parameter_set::CompressorParameterSet(
            compressor_ID, n_compressor_steps, compressor_step_ID, LZMAalgorithm_ID, use_default_pars,
            algorithm_parameters, n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars,
            completed_out_var_ID);
    }
    genie::core::record::annotation_parameter_set::CompressorParameterSet addJBIGCompressorParameterSet(
        uint8_t compressor_ID) {
        genie::entropy::jbig::JBIGparameters jbigParameters;
        auto LZMAalgorithmParameters = jbigParameters.convertToAlgorithmParameters();

        //-------------------
        std::vector<genie::core::AlgoID> JBIGalgorithm_ID{genie::core::AlgoID::JBIG};
        uint8_t n_compressor_steps = 1;
        std::vector<uint8_t> compressor_step_ID{0};
        std::vector<bool> use_default_pars{true};
        std::vector<genie::core::record::annotation_parameter_set::AlgorithmParameters> algorithm_parameters;
        std::vector<uint8_t> n_in_vars{0};
        std::vector<std::vector<uint8_t>> in_var_ID{{0}};
        std::vector<std::vector<uint8_t>> prev_step_ID;
        std::vector<std::vector<uint8_t>> prev_out_var_ID;
        std::vector<uint8_t> n_completed_out_vars{0};
        std::vector<std::vector<uint8_t>> completed_out_var_ID;

        return genie::core::record::annotation_parameter_set::CompressorParameterSet(
            compressor_ID, n_compressor_steps, compressor_step_ID, JBIGalgorithm_ID, use_default_pars,
            algorithm_parameters, n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars,
            completed_out_var_ID);
    }
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genotype
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_GENOTYPE_PARAMETERSETCOMPOSER_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
