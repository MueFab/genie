/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>

#include "genie/core/constants.h"
#include "genie/genotype/ParameterSetComposer.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace genotype {
genie::core::record::annotation_parameter_set::Record ParameterSetComposer::Build(
    uint8_t _AT_ID, std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData>& info,
    genie::genotype::GenotypeParameters& genotypeParameters,
    genie::likelihood::LikelihoodParameters likelihoodParameters, uint64_t defaultTileSize) {
    uint8_t parameter_set_ID = 1;
    uint8_t AT_ID = _AT_ID;
    genie::core::AlphabetID AT_alphabet_ID = genie::core::AlphabetID::ACGTN;
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
    std::vector<uint16_t> nAttributes = {0};
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
            nAttributes, attributeIDs, n_descriptors, descriptorIDs, additional_tile_structure)};

    //-------------------
    genie::entropy::lzma::LZMAParameters lzmaParameters;
    auto LZMAalgorithmParameters = lzmaParameters.convertToAlgorithmParameters();
    genie::entropy::jbig::JBIGparameters jbigParameters;
    auto JBIGalgorithmParameters = jbigParameters.convertToAlgorithmParameters();
    genie::entropy::bsc::BSCParameters bscParameters;
    auto BSCalgorithmParameters = bscParameters.convertToAlgorithmParameters();

    //-------------------

    std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet> compressor_parameter_set{
        lzmaParameters.compressorParameterSet(1), jbigParameters.compressorParameterSet(2),
        bscParameters.compressorParameterSet(3)};

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

    uint8_t ndescriptors = 2;
    genie::core::AnnotDesc descriptor_ID = genie::core::AnnotDesc::GENOTYPE;
    std::vector<genie::core::record::annotation_parameter_set::DescriptorConfiguration> descriptor_configuration{
        genie::core::record::annotation_parameter_set::DescriptorConfiguration(descriptor_ID, genie::core::AlgoID::LZMA,
                                                                               genotypeParameters, likelihoodParameters,
                                                                               LZMAalgorithmParameters)};

    descriptor_ID = genie::core::AnnotDesc::LIKELIHOOD;
    descriptor_configuration.emplace_back(genie::core::record::annotation_parameter_set::DescriptorConfiguration(
        descriptor_ID, genie::core::AlgoID::LZMA, genotypeParameters, likelihoodParameters, BSCalgorithmParameters));

    uint8_t n_compressors = static_cast<uint8_t>(compressor_parameter_set.size());

    std::vector<genie::core::record::annotation_parameter_set::AttributeParameterSet> attribute_parameter_set;
    uint8_t n_attributes = static_cast<uint8_t>(info.size());
    for (auto it = info.begin(); it != info.end(); ++it) {
        genie::core::ArrayType deftype;
        uint16_t attrID = info[it->first].getAttributeID();
        std::string attribute_name = info[it->first].getAttributeName();
        uint8_t attribute_name_len = static_cast<uint8_t>(attribute_name.length());
        genie::core::DataType attribute_type = info[it->first].getAttributeType();
        uint8_t attribute_num_array_dims = info[it->first].getArrayLength();
        std::vector<uint8_t> attribute_array_dims;
        if (info[it->first].getArrayLength() == 1) {
            attribute_num_array_dims = 0;
        } else {
            attribute_num_array_dims = 1;
            attribute_array_dims.push_back(info[it->first].getArrayLength());
        }
        std::vector<uint8_t> attribute_default_val =
            deftype.toArray(attribute_type, deftype.getDefaultValue(attribute_type));
        bool attribute_miss_val_flag = true;
        bool attribute_miss_default_flag = false;
        std::vector<uint8_t> attribute_miss_val = attribute_default_val;
        std::string attribute_miss_str = "";
        uint8_t n_steps_with_dependencies = 0;
        std::vector<uint8_t> dependency_step_ID;
        std::vector<uint8_t> n_dependencies;
        std::vector<std::vector<uint8_t>> dependency_var_ID;
        std::vector<std::vector<bool>> dependency_is_attribute;
        std::vector<std::vector<uint16_t>> dependency_ID;

        genie::core::record::annotation_parameter_set::AttributeParameterSet attributeParameterSet(
            attrID, attribute_name_len, attribute_name, attribute_type, attribute_num_array_dims, attribute_array_dims,
            attribute_default_val, attribute_miss_val_flag, attribute_miss_default_flag, attribute_miss_val,
            attribute_miss_str, 1, n_steps_with_dependencies, dependency_step_ID, n_dependencies, dependency_var_ID,
            dependency_is_attribute, dependency_ID);
        attribute_parameter_set.push_back(attributeParameterSet);
    }

    genie::core::record::annotation_parameter_set::AnnotationEncodingParameters annotation_encoding_parameters(
        n_filter, filter_ID_len, filter_ID, desc_len, description, n_features_names, feature_name_len, feature_name,
        n_ontology_terms, ontology_term_name_len, ontology_term_name, ndescriptors, descriptor_configuration,
        n_compressors, compressor_parameter_set, n_attributes, attribute_parameter_set);
    // ------------------------------------
    genie::core::record::annotation_parameter_set::Record annotationParameterSet(
        parameter_set_ID, AT_ID, AT_alphabet_ID, AT_coord_size, AT_pos_40_bits_flag, n_aux_attribute_groups,
        tile_configuration, annotation_encoding_parameters);
    // ----------------------
    return annotationParameterSet;
}

}  // namespace genotype
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
