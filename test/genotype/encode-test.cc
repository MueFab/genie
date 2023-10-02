/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <codecs/include/mpegg-codecs.h>
#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include <xtensor/xmath.hpp>
#include <xtensor/xoperation.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xview.hpp>
#include "genie/core/constants.h"
#include "genie/core/record/variant_genotype/record.h"
#include "genie/genotype/genotype_coder.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/runtime-exception.h"
#include "helpers.h"

#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie/core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie/genotype/genotype_parameters.h"
#include "genie/genotype/genotype_payload.h"

#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/core/record/data_unit/record.h"
#include "genie/variantsite/AccessUnitComposer.h"
#include "genie/variantsite/ParameterSetComposer.h"

class ParameterSetComposer {
 public:
    genie::core::record::annotation_parameter_set::Record Build(
        genie::genotype::GenotypeParameters& genotypeParameters, genie::genotype::EncodingOptions opt,
        uint64_t defaultTileSize = 100) {
        uint8_t parameter_set_ID = 1;
        uint8_t AT_ID = 1;
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

        uint8_t compressor_ID = 1;
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

        genie::core::record::annotation_parameter_set::CompressorParameterSet LZMACompressorParameterSet(
            compressor_ID, n_compressor_steps, compressor_step_ID, LZMAalgorithm_ID, use_default_pars,
            algorithm_parameters, n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars,
            completed_out_var_ID);
        std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet> compressor_parameter_set{
            LZMACompressorParameterSet};

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
                descriptor_ID, opt.codec_ID, genotypeParameters, LZMAalgorithmParameters)};

        uint8_t n_compressors = compressor_parameter_set.size();

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
};
TEST(Genotype, Decompose) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    genie::genotype::EncodingOptions opt = {
        512,                                         // block_size;
        genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
        false,                                       // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::core::AlgoID::JBIG                    // codec_ID;
    };

    genie::genotype::EncodingBlock block{};
    genie::genotype::decompose(opt, block, recs);

    ASSERT_EQ(block.max_ploidy, 2);
    ASSERT_EQ(block.dot_flag, false);
    ASSERT_EQ(block.na_flag, false);

    auto& allele_mat = block.allele_mat;
    auto& phasing_mat = block.phasing_mat;

    // Check allele_mat shape
    ASSERT_EQ(allele_mat.dimension(), 2);
    ASSERT_EQ(allele_mat.shape(0), 100);
    ASSERT_EQ(allele_mat.shape(1), 1092 * 2);
    ASSERT_EQ(allele_mat(0, 3), 1);

    // Check phasing_mat shape
    ASSERT_EQ(phasing_mat.dimension(), 2);
    ASSERT_EQ(phasing_mat.shape(0), 100);
    ASSERT_EQ(phasing_mat.shape(1), 1092);

    // Check all values
    ASSERT_EQ(xt::amin(allele_mat)(0), 0);
    ASSERT_EQ(xt::amax(allele_mat)(0), 1);

    // Check the content of the first row of allele_tensor
    {
        //        const auto& allele_rec = xt::view(allele_mat, 0, xt::all(), xt::all());
        auto allele_rec = xt::view(allele_mat, 0, xt::all());
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 0))(0), 2067);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 1))(0), 117);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 2))(0), 0);
    }

    // Check the content of the last row of allele_tensor
    {
        //        const auto& allele_rec = xt::view(allele_mat, -1, xt::all(), xt::all());
        auto allele_rec = xt::view(allele_mat, -1, xt::all());
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 0))(0), 2178);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 1))(0), 6);
        ASSERT_EQ(xt::sum(xt::equal(allele_rec, 2))(0), 0);
    }

    //------------------------------------------------------
}

