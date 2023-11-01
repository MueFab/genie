#include "VariantSiteParser.h"
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <algorithm>
#include <string>
#include <utility>

#include "genie/core/constants.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"
#include "genie/variantsite/ParameterSetComposer.h"
// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {

genie::core::record::annotation_parameter_set::Record ParameterSetComposer::setParameterSet(
    std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData>& info,
    uint64_t defaultTileSize) {
    return setParameterSet(info, genie::genotype::GenotypeParameters{}, genie::core::AlgoID::BSC, defaultTileSize);
}

genie::core::record::annotation_parameter_set::Record ParameterSetComposer::setParameterSet(
    std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData>& info,
    genie::genotype::GenotypeParameters genotype_parameters, genie::core::AlgoID encodeMode, uint64_t defaultTileSize) {
    //----------------------------------------------------//
    // default values
    core::record::annotation_parameter_set::ParameterSettings defaultset;
    genie::core::AlgoID encoding_mode_ID = encodeMode;

    uint8_t AG_class = 1;
    uint64_t n_tiles = 1;

    std::vector<std::vector<uint64_t>> start_index;
    std::vector<std::vector<uint64_t>> end_index;
    std::vector<uint64_t> tile_size(1, defaultTileSize);

    std::vector<uint16_t> nAttributes;
    std::vector<std::vector<uint16_t>> attribute_IDs;
    std::vector<uint8_t> nDescriptors;
    std::vector<std::vector<uint8_t>> descriptor_IDs;
    genie::core::record::annotation_parameter_set::TileStructure default_tile_structure(
        defaultset.ATCoordSize, defaultset.tileParameterSettings.twoDimensional,
        defaultset.tileParameterSettings.variable_size_tiles, n_tiles, start_index, end_index, tile_size);
    std::vector<genie::core::record::annotation_parameter_set::TileStructure> additional_tile_structure;
    std::vector<genie::core::record::annotation_parameter_set::TileConfiguration> tile_configurations;

    genie::core::record::annotation_parameter_set::TileConfiguration tileConfiguration(
        defaultset.ATCoordSize, AG_class, defaultset.tileParameterSettings.attributeContiguity,
        defaultset.tileParameterSettings.twoDimensional, defaultset.tileParameterSettings.columnMajorTileOrder,
        defaultset.tileParameterSettings.symmetry_mode, defaultset.tileParameterSettings.symmetry_minor_diagonal,
        defaultset.tileParameterSettings.attribute_dependent_tiles, default_tile_structure,
        defaultset.tileParameterSettings.n_add_tile_structures, nAttributes, attribute_IDs, nDescriptors,
        descriptor_IDs, additional_tile_structure);

    tile_configurations.push_back(tileConfiguration);

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

    // -----------------------------------------------------------------------------------------
    uint8_t n_compressors = 3;

    genie::entropy::bsc::BSCParameters bscParameters;
    auto BSCalgorithmParameters = bscParameters.convertToAlgorithmParameters();

    genie::entropy::lzma::LZMAParameters lzmaParameters;
    auto LZMAalgorithmParameters = lzmaParameters.convertToAlgorithmParameters();

    genie::entropy::zstd::ZSTDParameters zstdParameters;
    auto ZSTDalgorithmParameters = zstdParameters.convertToAlgorithmParameters();

    std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet> compressor_parameter_set{
        bscParameters.compressorParameterSet(1), lzmaParameters.compressorParameterSet(2),
        zstdParameters.compressorParameterSet(3)};

    // -----------------------------------------------------------------------------------------

    // genie::genotype::GenotypeParameters genotype_parameters;
    genie::core::record::annotation_parameter_set::LikelihoodParameters likelihood_parameters;
    genie::core::record::annotation_parameter_set::ContactMatrixParameters contact_matrix_parameters;

    std::vector<genie::core::record::annotation_parameter_set::DescriptorConfiguration> descriptor_configuration;
    uint8_t n_descriptors = static_cast<uint8_t>(encodedDescriptors.size());
    for (auto it = encodedDescriptors.begin(); it != encodedDescriptors.end(); ++it) {
        genie::core::AnnotDesc DescrID = it->first;
        if (DescrID == genie::core::AnnotDesc::GENOTYPE) encoding_mode_ID = genie::core::AlgoID::LZMA;

        genie::core::record::annotation_parameter_set::DescriptorConfiguration descrConf(
            DescrID, encoding_mode_ID, genotype_parameters, likelihood_parameters, contact_matrix_parameters,
            BSCalgorithmParameters);

        descriptor_configuration.push_back(descrConf);
    }

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
        n_ontology_terms, ontology_term_name_len, ontology_term_name, n_descriptors, descriptor_configuration,
        n_compressors, compressor_parameter_set, n_attributes, attribute_parameter_set);

    genie::core::record::annotation_parameter_set::Record annotationParameterSet(
        defaultset.parameter_set_ID, defaultset.AT_ID, defaultset.AT_alphabet_ID, defaultset.ATCoordSize,
        defaultset.AT_pos_40_bits_flag, defaultset.n_aux_attribute_groups, tile_configurations,
        annotation_encoding_parameters);
    return annotationParameterSet;
}

}  // namespace variant_site
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
