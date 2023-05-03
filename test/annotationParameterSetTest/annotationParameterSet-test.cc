#include <gtest/gtest.h>

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include "RandomRecordFillIn.h"
#include "genie/core/record/annotation_encoding_parameters/record.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#define GENERATE_TEST_FILES true

class AnnotationParameterSetTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    AnnotationParameterSetTests() = default;

    ~AnnotationParameterSetTests() override = default;

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
        srand(static_cast<unsigned int>(time(NULL)));
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }
    // void sharedSubroutine() {
    //    // If needed, define subroutines for your tests to share
    // }
};

TEST_F(AnnotationParameterSetTests, LikelihoodConstructZeros) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    genie::core::record::annotation_parameter_set::LikelihoodParameters likelihoodParameters;

    EXPECT_FALSE(likelihoodParameters.isTransformFlag());
    EXPECT_EQ(likelihoodParameters.getDtypeID(), 0);
    EXPECT_EQ(likelihoodParameters.getNumGLPerSample(), 0);
}
TEST_F(AnnotationParameterSetTests, Likelihoodtestrandom) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    genie::core::record::annotation_parameter_set::LikelihoodParameters likelihoodParameters;
    genie::core::record::annotation_parameter_set::LikelihoodParameters likelihoodParametersCheck;
    RandomAnnotationEncodingParameters randomLikelihood;
    likelihoodParameters = randomLikelihood.randomLikelihood();

    std::stringstream outputfile(std::stringstream::in | std::stringstream::out | std::stringstream::binary);

    genie::util::BitWriter strwriter(&outputfile);
    genie::util::BitReader strreader(outputfile);
    likelihoodParameters.write(strwriter);
    strwriter.flush();
    likelihoodParametersCheck.read(strreader);

    //    EXPECT_EQ(likelihoodParameters.getDtypeID(), likelihoodParametersCheck.getDtypeID());
    EXPECT_EQ(likelihoodParameters.getNumGLPerSample(), likelihoodParametersCheck.getNumGLPerSample());
    EXPECT_EQ(likelihoodParameters.isTransformFlag(), likelihoodParametersCheck.isTransformFlag());

#if GENERATE_TEST_FILES
    std::string name = "TestFiles/LikelihoodParameters_seed_";
    name += std::to_string(rand() % 10);

    std::ofstream testfile;
    testfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (testfile.is_open()) {
        genie::util::BitWriter writer(&testfile);
        likelihoodParameters.write(writer);
        writer.flush();
        testfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        likelihoodParameters.write(txtfile);
        txtfile.close();
    }

#endif


}

TEST_F(AnnotationParameterSetTests, LikelihoodConstructValues) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    genie::core::record::annotation_parameter_set::LikelihoodParameters likelihoodParameters(128, true, 45);

    EXPECT_TRUE(likelihoodParameters.isTransformFlag());
    EXPECT_EQ(likelihoodParameters.getDtypeID(), 45);
    EXPECT_EQ(likelihoodParameters.getNumGLPerSample(), 128);
}