TEST(Genotype, parameters) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();
    genie::genotype::EncodingOptions opt = {
        512,                                         // block_size;
        genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
        false,                                       // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::core::AlgoID::JBIG                    // codec_ID;
    };

    // ------------------------------
    auto tupleoutput = genie::genotype::encode_block(opt, recs);
    genie::genotype::GenotypeParameters genotypeParameters = std::get<genie::genotype::GenotypeParameters>(tupleoutput);
    std::stringstream dataout;
    {
        genie::core::Writer writer(&dataout);
        genotypeParameters.write(writer);
        EXPECT_GE(genotypeParameters.getSize(writer), 15);
    }

    ParameterSetComposer genotypeParameterSet;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet =
        genotypeParameterSet.Build(genotypeParameters, opt, recs.size());

    //// -----------------
    // uint8_t parameter_set_ID = 1;
    // uint8_t AT_ID = 1;
    // genie::core::AlphabetID AT_alphabet_ID = genie::core::AlphabetID::ACGTN;
    // genie::core::AnnotDesc descriptor_ID = genie::core::AnnotDesc::GENOTYPE;
    // uint8_t AT_coord_size = 3;
    // bool AT_pos_40_bits_flag = false;
    // uint8_t n_aux_attribute_groups = 0;
    //// -------------
    // bool two_dimensional = false;
    // bool variable_size_tiles = false;
    // uint64_t n_tiles = 1;
    // std::vector<std::vector<uint64_t>> start_index{0};
    // std::vector<std::vector<uint64_t>> end_index{0};
    // std::vector<uint64_t> tile_size{recs.size()};

    // genie::core::record::annotation_parameter_set::TileStructure defaultTileStructure(
    //    AT_coord_size, two_dimensional, variable_size_tiles, n_tiles, start_index, end_index, tile_size);
    //// ---------------
    // uint8_t AG_class = 0;
    // bool attribute_contiguity = false;
    // bool column_major_tile_order = false;
    // uint8_t symmetry_mode = 0;
    // bool symmetry_minor_diagonal = false;
    // bool attribute_dependent_tiles = false;

    // uint16_t n_add_tile_structures = 0;
    // std::vector<uint16_t> n_attributes = {0};
    // std::vector<std::vector<uint16_t>> attributeIDs{};
    // std::vector<uint8_t> n_descriptors{1};
    // std::vector<std::vector<uint8_t>> descriptorIDs{};

    // std::vector<genie::core::record::annotation_parameter_set::TileStructure> additional_tile_structure{
    //    defaultTileStructure};

    // std::vector<genie::core::record::annotation_parameter_set::TileConfiguration> tile_configuration{
    //    genie::core::record::annotation_parameter_set::TileConfiguration(
    //        AT_coord_size, AG_class, attribute_contiguity, two_dimensional, column_major_tile_order, symmetry_mode,
    //        symmetry_minor_diagonal, attribute_dependent_tiles, defaultTileStructure, n_add_tile_structures,
    //        n_attributes, attributeIDs, n_descriptors, descriptorIDs, additional_tile_structure)};

    ////-------------------
    // genie::entropy::lzma::LZMAParameters lzmaParameters;
    // auto LZMAalgorithmParameters = lzmaParameters.convertToAlgorithmParameters();

    // uint8_t compressor_ID = 1;

    // std::vector<genie::core::AlgoID> LZMAalgorithm_ID{genie::core::AlgoID::LZMA};
    // uint8_t n_compressor_steps = 1;
    // std::vector<uint8_t> compressor_step_ID{0};
    // std::vector<bool> use_default_pars{true};
    // std::vector<genie::core::record::annotation_parameter_set::AlgorithmParameters> algorithm_parameters;
    // std::vector<uint8_t> n_in_vars{0};
    // std::vector<std::vector<uint8_t>> in_var_ID{{0}};
    // std::vector<std::vector<uint8_t>> prev_step_ID;
    // std::vector<std::vector<uint8_t>> prev_out_var_ID;
    // std::vector<uint8_t> n_completed_out_vars{0};
    // std::vector<std::vector<uint8_t>> completed_out_var_ID;

    // genie::core::record::annotation_parameter_set::CompressorParameterSet LZMACompressorParameterSet(
    //    compressor_ID, n_compressor_steps, compressor_step_ID, LZMAalgorithm_ID, use_default_pars,
    //    algorithm_parameters, n_in_vars, in_var_ID, prev_step_ID, prev_out_var_ID, n_completed_out_vars,
    //    completed_out_var_ID);
    // std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet> compressor_parameter_set{
    //    LZMACompressorParameterSet};
    //// -----------------
    // uint8_t n_filter = 0;
    // std::vector<uint8_t> filter_ID_len;
    // std::vector<std::string> filter_ID;
    // std::vector<uint16_t> desc_len;
    // std::vector<std::string> description;

    // uint8_t n_features_names = 0;
    // std::vector<uint8_t> feature_name_len;
    // std::vector<std::string> feature_name;

    // uint8_t n_ontology_terms = 0;
    // std::vector<uint8_t> ontology_term_name_len;
    // std::vector<std::string> ontology_term_name;

    // uint8_t ndescriptors = 1;
    // std::vector<genie::core::record::annotation_parameter_set::DescriptorConfiguration> descriptor_configuration{
    //    genie::core::record::annotation_parameter_set::DescriptorConfiguration(
    //        descriptor_ID, opt.codec_ID, genotypeParameters, LZMAalgorithmParameters)};

    // uint8_t n_compressors = compressor_parameter_set.size();

    // uint8_t nattributes = 0;
    // std::vector<genie::core::record::annotation_parameter_set::AttributeParameterSet> attribute_parameter_set;

    // genie::core::record::annotation_parameter_set::AnnotationEncodingParameters annotation_encoding_parameters(
    //    n_filter, filter_ID_len, filter_ID, desc_len, description, n_features_names, feature_name_len, feature_name,
    //    n_ontology_terms, ontology_term_name_len, ontology_term_name, ndescriptors, descriptor_configuration,
    //    n_compressors, compressor_parameter_set, nattributes, attribute_parameter_set);
    //// ------------------------------------
    // genie::core::record::annotation_parameter_set::Record annotationParameterSet(
    //    parameter_set_ID, AT_ID, AT_alphabet_ID, AT_coord_size, AT_pos_40_bits_flag, n_aux_attribute_groups,
    //    tile_configuration, annotation_encoding_parameters);
    //// ----------------------
    std::map<genie::core::AnnotDesc, std::stringstream> encodedDescriptors;
    std::map<std::string, genie::core::record::variant_site::AttributeData> info;

    genie::core::record::data_unit::Record APS_dataUnit(annotationParameterSet);
    {
        std::string name = gitRootDir + "/data/records/genotype_APS_DataUnit";

        std::ofstream testfile;
        testfile.open(name + ".bin", std::ios::binary | std::ios::out);
        if (testfile.is_open()) {
            genie::core::Writer writer(&testfile);
            APS_dataUnit.write(writer);
            testfile.close();
        }

        std::ofstream txtfile;
        txtfile.open(name + ".txt", std::ios::out);
        if (txtfile.is_open()) {
            genie::core::Writer txtWriter(&txtfile, true);
            APS_dataUnit.write(txtWriter);
            txtfile.close();
        }
    }
    //-----------------------------------------------------
    auto datablock = std::get<genie::genotype::EncodingBlock>(tupleoutput);
    std::vector<genie::genotype::BinMatPayload> variantsPayload;
    datablock.allele_bin_mat_vect;
    for (auto alleleBinMat : datablock.allele_bin_mat_vect) {
        size_t payloadSize = 0;
        uint8_t* payloadArray;
        genie::genotype::bin_mat_to_bytes(alleleBinMat, &payloadArray, payloadSize);
        std::vector<uint8_t> payload;
        for (auto i = 0; i < payloadSize; ++i) {
            payload.push_back(payloadArray[i]);
        }
        variantsPayload.emplace_back(genie::genotype::BinMatPayload(genie::core::AlgoID::JBIG, payload));
    }
    std::vector<uint8_t> payload;
    if (datablock.phasing_mat.size() > 0) {
        size_t payloadSize = 0;
        uint8_t* payloadArray;
        genie::genotype::bin_mat_to_bytes(datablock.phasing_mat, &payloadArray, payloadSize);
        for (auto i = 0; i < payloadSize; ++i) {
            payload.push_back(payloadArray[i]);
        }
    }
    genie::genotype::BinMatPayload PhasesPayload(genie::core::AlgoID::JBIG, payload);

    std::vector<genie::genotype::RowColIdsPayload> sortRowIdsPayload;
    std::vector<genie::genotype::RowColIdsPayload> sortColIdsPayload;

    for (auto alleleRowIDs : datablock.allele_row_ids_vect) {
        std::vector<uint64_t> payloadVec;
        for (auto elem : alleleRowIDs) payloadVec.push_back(elem);
        sortRowIdsPayload.emplace_back(genie::genotype::RowColIdsPayload(payloadVec.size(), 64, payloadVec));
    }

    for (auto alleleColIDs : datablock.allele_col_ids_vect) {
        std::vector<uint64_t> payloadVec;
        for (auto elem : alleleColIDs) payloadVec.push_back(elem);
        sortColIdsPayload.emplace_back(genie::genotype::RowColIdsPayload(payloadVec.size(), 64, payloadVec));
    }
    genie::genotype::AmexPayload amexPayload(0, 0, 0, {}, {});
    //-----------------------------------------------------
    genie::genotype::GenotypePayload genotypePayload(genotypeParameters, variantsPayload, PhasesPayload,
                                                     sortRowIdsPayload, sortColIdsPayload, amexPayload);

    //-----------------------------------------------------
    std::stringstream compressedout;
    {
        genie::entropy::bsc::BSCEncoder bscEncoder;
        std::stringstream tempstream;
        genie::core::Writer writer(&tempstream);
        genotypePayload.write(writer);
        bscEncoder.encode(tempstream, compressedout);
    }

    //-----------------------------------------------------
    std::vector<uint8_t> genotypePayloadData;
    for (auto readbyte : compressedout.str()) genotypePayloadData.push_back(readbyte);

    genie::core::record::annotation_access_unit::BlockVectorData data(genie::core::AnnotDesc::GENOTYPE,
                                                                      genotypePayloadData);
    genie::core::record::annotation_access_unit::Block block;
    block.set(data);
    //-----------------------------------------------------

    genie::core::record::annotation_access_unit::AnnotationType AT_type =
        genie::core::record::annotation_access_unit::AnnotationType::VARIANTS;

    uint8_t AT_subtype = 0;
    //--------------------
    bool is_attribute = false;
    uint16_t attribute_ID = 0;
    uint64_t n_tiles_per_col = 1;
    uint64_t n_tiles_per_row = 1;
    uint64_t tile_index_1 = 0;
    bool tile_index_2_exists = false;
    uint64_t tile_index_2 = 0;
    uint64_t n_blocks = 1;
    uint8_t AT_coord_size = 3;
    bool AT_pos_40_bits_flag = false;
    uint8_t n_aux_attribute_groups = 0;
    // -------------
    bool two_dimensional = false;
    bool variable_size_tiles = false;
    bool attribute_contiguity = false;
    bool column_major_tile_order = false;
    uint8_t AG_class = 0;
    uint8_t AT_ID = 1;

    genie::core::AnnotDesc descriptor_ID = genie::core::AnnotDesc::GENOTYPE;

    std::vector<genie::core::record::annotation_access_unit::Block> blocks;

    genie::core::record::annotation_access_unit::AnnotationAccessUnitHeader annotation_access_unit_header(
        attribute_contiguity, two_dimensional, column_major_tile_order, variable_size_tiles, AT_coord_size,
        is_attribute, attribute_ID, descriptor_ID, n_tiles_per_col, n_tiles_per_row, n_blocks, tile_index_1,
        tile_index_2_exists, tile_index_2);
    bool indexed = false;
    uint32_t block_payload_size = 0;

    genie::core::record::annotation_access_unit::BlockHeader block_header(attribute_contiguity, descriptor_ID,
                                                                          attribute_ID, indexed, block_payload_size);

    int8_t numChrs = 0;

    genie::core::record::annotation_access_unit::Record annotationAccessUnit(
        AT_ID, AT_type, AT_subtype, AG_class, annotation_access_unit_header, blocks, attribute_contiguity,
        two_dimensional, column_major_tile_order, AT_coord_size, variable_size_tiles, n_blocks, numChrs);

    genie::core::record::data_unit::Record AAU_dataUnit(annotationAccessUnit);

    {
        std::string name = gitRootDir + "/data/records/genotype_AAU_DataUnit";

        std::ofstream testfile;
        testfile.open(name + ".bin", std::ios::binary | std::ios::out);
        if (testfile.is_open()) {
            genie::core::Writer writer(&testfile);
            AAU_dataUnit.write(writer);
            testfile.close();
        }

        std::ofstream txtfile;
        txtfile.open(name + ".txt", std::ios::out);
        if (txtfile.is_open()) {
            genie::core::Writer txtWriter(&txtfile, true);
            AAU_dataUnit.write(txtWriter);
            txtfile.close();
        }
    }
}

