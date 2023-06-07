#include <gtest/gtest.h>
#include <fstream>
//#include <iostream>
#include <sstream>

#include "genie//core/record/annotation_parameter_set/TileConfiguration.h"
#include "genie//core/record/annotation_parameter_set/TileStructure.h"
#include "genie//core/record/annotation_parameter_set/record.h"

#include "genie//core/record/annotation_parameter_set/AnnotationEncodingParameters.h"
#include "genie//core/record/annotation_parameter_set/AttributeParameterSet.h"
#include "genie//core/record/annotation_parameter_set/CompressorParameterSet.h"

#include "genie//core/record/annotation_parameter_set/AlgorithmParameters.h"
#include "genie//core/record/annotation_parameter_set/AnnotationEncodingParameters.h"
#include "genie//core/record/annotation_parameter_set/ContactMatrixParameters.h"
#include "genie//core/record/annotation_parameter_set/DescriptorConfiguration.h"
#include "genie//core/record/annotation_parameter_set/GenotypeParameters.h"
#include "genie//core/record/annotation_parameter_set/LikelihoodParameters.h"

#include "genie//core/record/annotation_access_unit/AnnotationAccessUnitHeader.h"
#include "genie//core/record/annotation_access_unit/block.h"
#include "genie//core/record/annotation_access_unit/record.h"

#include "codecs/include/mpegg-codecs.h"

#define GENERATE_TEST_FILES false

class MpeggBSCTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    MpeggBSCTests() = default;

    ~MpeggBSCTests() override = default;

    // Use SetUp instead of the constructor in the following cases:
    // - In the body of a constructor (or destructor), it's not possible to
    //   use the ASSERT_xx macros. Therefore, if the set-up operation could
    //   cause a fatal test failure that should prevent the test from running,
    //   it's necessary to use a CHECK macro or to use SetUp() instead of a
    //   constructor.
    // - If the tear-down operation could throw an exception, you must use
    //   TearDown() as opposed to the destructor, as throwing in a destructor
    //   leads to undefined behavior and usually will kill your program right
    //   away. Note that many standard libraries (like STL) may throw when
    //   exceptions are enabled in the compiler. Therefore you should prefer
    //   TearDown() if you want to write portable tests that work with or
    //   without exceptions.
    // - The googletest team is considering making the assertion macros throw
    //   on platforms where exceptions are enabled (e.g. Windows, Mac OS, and
    //   Linux client-side), which will eliminate the need for the user to
    //   propagate failures from a subroutine to its caller. Therefore, you
    //   shouldn't use googletest assertions in a destructor if your code
    //   could run on such a platform.
    // - In a constructor or destructor, you cannot make a virtual function
    //   call on this object. (You can call a method declared as virtual, but
    //   it will be statically bound.) Therefore, if you need to call a method
    //   that will be overridden in a derived class, you have to use
    //   SetUp()/TearDown().

    void SetUp() override {
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }

    // void sharedSubroutine() {
    //    // If needed, define subroutines for your tests to share
    // }
};

TEST_F(MpeggBSCTests, testPayload) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    genie::core::record::annotation_parameter_set::DescriptorID descriptor_ID =
        genie::core::record::annotation_parameter_set::DescriptorID::ATTRIBUTE;
    uint8_t num_chrs = 1;
    genie::core::record::annotation_access_unit::GenotypePayload genotype_payload;
    genie::core::record::annotation_access_unit::LikelihoodPayload likelihood_payload;
    std::vector<genie::core::record::annotation_access_unit::ContactMatrixBinPayload> cm_bin_payload;
    genie::core::record::annotation_access_unit::ContactMatrixMatPayload cm_mat_payload;

    std::string FileName = "TestFiles/exampleMGrecs/ALL.chrX.10000.site";
    std::ifstream infile(FileName, std::ios::binary | std::ios::in);
    if (infile.fail()) {
        std::cout << "failed to open: " << FileName << std::endl;
    }
    std::stringstream generic_payload;
    if (infile.is_open()) {
        infile.seekg(0, infile.end);
        int length = static_cast<int>(infile.tellg());
        infile.seekg(0, infile.beg);
        EXPECT_GT(length, 0);

        generic_payload << infile.rdbuf();
        infile.close();
    }

    uint16_t block_payload_size = static_cast<uint16_t>(generic_payload.str().size());

    genie::core::record::annotation_access_unit::BlockPayload block_payload(
        descriptor_ID, num_chrs, genotype_payload, likelihood_payload, cm_bin_payload, cm_mat_payload,
        block_payload_size, generic_payload);

    std::stringstream outputData;
    genie::core::Writer writer(&outputData);
    block_payload.write(writer);
    EXPECT_GE(outputData.str().size(), block_payload_size);
}

