
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "RandomRecordFillIn.h"

// ---------------------------------------------------------------------------------------------------------------------

genie::core::record::annotation_parameter_set::LikelihoodParameters
RandomAnnotationEncodingParameters::randomLikelihood() {
    uint8_t num_gl_per_sample = randomU8();
    bool transform_flag = randomBool();
    uint8_t dtype_id = randomU8();
    return genie::core::record::annotation_parameter_set::LikelihoodParameters(num_gl_per_sample, transform_flag,
                                                                               dtype_id);
}


genie::core::record::annotation_parameter_set::AttributeParameterSet
RandomAnnotationEncodingParameters::randomAttributeParameterSet() {
    uint16_t attribute_ID = randomU16();
    uint8_t attribute_name_len = randomU8();
    if (attribute_name_len < 3) attribute_name_len = 3;
    std::string attribute_name = randomString(attribute_name_len);
    genie::core::DataType attribute_type = genie::core::DataType::UINT8;
    // randomType();
    uint8_t attribute_num_array_dims = randomU2();

    std::vector<uint8_t> attribute_array_dims{};
    for (auto i = 0; i < attribute_num_array_dims; ++i) attribute_array_dims.push_back(randomU8());

    std::vector<uint8_t> attribute_default_val = randomValForType(attribute_type);

    bool attribute_miss_val_flag = randomBool();
    bool attribute_miss_default_flag = randomBool();
    if (!attribute_miss_val_flag) attribute_miss_default_flag = false;
    std::vector<uint8_t> attribute_miss_val = randomValForType(attribute_type);
    if (!attribute_miss_val_flag || attribute_miss_default_flag) attribute_miss_val.clear();
    uint8_t strLen = randomU4() + 3;
    std::string attribute_miss_str = randomString(strLen);
    if (!attribute_miss_val_flag) attribute_miss_str = "";

    uint8_t compressor_ID = randomU8();

    uint8_t n_steps_with_dependencies = randomU4();
    std::vector<uint8_t> dependency_step_ID(n_steps_with_dependencies, 0);
    for (auto& byte : dependency_step_ID) byte = randomU4();
    std::vector<uint8_t> n_dependencies(n_steps_with_dependencies, 0);
    for (auto& byte : n_dependencies) byte = randomU4();
    std::vector<std::vector<uint8_t>> dependency_var_ID(n_steps_with_dependencies, std::vector<uint8_t>());
    std::vector<std::vector<bool>> dependency_is_attribute(n_steps_with_dependencies, std::vector<bool>());
    std::vector<std::vector<uint16_t>> dependency_ID(n_steps_with_dependencies, std::vector<uint16_t>());
    for (auto i = 0; i < n_steps_with_dependencies; ++i) {
        dependency_var_ID[i].resize(n_dependencies[i]);
        dependency_is_attribute[i].resize(n_dependencies[i]);
        dependency_ID[i].resize(n_dependencies[i]);
        for (auto j = 0; j < n_dependencies[i]; ++j) {
            dependency_var_ID[i][j] = randomU4();
            dependency_is_attribute[i][j] = (randomBool());
            if (dependency_is_attribute[i][j])
                dependency_ID[i][j] = (randomU16());
            else
                dependency_ID[i][j] = (randomU8() / 2);
        }
    }
    genie::core::record::annotation_parameter_set::AttributeParameterSet attr(
        attribute_ID, attribute_name_len, attribute_name, attribute_type, attribute_num_array_dims,
        attribute_array_dims, attribute_default_val, attribute_miss_val_flag, attribute_miss_default_flag,
        attribute_miss_val, attribute_miss_str, compressor_ID, n_steps_with_dependencies, dependency_step_ID,
        n_dependencies, dependency_var_ID, dependency_is_attribute, dependency_ID);
    return attr;
}

genie::core::record::annotation_parameter_set::AlgorithmParameters
RandomAnnotationEncodingParameters::randomAlgorithmParameters() {
    uint8_t n_pars = randomU4();
    std::vector<uint8_t> par_num_array_dims(n_pars, 0);
    for (auto& num : par_num_array_dims) num = randomU2();
    return randomAlgorithmParameters(n_pars, par_num_array_dims);
}