TEST(Genotype, AdaptiveMaxValue) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    genie::genotype::EncodingOptions opt = {
        512,                                         // block_size;
        genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
        false,                                       // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::core::AlgoID::JBIG                    // codec_ID;
    };

    genie::genotype::EncodingBlock block{};
    genie::genotype::decompose(opt, block, recs);
    genie::genotype::transform_max_value(block);

    ASSERT_EQ(block.dot_flag, false);
    ASSERT_EQ(block.na_flag, false);
    ASSERT_EQ(xt::amin(block.allele_mat)(0), 0);
    ASSERT_EQ(xt::amax(block.allele_mat)(0), 1);
}

TEST(Genotype, BinarizeBitPlane) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    // Check DO_NOT_CONCAT
    {
        genie::genotype::EncodingOptions opt = {
            512,                                         // block_size;
            genie::genotype::BinarizationID::BIT_PLANE,  // binarization_ID;
            genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
            false,                                       // transpose_mat;
            genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
            genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
            genie::core::AlgoID::JBIG                    // codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);

        ASSERT_EQ(block.allele_bin_mat_vect.size(), 1);
        // TODO @Yeremia: Update the assert
    }

    // Check CONCAT_ROW_DIR
    {
        genie::genotype::EncodingOptions opt = {
            512,                                          // block_size;
            genie::genotype::BinarizationID::BIT_PLANE,   // binarization_ID;
            genie::genotype::ConcatAxis::CONCAT_ROW_DIR,  // concat_axis;
            false,                                        // transpose_mat;
            genie::genotype::SortingAlgoID::NO_SORTING,   // sort_row_method;
            genie::genotype::SortingAlgoID::NO_SORTING,   // sort_row_method;
            genie::core::AlgoID::JBIG                     // codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);

        ASSERT_EQ(block.allele_bin_mat_vect.size(), 1);
        // TODO @Yeremia: Update the assert
    }

    // Check CONCAT_COL_DIR
    {
        genie::genotype::EncodingOptions opt = {
            512,                                          // block_size;
            genie::genotype::BinarizationID::BIT_PLANE,   // binarization_ID;
            genie::genotype::ConcatAxis::CONCAT_COL_DIR,  // concat_axis;
            false,                                        // transpose_mat;
            genie::genotype::SortingAlgoID::NO_SORTING,   // sort_row_method;
            genie::genotype::SortingAlgoID::NO_SORTING,   // sort_row_method;
            genie::core::AlgoID::JBIG                     // codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);

        ASSERT_EQ(block.allele_bin_mat_vect.size(), 1);
        // TODO @Yeremia: Update the assert
    }
}

TEST(Genotype, BinarizeRowBin) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";
    std::vector<genie::core::record::VariantGenotype> recs;

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);
    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }
    reader.close();

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    genie::genotype::EncodingOptions opt = {
        512,                                         // block_size;
        genie::genotype::BinarizationID::ROW_BIN,    // binarization_ID;
        genie::genotype::ConcatAxis::DO_NOT_CONCAT,  // concat_axis;
        false,                                       // transpose_mat;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::genotype::SortingAlgoID::NO_SORTING,  // sort_row_method;
        genie::core::AlgoID::JBIG                    // codec_ID;
    };

    genie::genotype::EncodingBlock block{};
    genie::genotype::decompose(opt, block, recs);
    genie::genotype::transform_max_value(block);
    genie::genotype::binarize_allele_mat(opt, block);

    // TODO @Yeremia: Update the assert
    ASSERT_EQ(block.allele_bin_mat_vect.size(), 1);
    ASSERT_EQ(block.allele_bin_mat_vect[0].shape(0), static_cast<size_t>(xt::sum(block.amax_vec)(0)));
    //    ASSERT_EQ(block.allele_bin_mat_vect.shape(2), block.allele_mat.shape(1));
}