TEST_F(MpeggBSCTests, testBlock) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    genie::core::record::annotation_parameter_set::DescriptorID descriptor_ID =
        genie::core::record::annotation_parameter_set::DescriptorID::ATTRIBUTE;
    uint8_t num_chrs = 1;
    genie::core::record::annotation_access_unit::GenotypePayload genotype_payload;
    genie::core::record::annotation_access_unit::LikelihoodPayload likelihood_payload;
    std::vector<genie::core::record::annotation_access_unit::ContactMatrixBinPayload> cm_bin_payload;
    genie::core::record::annotation_access_unit::ContactMatrixMatPayload cm_mat_payload;

    std::string FileName = "TestFiles/exampleMGrecs/ALL.chrX.10000.site";
    std::ifstream infile(FileName, std::ios::binary | std::ios::in);
    if (infile.fail()) {
        std::cout << "failed to open: " << FileName << std::endl;
    }
    std::stringstream generic_payload;
    if (infile.is_open()) {
        infile.seekg(0, infile.end);
        int length = static_cast<int>(infile.tellg());
        infile.seekg(0, infile.beg);
        EXPECT_GT(length, 0);

        generic_payload << infile.rdbuf();
        infile.close();
    }

    uint16_t block_payload_size = static_cast<uint16_t>(generic_payload.str().size());

    genie::core::record::annotation_access_unit::BlockPayload block_payload(
        descriptor_ID, num_chrs, genotype_payload, likelihood_payload, cm_bin_payload, cm_mat_payload,
        block_payload_size, generic_payload);
    genie::core::record::annotation_access_unit::BlockHeader blockHeader(false, descriptor_ID, 1, false,
                                                                         block_payload_size);
    genie::core::record::annotation_access_unit::Block block(blockHeader, block_payload, num_chrs);

    std::stringstream outputData;
    genie::core::Writer writer(&outputData);
    block.write(writer);
    EXPECT_GE(outputData.str().size(), block_payload_size);
}

TEST_F(MpeggBSCTests, testCode) {  // NOLINT(cert-err58-cpp)
                                   // The rule of thumb is to use EXPECT_* when you want the test to continue
                                   // to reveal more errors after the assertion failure, and use ASSERT_*
                                   // when continuing after failure doesn't make sense.
    std::string inputFileName = "TestFiles/exampleMGrecs/ALL.chrX.10000.site";
    std::string OutputFileName = "TestFiles/testCode.mgb";

    std::ifstream infile;
    infile.open(inputFileName, std::ios::binary);

    std::stringstream wholeTestFile;
    std::stringstream compressedData; 
    if (infile.is_open()) {
        wholeTestFile << infile.rdbuf();
        infile.close();
    }

    unsigned char *dest = NULL;
    size_t destLen = 0;
    size_t srcLen = wholeTestFile.str().size();
    std::cout << "source length: " << std::to_string(srcLen) << std::endl;

    mpegg_bsc_compress(&dest, &destLen, (const unsigned char *)wholeTestFile.str().c_str(), srcLen,
                       MPEGG_BSC_DEFAULT_LZPHASHSIZE, MPEGG_BSC_DEFAULT_LZPMINLEN, MPEGG_BSC_BLOCKSORTER_BWT,
                       MPEGG_BSC_CODER_QLFC_STATIC);
    //for (auto i = 0; i < destLen; ++i) compressedData << dest[i];
    

     compressedData.write((const char *)dest, destLen);
   
    auto compressed_dataSize = compressedData.str().size();
    std::cout << "compressedData length: " << std::to_string(compressed_dataSize) << std::endl;
}