genie::core::record::annotation_parameter_set::AlgorithmParameters
RandomAnnotationEncodingParameters::randomAlgorithmParameters(uint8_t nuMOfpars, std::vector<uint8_t> parNumArrayDims) {
    std::vector<uint8_t> par_num_array_dims(parNumArrayDims);
    uint8_t n_pars = nuMOfpars;
    std::vector<uint8_t> par_ID(n_pars, 0);
    std::vector<genie::core::DataType> par_type(n_pars, genie::core::DataType::UINT16);
    std::vector<std::vector<uint8_t>> par_array_dims(n_pars, std::vector<uint8_t>(0));
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val;

    for (auto i = 0; i < n_pars; ++i) {
        par_ID[i] = randomU4();
        par_type[i] = randomType();
        if (par_type[i] == genie::core::DataType::STRING) par_type[i] = genie::core::DataType::UINT8;
        //par_array_dims[i].resize(par_num_array_dims[i]+1);
        for (auto j = 0; j < par_num_array_dims[i]; ++j) par_array_dims[i][j] = randomU4();  // randomU8();
        std::vector<std::vector<std::vector<std::vector<uint8_t>>>> tempvec=
            genie::core::record::annotation_parameter_set::AlgorithmParameters::resizeVector(par_num_array_dims[i],
                                                                                             par_array_dims[i]);
        par_val.emplace_back(tempvec);
        for (auto& j : par_val[i])
            for (auto& k : j)
                for (auto& l : k) l = randomValForType(par_type[i]);
    }

    return genie::core::record::annotation_parameter_set::AlgorithmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);
}

genie::core::record::annotation_parameter_set::TileStructure RandomAnnotationEncodingParameters::randomTileStructure() {
    uint8_t ATCoordSize = randomU2();
    bool two_dimensional = randomBool();
    return randomTileStructure(ATCoordSize, two_dimensional);
}
genie::core::record::annotation_parameter_set::TileStructure RandomAnnotationEncodingParameters::simpleTileStructure(
    uint8_t ATCoordSize, bool two_dimensional) {
    bool variable_size_tiles = false;
    uint64_t n_tiles = 0;
    uint8_t dimensions = two_dimensional ? 2 : 1;
    std::vector<std::vector<uint64_t>> start_index;
    std::vector<std::vector<uint64_t>> end_index;
    std::vector<uint64_t> tile_size;
    start_index.resize(n_tiles, std::vector<uint64_t>(dimensions, 0));
    end_index.resize(n_tiles, std::vector<uint64_t>(dimensions, 0));
    tile_size.resize(dimensions, 0);

    if (variable_size_tiles)
        for (uint64_t i = 0; i < n_tiles; ++i) {
            for (auto j = 0; j < dimensions; ++j) {
                start_index[i][j] = 0;
                end_index[i][j] = 1;
            }
        }
    else
        for (auto j = 0; j < dimensions; ++j) tile_size[j] = 2;

    return genie::core::record::annotation_parameter_set::TileStructure(
        ATCoordSize, two_dimensional, variable_size_tiles, n_tiles, start_index, end_index, tile_size);
}

genie::core::record::annotation_parameter_set::TileStructure RandomAnnotationEncodingParameters::randomTileStructure(
    uint8_t ATCoordSize, bool two_dimensional) {
    bool variable_size_tiles = randomBool();
    uint64_t n_tiles = randomAtCoordSize(ATCoordSize);
    if (n_tiles > 4096) n_tiles = n_tiles % 4096;
    uint8_t dimensions = two_dimensional ? 2 : 1;
    std::vector<std::vector<uint64_t>> start_index;
    std::vector<std::vector<uint64_t>> end_index;
    std::vector<uint64_t> tile_size;
    if (variable_size_tiles) {
        start_index.resize(n_tiles, std::vector<uint64_t>(dimensions, 0));
        end_index.resize(n_tiles, std::vector<uint64_t>(dimensions, 0));
        for (uint64_t i = 0; i < n_tiles; ++i) {
            for (auto j = 0; j < dimensions; ++j) {
                start_index[i][j] = randomAtCoordSize(ATCoordSize);
                end_index[i][j] = randomAtCoordSize(ATCoordSize);
            }
        }
    } else {
        tile_size.resize(dimensions);
        for (auto i = 0; i < dimensions; ++i) {
            tile_size[i] = randomAtCoordSize(ATCoordSize);
        }
    }

    return genie::core::record::annotation_parameter_set::TileStructure(
        ATCoordSize, two_dimensional, variable_size_tiles, n_tiles, start_index, end_index, tile_size);
}