TEST_F(AnnotationParameterSetTests, GenotypeParametersConstructValues) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    uint8_t max_ploidy = 1;
    bool no_reference_flag = true;
    bool not_available_flag = false;
    genie::core::record::annotation_parameter_set::BinarizationID binarization_ID =
        genie::core::record::annotation_parameter_set::BinarizationID::BIT_PLANE;
    uint8_t num_bit_plane = 2;
    genie::core::record::annotation_parameter_set::ConcatAxis concat_axis =
        genie::core::record::annotation_parameter_set::ConcatAxis::DO_NOT_CONCAT;
    std::vector<bool> sort_variants_rows_flag{false, true, true, false, false};
    std::vector<bool> sort_variants_cols_flag{true, true, false, false, true};
    std::vector<bool> transpose_variants_mat_flag{true, false, false, true, true};
    std::vector<uint8_t> variants_codec_ID{6, 7, 8, 9, 10};

    bool encode_phases_data_flag = false;
    bool sort_phases_rows_flag = true;
    bool sort_phases_cols_flag = true;
    bool transpose_phases_mat_flag = false;
    bool phases_codec_ID = true;
    bool phases_value = true;

    genie::core::record::annotation_parameter_set::GenotypeParameters genotypeParameters(
        max_ploidy, no_reference_flag, not_available_flag, binarization_ID, num_bit_plane, concat_axis,
        sort_variants_rows_flag, sort_variants_cols_flag, transpose_variants_mat_flag, variants_codec_ID,
        encode_phases_data_flag, sort_phases_rows_flag, sort_phases_cols_flag, transpose_phases_mat_flag,
        phases_codec_ID, phases_value);

    EXPECT_EQ(genotypeParameters.getMaxPloidy(), max_ploidy);
    EXPECT_EQ(genotypeParameters.isNoReference(), no_reference_flag);
    EXPECT_EQ(genotypeParameters.isNotAvailable(), not_available_flag);
    EXPECT_EQ(genotypeParameters.getBinarizationID(), binarization_ID);
    EXPECT_EQ(genotypeParameters.getNumberOfBitPlane(), num_bit_plane);
    EXPECT_EQ(genotypeParameters.getConcatAxis(), concat_axis);
    EXPECT_EQ(genotypeParameters.getSortVariantsRowsFlags(), sort_variants_rows_flag);
    EXPECT_EQ(genotypeParameters.getSortVariantsColsFlags(), sort_variants_cols_flag);
    EXPECT_EQ(genotypeParameters.getTransposeVariantsMatFlags(), transpose_variants_mat_flag);
    EXPECT_EQ(genotypeParameters.getVariantsCodecIDs(), variants_codec_ID);

    EXPECT_EQ(genotypeParameters.isEncodePhaseData(), encode_phases_data_flag);
    EXPECT_EQ(genotypeParameters.isSortPhasesRows(), sort_phases_rows_flag);
    EXPECT_EQ(genotypeParameters.isSortPhasesCols(), sort_phases_cols_flag);
    EXPECT_EQ(genotypeParameters.isTransposePhaseMat(), transpose_phases_mat_flag);
    EXPECT_EQ(genotypeParameters.isPhasesCodecID(), phases_codec_ID);
    EXPECT_EQ(genotypeParameters.isPhasesValue(), phases_value);
}


TEST_F(AnnotationParameterSetTests, GenotypeParameterWriteRandom) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    RandomAnnotationEncodingParameters randomgenotypeParameters;

    genie::core::record::annotation_parameter_set::GenotypeParameters genotypeParameters;
    genie::core::record::annotation_parameter_set::GenotypeParameters genotypeParametersCheck;
    genotypeParameters = randomgenotypeParameters.randomGenotypeParameters();

    std::stringstream InOut;
    genie::util::BitWriter strwriter(&InOut);
    genie::util::BitReader strreader(InOut);
    genotypeParameters.write(strwriter);
    strwriter.flush();
    genotypeParametersCheck.read(strreader);

    EXPECT_EQ(genotypeParameters.getBinarizationID(), genotypeParametersCheck.getBinarizationID());
    EXPECT_EQ(genotypeParameters.getConcatAxis(), genotypeParametersCheck.getConcatAxis());
    EXPECT_EQ(genotypeParameters.getMaxPloidy(), genotypeParametersCheck.getMaxPloidy());
    EXPECT_EQ(genotypeParameters.getNumberOfBitPlane(), genotypeParametersCheck.getNumberOfBitPlane());
    EXPECT_EQ(genotypeParameters.getSortVariantsColsFlags(), genotypeParametersCheck.getSortVariantsColsFlags());
    EXPECT_EQ(genotypeParameters.getSortVariantsRowsFlags(), genotypeParametersCheck.getSortVariantsRowsFlags());
    EXPECT_EQ(genotypeParameters.getTransposeVariantsMatFlags(),
              genotypeParametersCheck.getTransposeVariantsMatFlags());
    EXPECT_EQ(genotypeParameters.getVariantsCodecIDs(), genotypeParametersCheck.getVariantsCodecIDs());
    EXPECT_EQ(genotypeParameters.isEncodePhaseData(), genotypeParametersCheck.isEncodePhaseData());
    EXPECT_EQ(genotypeParameters.isNoReference(), genotypeParametersCheck.isNoReference());
    EXPECT_EQ(genotypeParameters.isNotAvailable(), genotypeParametersCheck.isNotAvailable());
    EXPECT_EQ(genotypeParameters.isPhasesCodecID(), genotypeParametersCheck.isPhasesCodecID());
    EXPECT_EQ(genotypeParameters.isPhasesValue(), genotypeParametersCheck.isPhasesValue());
    EXPECT_EQ(genotypeParameters.isSortPhasesCols(), genotypeParametersCheck.isSortPhasesCols());
    EXPECT_EQ(genotypeParameters.isSortPhasesRows(), genotypeParametersCheck.isSortPhasesRows());
    EXPECT_EQ(genotypeParameters.isTransposePhaseMat(), genotypeParametersCheck.isTransposePhaseMat());