TEST_F(MpeggBSCTests, DISABLED_ExampleTest) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    uint64_t fileSize = 197974;
    uint8_t AT_coord_size = 0;
    genie::core::record::annotation_parameter_set::DescriptorID descriptorID =
        genie::core::record::annotation_parameter_set::DescriptorID::ATTRIBUTE;

    //-----------------------descriiptor_configuration----------------------------//
    uint8_t encodingMode = 3;  // BSC
    genie::core::record::annotation_parameter_set::ContactMatrixParameters contactMatrixParameters;
    genie::core::record::annotation_parameter_set::LikelihoodParameters likelihoodParameters;
    genie::core::record::annotation_parameter_set::GenotypeParameters genotypeParameters;

    uint8_t n_pars = 1;
    std::vector<uint8_t> par_ID(n_pars, 0);
    std::vector<uint8_t> par_type(n_pars, 4);
    std::vector<uint8_t> par_num_array_dims(n_pars, 0);
    par_num_array_dims = {0};
    std::vector<std::vector<uint8_t>> par_array_dims(n_pars, std::vector<uint8_t>(0));
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val(
        n_pars, std::vector<std::vector<std::vector<std::vector<uint8_t>>>>(0));
    genie::core::record::annotation_parameter_set::AlgorithmParameters algorithmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);

    genie::core::record::annotation_parameter_set::DescriptorConfiguration descriptorConfigration(
        descriptorID, encodingMode, genotypeParameters, likelihoodParameters, contactMatrixParameters,
        algorithmParameters);
    //-----------------------------------------------------------------------------//
    //--------- annotation encoding parameters ------------------------------------//
    genie::core::record::annotation_parameter_set::CompressorParameterSet compressorParameterSet;
    genie::core::record::annotation_parameter_set::AttributeParameterSet attributeParameterSet;

    uint8_t n_filter = 0;
    std::vector<uint8_t> filter_ID_len{};
    std::vector<std::string> filter_ID{};
    std::vector<uint16_t> desc_len{};
    std::vector<std::string> description{};

    uint8_t n_features_names = 0;
    std::vector<uint8_t> feature_name_len{};
    std::vector<std::string> feature_name{};

    uint8_t n_ontology_terms = 0;
    std::vector<uint8_t> ontology_term_name_len{};
    std::vector<std::string> ontology_term_name{};

    uint8_t n_descriptors = 1;
    std::vector<genie::core::record::annotation_parameter_set::DescriptorConfiguration> descriptor_configuration(
        n_descriptors, descriptorConfigration);

    uint8_t n_compressors = 0;
    std::vector<genie::core::record::annotation_parameter_set::CompressorParameterSet> compressor_parameter_set(
        n_compressors, genie::core::record::annotation_parameter_set::CompressorParameterSet());

    uint8_t n_attributes = 0;
    std::vector<genie::core::record::annotation_parameter_set::AttributeParameterSet> attribute_parameter_set(
        n_attributes, genie::core::record::annotation_parameter_set::AttributeParameterSet());

    genie::core::record::annotation_parameter_set::AnnotationEncodingParameters annotationEncodingParameters(
        n_filter, filter_ID_len, filter_ID, desc_len, description, n_features_names, feature_name_len, feature_name,
        n_ontology_terms, ontology_term_name_len, ontology_term_name, n_descriptors, descriptor_configuration,
        n_compressors, compressor_parameter_set, n_attributes, attribute_parameter_set);
    //-----------------------------------------------------------------------------//
    //--------- annotation parameter set ------------------------------------------//
    bool variable_size_tiles = false;
    uint64_t n_tiles = 1;
    std::vector<std::vector<uint64_t>> start_index;
    std::vector<std::vector<uint64_t>> end_index;
    std::vector<uint64_t> tile_size;
    tile_size.resize(n_tiles, fileSize);

    genie::core::record::annotation_parameter_set::TileStructure defaultTileStructure(
        AT_coord_size, false, variable_size_tiles, n_tiles, start_index, end_index, tile_size);

    uint8_t AG_class = 1;
    bool attribute_contiguity = false;

    bool two_dimensional = false;

    bool column_major_tile_order = false;
    uint8_t symmetry_mode = 0;
    bool symmetry_minor_diagonal = false;

    bool attribute_dependent_tiles = false;

    uint16_t n_add_tile_structures = 0;
    std::vector<uint16_t> nAttributes{};
    std::vector<std::vector<uint16_t>> attribute_IDs;
    std::vector<uint8_t> nDescriptors{};
    std::vector<std::vector<uint8_t>> descriptor_IDs;
    std::vector<genie::core::record::annotation_parameter_set::TileStructure> additional_tile_structure;

    genie::core::record::annotation_parameter_set::TileConfiguration tileConfiguration(
        AT_coord_size, AG_class, attribute_contiguity, two_dimensional, column_major_tile_order, symmetry_mode,
        symmetry_minor_diagonal, attribute_dependent_tiles, defaultTileStructure, n_add_tile_structures, nAttributes,
        attribute_IDs, nDescriptors, descriptor_IDs, additional_tile_structure);

    uint8_t parameter_set_ID = 1;
    uint8_t AT_ID = 1;
    uint8_t AT_alphabet_ID = 0;
    bool AT_pos_40_bits_flag = false;
    uint8_t n_aux_attribute_groups = 0;
    std::vector<genie::core::record::annotation_parameter_set::TileConfiguration> tile_configuration(1,
                                                                                                     tileConfiguration);

    genie::core::record::annotation_parameter_set::Record annotationParameterSet(
        parameter_set_ID, AT_ID, AT_alphabet_ID, AT_coord_size, AT_pos_40_bits_flag, n_aux_attribute_groups,
        tile_configuration, annotationEncodingParameters);
    //---------------------------------------------------------//
    //---------------------annotation access unit  ------------//

    bool is_attribute = false;
    uint16_t attribute_ID = false;
    uint64_t n_tiles_per_col = 0;
    uint64_t n_tiles_per_row = 0;
    uint64_t n_blocks = 1;
    uint64_t tile_index_1 = 0;
    bool tile_index_2_exists = false;
    uint64_t tile_index_2 = 0;
    genie::core::record::annotation_access_unit::AnnotationAccessUnitHeader annotation_access_unit_header(
        attribute_contiguity, two_dimensional, column_major_tile_order, variable_size_tiles, AT_coord_size,
        is_attribute, attribute_ID, descriptorID, n_tiles_per_col, n_tiles_per_row, n_blocks, tile_index_1,
        tile_index_2_exists, tile_index_2);

    bool indexed = false;
    uint32_t block_payload_size = static_cast<uint32_t>(fileSize);

    genie::core::record::annotation_access_unit::BlockHeader block_header(attribute_contiguity, descriptorID,
                                                                          attribute_ID, indexed, block_payload_size);

    genie::core::record::annotation_access_unit::GenotypePayload genotype_payload;
    genie::core::record::annotation_access_unit::LikelihoodPayload likelihood_payload;
    genie::core::record::annotation_access_unit::ContactMatrixMatPayload cm_mat_payload;
    std::vector<genie::core::record::annotation_access_unit::ContactMatrixBinPayload> cm_bin_payload;
    std::string filename = "testFiles/exampleMGrecs/ALL.chrX.10000_site.compressed";
    std::ifstream compressedFile;
    compressedFile.open(filename, std::ios::binary | std::ios::in);

    std::stringstream stringPayload;
    stringPayload << compressedFile.rdbuf();
    std::stringstream& stringPayload1 = stringPayload;
    // std::string generic_payload = stringPayload.str();
    uint16_t payloadSize = static_cast<uint16_t>(stringPayload.str().size());
    uint8_t num_chrs = 0;

    genie::core::record::annotation_access_unit::BlockPayload block_payload(
        descriptorID, num_chrs, genotype_payload, likelihood_payload, cm_bin_payload, cm_mat_payload, payloadSize,
        stringPayload1);
    stringPayload << "boe";

    std::vector<genie::core::record::annotation_access_unit::Block> blocks;
    genie::core::record::annotation_access_unit::Block block(block_header, block_payload, num_chrs);
    blocks.push_back(block);

    genie::core::record::annotation_access_unit::AnnotationType AT_type =
        genie::core::record::annotation_access_unit::AnnotationType::VARIANTS;
    uint8_t AT_subtype = 1;

    genie::core::record::annotation_access_unit::Record annotationAccessUnit(
        AT_ID, AT_type, AT_subtype, AG_class, annotation_access_unit_header, blocks, attribute_contiguity,
        two_dimensional, column_major_tile_order, AT_coord_size, n_blocks, num_chrs);

    //--------------------------------------------------------//