genie::core::record::annotation_parameter_set::TileConfiguration
RandomAnnotationEncodingParameters::randomTileConfiguration(uint8_t AT_coord_size) {
    genie::core::record::annotation_parameter_set::TileParameterSettings tileParameterSettings;
    uint8_t AG_class = randomU3();
    tileParameterSettings.attributeContiguity = randomBool();

    tileParameterSettings.twoDimensional = randomBool();

    tileParameterSettings.columnMajorTileOrder = randomBool();
    tileParameterSettings.symmetry_mode = randomU3();
    tileParameterSettings.symmetry_minor_diagonal = randomBool();

    tileParameterSettings.attribute_dependent_tiles = randomBool();

    genie::core::record::annotation_parameter_set::TileStructure default_tile_structure =
        simpleTileStructure(AT_coord_size, tileParameterSettings.twoDimensional);

    tileParameterSettings.n_add_tile_structures = randomU16();
    std::vector<uint16_t> n_attributes;
    std::vector<std::vector<uint16_t>> attribute_ID;
    std::vector<uint8_t> n_descriptors;
    std::vector<std::vector<uint8_t>> descriptor_ID;
    std::vector<genie::core::record::annotation_parameter_set::TileStructure> additional_tile_structure;

    if (tileParameterSettings.attribute_dependent_tiles) {
        tileParameterSettings.n_add_tile_structures = randomU8();  // randomU16
        n_attributes.resize(tileParameterSettings.n_add_tile_structures);
        descriptor_ID.resize(tileParameterSettings.n_add_tile_structures, std::vector<uint8_t>(0));
        attribute_ID.resize(tileParameterSettings.n_add_tile_structures, std::vector<uint16_t>(0));
        for (auto i = 0; i < tileParameterSettings.n_add_tile_structures; ++i) {
            n_attributes[i] = randomU8();  // randomU16
            for (auto j = 0; j < n_attributes[i]; ++j) {
                attribute_ID[i].push_back(randomU16());
            }
            n_descriptors.push_back(randomU8() / 2);
            for (auto j = 0; j < n_descriptors[i]; ++j) {
                descriptor_ID[i].push_back(randomU8() / 2);
            }
            additional_tile_structure.push_back(
                simpleTileStructure(AT_coord_size, tileParameterSettings.twoDimensional));
        }
    }
    return genie::core::record::annotation_parameter_set::TileConfiguration(
        AT_coord_size, AG_class, tileParameterSettings.attributeContiguity, tileParameterSettings.twoDimensional,
        tileParameterSettings.columnMajorTileOrder, tileParameterSettings.symmetry_mode,
        tileParameterSettings.symmetry_minor_diagonal,
        tileParameterSettings.attribute_dependent_tiles, default_tile_structure,
        tileParameterSettings.n_add_tile_structures, n_attributes,
        attribute_ID, n_descriptors, descriptor_ID, additional_tile_structure);
}