#if GENERATE_TEST_FILES
    std::string name = "TestFiles/GenotypeParameters_seed_";
    name += std::to_string(rand() % 10);

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::util::BitWriter writer(&outputfile);
        genotypeParameters.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        genotypeParameters.write(txtfile);
        txtfile.close();
    }

#endif
}

TEST_F(AnnotationParameterSetTests, AttributeParameterSetZeros) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    genie::core::record::annotation_parameter_set::AttributeParameterSet attributeParameterSet;

    EXPECT_EQ(attributeParameterSet.getCompressorID(), 0);
}

TEST_F(AnnotationParameterSetTests, AttributeParameterSetValues) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    uint16_t attribute_ID = 1;
    uint8_t attribute_name_len = 5;
    std::string attribute_name = "12345";
    uint8_t attribute_type = 1;
    uint8_t attribute_num_array_dims = 5;
    std::vector<uint8_t> attribute_array_dims = {1, 2, 3, 4, 5};

    std::vector<uint8_t> attribute_default_val = {9, 8, 7, 6, 5};
    bool attribute_miss_val_flag = true;
    bool attribute_miss_default_flag = false;
    std::vector<uint8_t> attribute_miss_val = {4, 3, 2, 1, 0};

    std::string attribute_miss_str = "qwerty";
    uint8_t compressor_ID = 4;

    uint8_t n_steps_with_dependencies = 3;
    std::vector<uint8_t> dependency_step_ID = {7, 5, 3};
    std::vector<uint8_t> n_dependencies = {2, 2, 2};
    std::vector<std::vector<uint8_t>> dependency_var_ID{{1, 1}, {2, 2}, {3, 3}, {4, 4}, {0, 0}};
    std::vector<std::vector<bool>> dependency_is_attribute{
        {true, true}, {false, false}, {false, true}, {true, false}, {true, true}};
    std::vector<std::vector<uint16_t>> dependency_ID{{3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}};

    genie::core::record::annotation_parameter_set::AttributeParameterSet attributeParameterSet(
        attribute_ID, attribute_name_len, attribute_name, attribute_type, attribute_num_array_dims,
        attribute_array_dims, attribute_default_val, attribute_miss_val_flag, attribute_miss_default_flag,
        attribute_miss_val, attribute_miss_str, compressor_ID, n_steps_with_dependencies, dependency_step_ID,
        n_dependencies, dependency_var_ID, dependency_is_attribute, dependency_ID);

    EXPECT_EQ(attributeParameterSet.getCompressorID(), 4);
}