#if GENERATE_TEST_FILES
    {
        std::string name = "TestFiles/dataunit_annotation_31_BSC";

        std::ofstream testfile;
        testfile.open(name + ".bin", std::ios::binary | std::ios::out);
        uint8_t data_unit_type = 3;
        uint64_t data_unit_size = 24;
        if (testfile.is_open()) {
            genie::core::Writer writer(&testfile);
            writer.write(data_unit_type, 8);
            writer.write(0, 10);
            writer.write(data_unit_size, 22);
            annotationParameterSet.write(writer);
            writer.flush();
            testfile.close();
        }
        std::ofstream txtfile;
        txtfile.open(name + ".txt", std::ios::out);
        if (txtfile.is_open()) {
            genie::core::Writer txtWriter(&txtfile, true);
            txtfile << std::to_string(data_unit_type) << ",";
            txtfile << std::to_string(data_unit_size) << ",";
            annotationParameterSet.write(txtWriter);
            txtfile.close();
        }
    }
    {
        std::string name = "TestFiles/dataunit_access_31_BSC";
        std::ofstream testfile;
        testfile.open(name + ".bin", std::ios::binary | std::ios::out);
        uint8_t data_unit_type = 3;
        uint64_t data_unit_size = 197993;
        if (testfile.is_open()) {
            genie::core::Writer writer(&testfile);
            writer.write(data_unit_type, 8);
            writer.write(0, 10);
            writer.write(data_unit_size, 22);
            annotationAccessUnit.write(writer);
            writer.flush();
            testfile.close();
        }
    }
    {
        std::string name = "TestFiles/dataunit_combined_31_BSC";
        std::ofstream testfile;
        testfile.open(name + ".bin", std::ios::binary | std::ios::out);
        uint8_t data_unit_type = 3;
        uint64_t data_unit_size = 24;
        if (testfile.is_open()) {
            genie::core::Writer writer(&testfile);
            writer.write(data_unit_type, 8);
            writer.write(0, 10);
            writer.write(data_unit_size, 22);
            annotationParameterSet.write(writer);
            data_unit_type = 3;
            data_unit_size = 197993;
            writer.write(data_unit_type, 8);
            writer.write(0, 10);
            writer.write(data_unit_size, 22);
            annotationAccessUnit.write(writer);
            writer.flush();
            testfile.close();
        }
    }

#endif

    EXPECT_EQ(0, 0);
    ASSERT_EQ(0, 0);
}