genie::core::record::annotation_parameter_set::ContactMatrixParameters
RandomAnnotationEncodingParameters::randomContactMatrixParameters() {
    uint8_t num_samples = randomU8();
    std::vector<uint8_t> sample_ID(num_samples, 0);
    for (auto& sampleID : sample_ID) sampleID = randomU8();
    std::vector<std::string> sample_name(num_samples, "");
    for (auto& sampleName : sample_name) sampleName = randomString(randomU4() + 2);
    uint8_t num_chrs = randomU8();
    std::vector<uint8_t> chr_ID(num_chrs, 0);
    for (auto& chrID : chr_ID) chrID = randomU8();
    std::vector<std::string> chr_name(num_chrs, "");
    for (auto& chrName : chr_name) chrName = randomString(randomU4() + 2);
    std::vector<uint64_t> chr_length(num_chrs, 0);
    for (auto& length : chr_length) length = randomU64();

    uint32_t interval = randomU32();
    uint32_t tile_size = randomU32();
    uint8_t num_interval_multipliers = randomU8();
    std::vector<uint32_t> interval_multiplier(num_interval_multipliers, 0);
    for (auto& intMult : interval_multiplier) intMult = randomU32();
    uint8_t num_norm_methods = randomU8();
    std::vector<uint8_t> norm_method_ID(num_norm_methods, 0);
    for (auto& normMethodID : norm_method_ID) normMethodID = randomU8();
    std::vector<std::string> norm_method_name(num_norm_methods, "");
    for (auto& normMethodName : norm_method_name) normMethodName = randomString(randomU4() + 2);
    std::vector<bool> norm_method_mult_flag(num_norm_methods, false);
    for (auto i = 0; i < num_norm_methods; ++i) norm_method_mult_flag[i] = randomBool();

    uint8_t num_norm_matrices = randomU8();
    std::vector<uint8_t> norm_matrix_ID(num_norm_matrices, 0);
    for (auto& ID : norm_matrix_ID) ID = randomU8();
    std::vector<std::string> norm_matrix_name(num_norm_matrices, "");
    for (auto& name : norm_matrix_name) name = randomString(randomU4() + 2);

    return genie::core::record::annotation_parameter_set::ContactMatrixParameters(
        num_samples, sample_ID, sample_name, num_chrs, chr_ID, chr_name, chr_length, interval, tile_size,
        num_interval_multipliers, interval_multiplier, num_norm_methods, norm_method_ID, norm_method_name,
        norm_method_mult_flag, num_norm_matrices, norm_matrix_ID, norm_matrix_name);
}

genie::core::record::annotation_parameter_set::ContactMatrixParameters
RandomAnnotationEncodingParameters::simpleContactMatrixParameters() {
    return genie::core::record::annotation_parameter_set::ContactMatrixParameters();
}

genie::core::record::annotation_parameter_set::TileConfiguration
RandomAnnotationEncodingParameters::randomTileConfiguration() {
    uint8_t AT_coord_size = randomU2();
    return randomTileConfiguration(AT_coord_size);
}



genie::core::record::annotation_parameter_set::CompressorParameterSet
RandomAnnotationEncodingParameters::randomCompressorParameterSet() {
    uint8_t compressor_ID = randomU8();
    uint8_t n_compressor_steps = randomU4();
    std::vector<uint8_t> compressor_step_ID(n_compressor_steps, 0);
    for (auto& ID : compressor_step_ID) ID = randomU4();
    std::vector<genie::core::AlgoID> algorithm_ID(n_compressor_steps, genie::core::AlgoID::BSC);
    //    for (auto& ID : algorithm_ID) ID = static_cast<genie::core::AlgoID>(randomU4() + randomU4());
    std::vector<bool> use_default_pars(n_compressor_steps);
    std::vector<genie::core::record::annotation_parameter_set::AlgorithmParameters> algorithm_parameters;
    for (auto i = 0; i < n_compressor_steps; ++i) {
        use_default_pars[i] = true;
        // temporary set al to true, randomBool();
        if (!use_default_pars[i])
            algorithm_parameters.push_back(genie::core::record::annotation_parameter_set::AlgorithmParameters());
    }
    std::vector<uint8_t> n_in_vars(n_compressor_steps, 0);
    std::vector<std::vector<uint8_t>> in_var_ID(n_compressor_steps);
    std::vector<std::vector<uint8_t>> prev_step_ID(n_compressor_steps);
    std::vector<std::vector<uint8_t>> prev_out_var_ID(n_compressor_steps);
    std::vector<uint8_t> n_completed_out_vars(n_compressor_steps);
    std::vector<std::vector<uint8_t>> completed_out_var_ID(n_compressor_steps);
    for (auto i = 0; i < n_compressor_steps; ++i) {
        n_in_vars[i] = randomU4();
        for (auto j = 0; j < n_in_vars[i]; ++j) {
            in_var_ID[i].push_back(randomU4());
            prev_step_ID[i].push_back(randomU4());
            prev_out_var_ID[i].push_back(randomU4());
        }
        n_completed_out_vars[i] = randomU4();
        for (auto j = 0; j < n_completed_out_vars[i]; ++j) completed_out_var_ID[i].push_back(randomU4());
    }

    return genie::core::record::annotation_parameter_set::CompressorParameterSet(
        compressor_ID, n_compressor_steps, compressor_step_ID, algorithm_ID, use_default_pars, algorithm_parameters,
        n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars, completed_out_var_ID);
}