TEST_F(AnnotationParameterSetTests, DISABLED_AttributeParameterSetRandom) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    RandomAnnotationEncodingParameters randomattributeParameterSet;
    genie::core::record::annotation_parameter_set::AttributeParameterSet attributeParameterSet;
    genie::core::record::annotation_parameter_set::AttributeParameterSet attributeParameterSetCheck;
    attributeParameterSet = randomattributeParameterSet.randomAttributeParameterSet();

    std::stringstream InOut(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
    genie::util::BitWriter strwriter(&InOut);
    genie::util::BitReader strreader(InOut);
    attributeParameterSet.write(strwriter);
    strwriter.flush();
    attributeParameterSetCheck.read(strreader);

    //    EXPECT_EQ(attributeParameterSet.areDependeciesAttributes(),
    //    attributeParameterSetCheck.areDependeciesAttributes());
    //    EXPECT_EQ(attributeParameterSet.getAttributeArrayDims(), attributeParameterSetCheck.getAttributeArrayDims());
    EXPECT_EQ(attributeParameterSet.getAttributeDefaultValue(), attributeParameterSetCheck.getAttributeDefaultValue());
    EXPECT_EQ(attributeParameterSet.getAttributeMissedString(), attributeParameterSetCheck.getAttributeMissedString());
    EXPECT_EQ(attributeParameterSet.getAttributeMissedValues(), attributeParameterSetCheck.getAttributeMissedValues());
    EXPECT_EQ(attributeParameterSet.getAttributeName(), attributeParameterSetCheck.getAttributeName());
    EXPECT_EQ(attributeParameterSet.getAttributeNameLength(), attributeParameterSetCheck.getAttributeNameLength());
    EXPECT_EQ(attributeParameterSet.getAttributeNumberOFArrayDims(),
              attributeParameterSetCheck.getAttributeNumberOFArrayDims());
    EXPECT_EQ(attributeParameterSet.getAttributeType(), attributeParameterSetCheck.getAttributeType());
    EXPECT_EQ(attributeParameterSet.getAttriubuteID(), attributeParameterSetCheck.getAttriubuteID());

#if GENERATE_TEST_FILES

    std::string name = "TestFiles/AttributeParameterSet_seed_";
    name += std::to_string(rand() % 10);

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::util::BitWriter writer(&outputfile);
        attributeParameterSet.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        attributeParameterSet.write(txtfile);
        txtfile.close();
    }
#endif
}

TEST_F(AnnotationParameterSetTests, AlgorithmParametersZero) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    genie::core::record::annotation_parameter_set::AlgorithmParameters algortihmParameters;

    EXPECT_EQ(algortihmParameters.getNumberOfPars(), 0);
    EXPECT_EQ(algortihmParameters.getParArrayDims().size(), 0);
    EXPECT_EQ(algortihmParameters.getParIDs().size(), 0);
    EXPECT_EQ(algortihmParameters.getParNumberOfArrayDims().size(), 0);
    EXPECT_EQ(algortihmParameters.getParTypes().size(), 0);
    EXPECT_EQ(algortihmParameters.getParValues().size(), 0);
}

TEST_F(AnnotationParameterSetTests, AlgorithmParametersWithValues) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    uint8_t n_pars = 3;
    std::vector<uint8_t> par_ID{8, 6, 0};
    std::vector<uint8_t> par_type{1, 3, 4};
    std::vector<uint8_t> par_num_array_dims{1, 1};
    std::vector<std::vector<uint8_t>> par_array_dims{{7}, {5}};
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val{{{{{5}}}}, {{{{3}}}}, {{{{2}}}}};

    genie::core::record::annotation_parameter_set::AlgorithmParameters algortihmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);

    EXPECT_EQ(algortihmParameters.getNumberOfPars(), n_pars);
    EXPECT_EQ(algortihmParameters.getParArrayDims().size(), par_num_array_dims.size());
    EXPECT_EQ(algortihmParameters.getParIDs().size(), par_ID.size());
    EXPECT_EQ(algortihmParameters.getParNumberOfArrayDims()[0], par_num_array_dims[0]);
    EXPECT_EQ(algortihmParameters.getParTypes()[1], par_type[1]);
    EXPECT_EQ(algortihmParameters.getParValues()[0][0][0][0], par_val[0][0][0][0]);
}

