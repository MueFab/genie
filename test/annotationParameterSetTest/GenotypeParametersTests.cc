
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include <gtest/gtest.h>

#include "RandomRecordFillIn.h"
#include "genie/core/record/annotation_parameter_set/GenotypeParameters.h"
// ---------------------------------------------------------------------------------------------------------------------

class GenotypeParametersTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    GenotypeParametersTests() = default;

    ~GenotypeParametersTests() override = default;

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

TEST_F(GenotypeParametersTests, GenotypeParametersConstructValues) {  // NOLINT(cert-err58-cpp)
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

TEST_F(GenotypeParametersTests, GenotypeParameterWriteRandom) {  // NOLINT(cert-err58-cpp)
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

TEST_F(GenotypeParametersTests, AttributeParameterSetZeros) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    genie::core::record::annotation_parameter_set::AttributeParameterSet attributeParameterSet;

    EXPECT_EQ(attributeParameterSet.getCompressorID(), 0);
}