genie::core::record::annotation_parameter_set::AnnotationEncodingParameters
RandomAnnotationEncodingParameters::randomAnnotationEncodingParameters() {
    uint8_t n_filter = randomU8();
    std::vector<uint8_t> filter_ID_len;
    std::vector<std::string> filter_ID;
    std::vector<uint16_t> desc_len;
    std::vector<std::string> description;

    for (auto i = 0; i < n_filter; ++i) filter_ID_len.push_back(randomU6());
    for (auto i = 0; i < n_filter; ++i) filter_ID.push_back(randomString(filter_ID_len[i]));
    for (auto i = 0; i < n_filter; ++i) desc_len.push_back(randomU6());
    for (auto i = 0; i < n_filter; ++i) description.push_back(randomString(desc_len[i]));

    uint8_t n_features_names = randomU8();
    std::vector<uint8_t> feature_name_len;
    std::vector<std::string> feature_name;
    for (auto i = 0; i < n_features_names; ++i) feature_name_len.push_back(randomU6());
    for (auto i = 0; i < n_features_names; ++i) feature_name.push_back(randomString(feature_name_len[i]));

    uint8_t n_ontology_terms = randomU8();
    std::vector<uint8_t> ontology_term_name_len;
    std::vector<std::string> ontology_term_name;
    for (auto i = 0; i < n_ontology_terms; ++i) ontology_term_name_len.push_back(randomU6());
    for (auto i = 0; i < n_ontology_terms; ++i) ontology_term_name.push_back(randomString(ontology_term_name_len[i]));

    uint8_t n_descriptors = randomU8();
    std::vector<genie::core::record::annotation_parameter_set::DescriptorConfiguration> descriptor_configuration(
        n_descriptors, genie::core::record::annotation_parameter_set::DescriptorConfiguration());

    uint8_t n_compressors = randomU8();
    std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet> compressor_parameter_set(
        n_compressors, genie::core::record::annotation_parameter_set::CompressorParameterSet());

    uint8_t n_attributes = randomU8();
    std::vector<genie::core::record::annotation_parameter_set::AttributeParameterSet> attribute_parameter_set(
        n_attributes, genie::core::record::annotation_parameter_set::AttributeParameterSet());

    return genie::core::record::annotation_parameter_set::AnnotationEncodingParameters(
        n_filter, filter_ID_len, filter_ID, desc_len, description, n_features_names, feature_name_len, feature_name,
        n_ontology_terms, ontology_term_name_len, ontology_term_name, n_descriptors, descriptor_configuration,
        n_compressors, compressor_parameter_set, n_attributes, attribute_parameter_set);
}

genie::core::record::annotation_parameter_set::Record
RandomAnnotationEncodingParameters::randomAnnotationParameterSet() {
    uint8_t parameter_set_ID = randomU8();
    uint8_t AT_ID = randomU8();
    genie::core::AlphabetID AT_alphabet_ID = static_cast <genie::core::AlphabetID>(randomU8());
    uint8_t AT_coord_size = randomU2();
    bool AT_pos_40_bits_flag = randomBool();
    uint8_t n_aux_attribute_groups = randomU3();
    std::vector<genie::core::record::annotation_parameter_set::TileConfiguration> tile_configuration;
    for (auto i = 0; i <= n_aux_attribute_groups; ++i)
        tile_configuration.push_back(genie::core::record::annotation_parameter_set::TileConfiguration(AT_coord_size));

    genie::core::record::annotation_parameter_set::AnnotationEncodingParameters annotation_encoding_parameters;

    return genie::core::record::annotation_parameter_set::Record(parameter_set_ID, AT_ID, AT_alphabet_ID, AT_coord_size,
                                                                 AT_pos_40_bits_flag, n_aux_attribute_groups,
                                                                 tile_configuration, annotation_encoding_parameters);
}