TEST_F(AnnotationParameterSetTests, AlgorithmParametersRandom) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    RandomAnnotationEncodingParameters randomAlgorithmParameters;
    genie::core::record::annotation_parameter_set::AlgorithmParameters algorithmParameters;
    algorithmParameters = randomAlgorithmParameters.randomAlgorithmParameters();

    std::string name = "TestFiles/AlgorithmParameters_seed_";
    name += std::to_string(rand() % 10);
#if GENERATE_TEST_FILES

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::util::BitWriter writer(&outputfile);
        algorithmParameters.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        algorithmParameters.write(txtfile);
        txtfile.close();
    }

#endif
}

TEST_F(AnnotationParameterSetTests, TileStructureZeros) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    genie::core::record::annotation_parameter_set::TileStructure tileStructure;

    EXPECT_FALSE(tileStructure.isVariableSizeTiles());
    EXPECT_EQ(tileStructure.getALLEndIndices().size(), 0);
    EXPECT_EQ(tileStructure.getAllStartIndices().size(), 0);
    EXPECT_EQ(tileStructure.getAllTileSizes().size(), 0);
    EXPECT_EQ(tileStructure.getNumberOfTiles(), 0);
}

TEST_F(AnnotationParameterSetTests, TileStructurevalues) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    uint8_t ATCoordSize = 3;
    bool two_dimensional = false;
    bool variable_size_tiles = true;
    uint64_t n_tiles = 4;
    std::vector<std::vector<uint64_t>> start_index{{1}, {2}, {3}, {4}};
    std::vector<std::vector<uint64_t>> end_index{{5}, {6}, {7}, {8}};
    std::vector<uint64_t> tile_size = {8, 16, 25, 64};
    genie::core::record::annotation_parameter_set::TileStructure tileStructure(
        two_dimensional, ATCoordSize, variable_size_tiles, n_tiles, start_index, end_index, tile_size);
    EXPECT_TRUE(tileStructure.isVariableSizeTiles());
    EXPECT_EQ(tileStructure.getALLEndIndices().size(), n_tiles);
    EXPECT_EQ(tileStructure.getAllStartIndices()[3], start_index[3]);
    EXPECT_EQ(tileStructure.getAllTileSizes()[1], tile_size[1]);
    EXPECT_EQ(tileStructure.getNumberOfTiles(), n_tiles);
}

TEST_F(AnnotationParameterSetTests, TileStructureRandom) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    RandomAnnotationEncodingParameters RandomTileStructure;
    genie::core::record::annotation_parameter_set::TileStructure tileStructure;
    uint8_t ATCoordSize = static_cast<uint8_t>(rand() % 4);
    bool two_dimensional = static_cast<bool>(rand() % 2);
    tileStructure = RandomTileStructure.randomTileStructure(ATCoordSize, two_dimensional);

    std::string name = "TestFiles/TileStructure_seed_";
    name += std::to_string(rand() % 10);

#if GENERATE_TEST_FILES

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::util::BitWriter writer(&outputfile);
        tileStructure.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        txtfile << std::to_string(two_dimensional) << ",";
        txtfile << std::to_string(ATCoordSize) << ",";
        tileStructure.write(txtfile);
        txtfile.close();
    }
#endif
}

TEST_F(AnnotationParameterSetTests, TileConfigurationZeros) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    genie::core::record::annotation_parameter_set::TileConfiguration tileConfiguration;

    EXPECT_EQ(tileConfiguration.getAttributeGroupClass(), 0);
    EXPECT_EQ(tileConfiguration.getAttributeIDs().size(), 0);
}

TEST_F(AnnotationParameterSetTests, TileConfigurationRandom) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    RandomAnnotationEncodingParameters randomTileConfiguration;
    genie::core::record::annotation_parameter_set::TileConfiguration tileConfiguration;

    tileConfiguration = randomTileConfiguration.randomTileConfiguration();

