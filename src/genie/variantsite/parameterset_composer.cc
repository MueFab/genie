/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include "variantsite_parser.h"

#include <algorithm>
#include <string>
#include <utility>

#include "genie/core/constants.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

//#include "genie/contact/contact_matrix_parameters.h"
#include "genie/core/record/annotation_parameter_set/CompressorParameterSet.h"
#include "genie/core/record/annotation_parameter_set/TileConfiguration.h"
#include "genie/core/record/annotation_parameter_set/TileStructure.h"
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/lzma/encoder.h"
#include "genie/entropy/zstd/encoder.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/likelihood/likelihood_parameters.h"
#include "genie/variantsite/parameterset_composer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace variant_site {

genie::core::record::annotation_parameter_set::Record ParameterSetComposer::setParameterSet(
    std::vector<genie::core::AnnotDesc> descrList,
    std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData>& info,
    uint64_t defaultTileSize, uint8_t AT_ID) {
    genie::entropy::bsc::BSCParameters bscParameters;
    auto BSCalgorithmParameters = bscParameters.convertToAlgorithmParameters();

    genie::entropy::lzma::LZMAParameters lzmaParameters;
    auto LZMAalgorithmParameters = lzmaParameters.convertToAlgorithmParameters();

    genie::entropy::zstd::ZSTDParameters zstdParameters;
    auto ZSTDalgorithmParameters = zstdParameters.convertToAlgorithmParameters();

    std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet> compressor_parameter_set{
        bscParameters.compressorParameterSet(1), lzmaParameters.compressorParameterSet(2),
        zstdParameters.compressorParameterSet(3)};

    return setParameterSet(descrList, info, compressor_parameter_set, defaultTileSize,AT_ID);
}

genie::core::record::annotation_parameter_set::Record ParameterSetComposer::setParameterSet(
    std::vector<genie::core::AnnotDesc> descrList,
    std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData>& info,
    const std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet>& compressor_parameter_set,
    uint64_t defaultTileSize, uint8_t _AT_ID) {
    //----------------------------------------------------//
    core::record::annotation_parameter_set::ParameterSettings defaultset;
    defaultset.ATCoordSize = 3;
    defaultset.AG_class = 1;
    defaultset.AT_ID = _AT_ID;

    genie::core::record::annotation_parameter_set::TileConfiguration tileConfiguration(
        defaultset.ATCoordSize, defaultset.AG_class, defaultTileSize);
    std::vector<genie::core::record::annotation_parameter_set::TileConfiguration> tile_configurations;
    genie::core::AlgoID encoding_mode_ID = genie::core::AlgoID::BSC;

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
    uint8_t n_compressors = static_cast<uint8_t>(compressor_parameter_set.size());

    genie::entropy::bsc::BSCParameters bscParameters;
    auto BSCalgorithmParameters = bscParameters.convertToAlgorithmParameters();

    genie::entropy::lzma::LZMAParameters lzmaParameters;
    auto LZMAalgorithmParameters = lzmaParameters.convertToAlgorithmParameters();

    genie::entropy::zstd::ZSTDParameters zstdParameters;
    auto ZSTDalgorithmParameters = zstdParameters.convertToAlgorithmParameters();
    // -----------------------------------------------------------------------------------------

    std::vector<genie::core::record::annotation_parameter_set::DescriptorConfiguration> descriptor_configuration;
    uint8_t n_descriptors = static_cast<uint8_t>(descrList.size());
    for (auto descr : descrList) {
        genie::core::AnnotDesc DescrID = descr;
        if (DescrID == genie::core::AnnotDesc::STRAND)
            encoding_mode_ID = genie::core::AlgoID::LZMA;
        else if (DescrID == genie::core::AnnotDesc::REFERENCE)
            encoding_mode_ID = genie::core::AlgoID::ZSTD;
        else
            encoding_mode_ID = genie::core::AlgoID::BSC;

        descriptor_configuration.emplace_back(DescrID, encoding_mode_ID, BSCalgorithmParameters);
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

genie::core::record::annotation_parameter_set::Record ParameterSetComposer::setParameterSet(
    std::vector<genie::core::AnnotDesc> descrList,
    std::map<std::string, genie::core::record::annotation_parameter_set::AttributeData>& info,
    genie::annotation::Compressor compressors, uint64_t defaultTileSize) {
    // default values
    core::record::annotation_parameter_set::ParameterSettings defaultset;
    defaultset.ATCoordSize = 3;
    defaultset.AG_class = 1;
    genie::core::AlgoID encoding_mode_ID = genie::core::AlgoID::BSC;

    std::vector<genie::core::record::annotation_parameter_set::TileConfiguration> tile_configurations;

    genie::core::record::annotation_parameter_set::TileConfiguration tileConfiguration(
        defaultset.ATCoordSize, defaultset.AG_class, defaultTileSize);

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
    uint8_t n_compressors = static_cast<uint8_t>(compressors.getNrOfCompressorIDs());

    genie::entropy::bsc::BSCParameters bscParameters;
    auto BSCalgorithmParameters = bscParameters.convertToAlgorithmParameters();

    genie::entropy::lzma::LZMAParameters lzmaParameters;
    auto LZMAalgorithmParameters = lzmaParameters.convertToAlgorithmParameters();

    genie::entropy::zstd::ZSTDParameters zstdParameters;
    auto ZSTDalgorithmParameters = zstdParameters.convertToAlgorithmParameters();
    // -----------------------------------------------------------------------------------------

    std::vector<genie::core::record::annotation_parameter_set::DescriptorConfiguration> descriptor_configuration;
    uint8_t n_descriptors = static_cast<uint8_t>(descrList.size());
    for (auto descr : descrList) {
        genie::core::AnnotDesc DescrID = descr;
        if (DescrID == genie::core::AnnotDesc::STRAND)
            encoding_mode_ID = genie::core::AlgoID::LZMA;
        else if (DescrID == genie::core::AnnotDesc::REFERENCE)
            encoding_mode_ID = genie::core::AlgoID::ZSTD;
        else
            encoding_mode_ID = genie::core::AlgoID::BSC;

        descriptor_configuration.emplace_back(DescrID, encoding_mode_ID, BSCalgorithmParameters);
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
        n_compressors, compressors.getCompressorParameters(), n_attributes, attribute_parameter_set);

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