TEST(Genotype, RandomSort) {
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
    std::string filepath = gitRootDir + "/data/records/1.3.5.header100.gt_only.vcf.geno";

    std::ifstream reader(filepath, std::ios::binary | std::ios::in);
    ASSERT_EQ(reader.fail(), false);
    genie::util::BitReader bitreader(reader);

    std::vector<genie::core::record::VariantGenotype> recs;

    while (bitreader.isGood()) {
        recs.emplace_back(bitreader);
    }

    // TODO (Yeremia): Temporary fix as the number of records exceeded by 1
    recs.pop_back();

    ASSERT_EQ(recs.size(), 100);

    {
        genie::genotype::EncodingOptions opt = {
            512,                                          // block_size;
            genie::genotype::BinarizationID::BIT_PLANE,   // binarization_ID;
            genie::genotype::ConcatAxis::DO_NOT_CONCAT,   // concat_axis;
            false,                                        // transpose_mat;
            genie::genotype::SortingAlgoID::RANDOM_SORT,  // sort_row_method;
            genie::genotype::SortingAlgoID::NO_SORTING,   // sort_row_method;
            genie::core::AlgoID::JBIG                     // codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);
        genie::genotype::sort_block(opt, block);
    }

    {
        genie::genotype::EncodingOptions opt = {
            512,                                          // block_size;
            genie::genotype::BinarizationID::BIT_PLANE,   // binarization_ID;
            genie::genotype::ConcatAxis::DO_NOT_CONCAT,   // concat_axis;
            false,                                        // transpose_mat;
            genie::genotype::SortingAlgoID::NO_SORTING,   // sort_row_method;
            genie::genotype::SortingAlgoID::RANDOM_SORT,  // sort_row_method;
            genie::core::AlgoID::JBIG                     // codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);
        genie::genotype::sort_block(opt, block);
    }

    {
        genie::genotype::EncodingOptions opt = {
            512,                                          // block_size;
            genie::genotype::BinarizationID::BIT_PLANE,   // binarization_ID;
            genie::genotype::ConcatAxis::DO_NOT_CONCAT,   // concat_axis;
            false,                                        // transpose_mat;
            genie::genotype::SortingAlgoID::RANDOM_SORT,  // sort_row_method;
            genie::genotype::SortingAlgoID::RANDOM_SORT,  // sort_row_method;
            genie::core::AlgoID::JBIG                     // codec_ID;
        };

        genie::genotype::EncodingBlock block{};
        genie::genotype::decompose(opt, block, recs);
        genie::genotype::transform_max_value(block);
        genie::genotype::binarize_allele_mat(opt, block);
        genie::genotype::sort_block(opt, block);
    }
}

TEST(Genotype, Serializer) {
    genie::genotype::BinMatDtype bin_mat;
    size_t orig_payload_len = 15;
    uint8_t bitmap[15] = {0x7c, 0xe2, 0x38, 0x04, 0x92, 0x40, 0x04, 0xe2, 0x5c, 0x44, 0x92, 0x44, 0x38, 0xe2, 0x38};
    size_t ncols = 23;
    size_t nrows = 5;

    genie::genotype::bin_mat_from_bytes(bin_mat, bitmap, orig_payload_len, nrows, ncols);

    std::vector<uint8_t> temp;
    for (auto byte : bin_mat) temp.push_back(byte);

    uint8_t* payload;
    size_t payload_len;
    genie::genotype::bin_mat_to_bytes(bin_mat, &payload, payload_len);

    ASSERT_EQ(orig_payload_len, payload_len);
    for (size_t i = 0; i < payload_len; i++) {
        ASSERT_EQ(*(payload + i), *(bitmap + i));
    }
}

TEST(Genotype, JBIG) {
    genie::genotype::BinMatDtype bin_mat;
    size_t orig_payload_len = 15;
    uint8_t orig_payload[15] = {0x7c, 0xe2, 0x38, 0x04, 0x92, 0x40, 0x04, 0xe2,
                                0x5c, 0x44, 0x92, 0x44, 0x38, 0xe2, 0x38};
    unsigned long orig_ncols = 23;
    unsigned long orig_nrows = 5;

    std::vector<uint8_t> mem_data_destination(3 * orig_payload_len);
    uint8_t* compressed_data = &mem_data_destination[0];

    size_t compressed_data_len;

    mpegg_jbig_compress_default(&compressed_data, &compressed_data_len, orig_payload, orig_payload_len, orig_nrows,
                                orig_ncols);

    std::vector<uint8_t> mem_data_source(3 * orig_payload_len);
    uint8_t* payload = &mem_data_source[0];
    ;
    size_t payload_len;

    unsigned long ncols;
    unsigned long nrows;
    mpegg_jbig_decompress_default(&payload, &payload_len, compressed_data, compressed_data_len, &nrows, &ncols);

    ASSERT_EQ(nrows, orig_nrows);
    ASSERT_EQ(ncols, orig_ncols);

    ASSERT_EQ(orig_payload_len, payload_len);
    for (size_t i = 0; i < payload_len; i++) {
        ASSERT_EQ(*(payload + i), *(orig_payload + i));
    }
}