#if GENERATE_TEST_FILES
    uint8_t randomOrder = static_cast<uint8_t>(rand() % 10);
    std::string name = "TestFiles/TileConfugration_seed_";
    name += std::to_string(randomOrder);

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::util::BitWriter writer(&outputfile);
        tileConfiguration.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        tileConfiguration.write(txtfile);
        txtfile.close();
    }
#endif
}

TEST_F(AnnotationParameterSetTests, TileConfigurationRandomSimpleStructure) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    RandomAnnotationEncodingParameters randomTileConfiguration;
    genie::core::record::annotation_parameter_set::TileConfiguration tileConfiguration;
    uint8_t ATCoordSize = static_cast<uint8_t>(rand() % 4);
    tileConfiguration = randomTileConfiguration.randomTileConfiguration(ATCoordSize);
    genie::core::record::annotation_parameter_set::TileConfiguration tileConfigurationCheck;

    std::stringstream InOut;
    //(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
    genie::util::BitWriter strwriter(&InOut);
    genie::util::BitReader strreader(InOut);
    tileConfiguration.write(strwriter);
    strwriter.flush();
    tileConfigurationCheck.read(strreader);

    EXPECT_EQ(tileConfiguration.getAttributeGroupClass(), tileConfigurationCheck.getAttributeGroupClass());
    EXPECT_EQ(tileConfiguration.isAttributeContiguity(), tileConfigurationCheck.isAttributeContiguity());
    EXPECT_EQ(tileConfiguration.isTwoDimensional(), tileConfigurationCheck.isTwoDimensional());

#if GENERATE_TEST_FILES
    uint8_t randomOrder = static_cast<uint8_t>(rand() % 10);
    std::string name = "TestFiles/TileConfugrationSimple_seed_";
    name += std::to_string(randomOrder);

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::util::BitWriter writer(&outputfile);
        tileConfiguration.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        txtfile << std::to_string(ATCoordSize) << ",";
        tileConfiguration.write(txtfile);
        txtfile.close();
    }
#endif
}

TEST_F(AnnotationParameterSetTests, ContactMatrixZeros) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    genie::core::record::annotation_parameter_set::ContactMatrixParameters cmParameters;
    EXPECT_EQ(cmParameters.getInterval(), uint8_t(0));
    EXPECT_EQ(cmParameters.getNumberOfSamples(), uint8_t(0));
    EXPECT_EQ(cmParameters.getNumberOfNormalizationMethods(), uint8_t(0));
}


TEST_F(AnnotationParameterSetTests, ContactMatrixRandom) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    RandomAnnotationEncodingParameters RandomContactMatrixParameters;
    genie::core::record::annotation_parameter_set::ContactMatrixParameters contactMatrixParameters;
    genie::core::record::annotation_parameter_set::ContactMatrixParameters contactMatrixParametersCheck;

    contactMatrixParameters = RandomContactMatrixParameters.randomContactMatrixParameters();

    std::stringstream InOut;
    //(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
    genie::util::BitWriter strwriter(&InOut);
    genie::util::BitReader strreader(InOut);
    contactMatrixParameters.write(strwriter);
    strwriter.flush();
    contactMatrixParametersCheck.read(strreader);

    EXPECT_EQ(contactMatrixParameters.getNumberOfSamples(), contactMatrixParametersCheck.getNumberOfSamples());
    EXPECT_EQ(contactMatrixParameters.getSampleIDs(), contactMatrixParametersCheck.getSampleIDs());
    EXPECT_EQ(contactMatrixParameters.getSampleNames(), contactMatrixParametersCheck.getSampleNames());
    EXPECT_EQ(contactMatrixParameters.getNumberOfChromosomes(), contactMatrixParametersCheck.getNumberOfChromosomes());
    EXPECT_EQ(contactMatrixParameters.getChromosomeIDs(), contactMatrixParametersCheck.getChromosomeIDs());
    EXPECT_EQ(contactMatrixParameters.getChromosomeNames(), contactMatrixParametersCheck.getChromosomeNames());
    EXPECT_EQ(contactMatrixParameters.getChromsomeLength(), contactMatrixParametersCheck.getChromsomeLength());

#if GENERATE_TEST_FILES
    std::string name = "TestFiles/ContactMatrixParameters_seed_";
    name += std::to_string(rand() % 10);

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::util::BitWriter writer(&outputfile);
        contactMatrixParameters.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        contactMatrixParameters.write(txtfile);
        txtfile.close();
    }
#endif
}


TEST_F(AnnotationParameterSetTests, DescriptorConfigurationRandom) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    RandomAnnotationEncodingParameters RandomContactMatrixParameters;
    genie::core::record::annotation_parameter_set::DescriptorConfiguration descriptorConfiguration;
    genie::core::record::annotation_parameter_set::DescriptorConfiguration descriptorConfigurationCheck;

    descriptorConfiguration = RandomContactMatrixParameters.randomDescriptorConfiguration();

    std::stringstream InOut;
    //(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
    genie::util::BitWriter strwriter(&InOut);
    genie::util::BitReader strreader(InOut);
    descriptorConfiguration.write(strwriter);
    strwriter.flush();
    descriptorConfigurationCheck.read(strreader);

    EXPECT_EQ(descriptorConfiguration.getDescriptorID(), descriptorConfigurationCheck.getDescriptorID());
    EXPECT_EQ(descriptorConfiguration.getEncodingModeID(), descriptorConfigurationCheck.getEncodingModeID());

#if GENERATE_TEST_FILES
    std::string name = "TestFiles/DescriptorConfiguration_seed_";
    name += std::to_string(rand() % 10);

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::util::BitWriter writer(&outputfile);
        descriptorConfiguration.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        descriptorConfiguration.write(txtfile);
        txtfile.close();
    }
#endif
}


TEST_F(AnnotationParameterSetTests, CompressorParameterSetZeros) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    genie::core::record::annotation_parameter_set::CompressorParameterSet compressorParameterset;
    EXPECT_EQ(compressorParameterset.getCompressorID(), uint8_t(0));
    EXPECT_EQ(compressorParameterset.getCompressorStepIDs().size(), 0);
    EXPECT_EQ(compressorParameterset.getNumberOfCompletedOutVars().size(), 0);
}


TEST_F(AnnotationParameterSetTests, CompressorParameterSetRandom) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    RandomAnnotationEncodingParameters RandomContactMatrixParameters;
    genie::core::record::annotation_parameter_set::CompressorParameterSet compressorParameterSet;
    genie::core::record::annotation_parameter_set::CompressorParameterSet compressorParameterSetCheck;

    compressorParameterSet = RandomContactMatrixParameters.randomCompressorParameterSet();

    std::stringstream InOut;
    //(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
    genie::util::BitWriter strwriter(&InOut);
    genie::util::BitReader strreader(InOut);
    compressorParameterSet.write(strwriter);
    strwriter.flush();
    compressorParameterSetCheck.read(strreader);

    EXPECT_EQ(compressorParameterSet.getCompressorID(), compressorParameterSetCheck.getCompressorID());
    EXPECT_EQ(compressorParameterSet.getNumberOfCompressorSteps(),
              compressorParameterSetCheck.getNumberOfCompressorSteps());
    EXPECT_EQ(compressorParameterSet.getCompressorStepIDs(), compressorParameterSetCheck.getCompressorStepIDs());
    EXPECT_EQ(compressorParameterSet.getNumberInVars(), compressorParameterSetCheck.getNumberInVars());

#if GENERATE_TEST_FILES
    std::string name = "TestFiles/CompressorParameterSet_seed_";
    name += std::to_string(rand() % 10);

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::util::BitWriter writer(&outputfile);
        compressorParameterSet.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        compressorParameterSet.write(txtfile);
        txtfile.close();
    }
#endif
}
