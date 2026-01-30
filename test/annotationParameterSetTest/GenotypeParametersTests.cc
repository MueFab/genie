
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include <gtest/gtest.h>
#include <string>

#include "RandomRecordFillIn.h"
#include "genie/genotype/genotype_parameters.h"

// ---------------------------------------------------------------------------------------------------------------------
#define GENERATE_TEST_FILES false

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

    genie::genotype::BinarizationID binarization_ID =
        genie::genotype::BinarizationID::BIT_PLANE;
    genie::genotype::ConcatAxis concat_axis =
        genie::genotype::ConcatAxis::DO_NOT_CONCAT;
    bool sort_variants_rows_flag = false;
    bool sort_variants_cols_flag = true;
    bool transpose_variants_mat_flag = true;
    genie::core::AlgoID variants_codec_ID = genie::core::AlgoID::RLE;

    bool encode_phases_data_flag = false;
    bool sort_phases_rows_flag = true;
    bool sort_phases_cols_flag = false;
    bool transpose_phases_mat_flag = true;
    genie::core::AlgoID phases_codec_ID = genie::core::AlgoID::BSC;

    genie::genotype::GenotypeParameters genotypeParameters(
        binarization_ID, concat_axis, sort_variants_rows_flag, sort_variants_cols_flag,
        transpose_variants_mat_flag, variants_codec_ID, encode_phases_data_flag,
        sort_phases_rows_flag, sort_phases_cols_flag, transpose_phases_mat_flag,
        phases_codec_ID);

    EXPECT_EQ(genotypeParameters.GetBinarizationID(), binarization_ID);
    EXPECT_EQ(genotypeParameters.GetConcatAxis(), concat_axis);
    EXPECT_EQ(genotypeParameters.GetSortVariantsRowsFlag(), sort_variants_rows_flag);
    EXPECT_EQ(genotypeParameters.GetSortVariantsColsFlag(), sort_variants_cols_flag);
    EXPECT_EQ(genotypeParameters.GetTransposeVariantsMatFlag(), transpose_variants_mat_flag);
    EXPECT_EQ(genotypeParameters.GetVariantsCodecID(), variants_codec_ID);

    EXPECT_EQ(genotypeParameters.GetEncodePhasesDataFlag(), encode_phases_data_flag);
    EXPECT_EQ(genotypeParameters.GetSortPhasesRowsFlag(), sort_phases_rows_flag);
    EXPECT_EQ(genotypeParameters.GetSortPhasesColsFlag(), sort_phases_cols_flag);
    EXPECT_EQ(genotypeParameters.GetTransposePhasesMatFlag(), transpose_phases_mat_flag);
    EXPECT_EQ(genotypeParameters.GetPhasesCodecID(), phases_codec_ID);
}

TEST_F(GenotypeParametersTests, GenotypeParameterWriteRandom) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    RandomAnnotationEncodingParameters randomgenotypeParameters;

    genie::genotype::GenotypeParameters genotypeParameters;
    genie::genotype::GenotypeParameters genotypeParametersCheck;
    genotypeParameters = randomgenotypeParameters.randomGenotypeParameters();

    std::stringstream InOut;
    genie::util::BitWriter strwriter(&InOut);
    genie::util::BitReader strreader(InOut);
    genotypeParameters.Write(strwriter);
    strwriter.FlushBits();
    genotypeParametersCheck.Read(strreader);

    auto size = genotypeParameters.GetSize();
    if (size % 8 != 0) size += (8 - size % 8);
    EXPECT_EQ(InOut.str().size(), size / 8);

    EXPECT_EQ(genotypeParameters.GetBinarizationID(), genotypeParametersCheck.GetBinarizationID());
    EXPECT_EQ(genotypeParameters.GetConcatAxis(), genotypeParametersCheck.GetConcatAxis());
    EXPECT_EQ(genotypeParameters.GetSortVariantsColsFlag(), genotypeParametersCheck.GetSortVariantsColsFlag());
    EXPECT_EQ(genotypeParameters.GetSortVariantsRowsFlag(), genotypeParametersCheck.GetSortVariantsRowsFlag());
    EXPECT_EQ(genotypeParameters.GetTransposeVariantsMatFlag(), genotypeParametersCheck.GetTransposeVariantsMatFlag());
    EXPECT_EQ(genotypeParameters.GetVariantsCodecID(), genotypeParametersCheck.GetVariantsCodecID());
    EXPECT_EQ(genotypeParameters.GetEncodePhasesDataFlag(), genotypeParametersCheck.GetEncodePhasesDataFlag());
    EXPECT_EQ(genotypeParameters.GetPhasesCodecID(), genotypeParametersCheck.GetPhasesCodecID());
    EXPECT_EQ(genotypeParameters.GetSortPhasesColsFlag(), genotypeParametersCheck.GetSortPhasesColsFlag());
    EXPECT_EQ(genotypeParameters.GetSortPhasesRowsFlag(), genotypeParametersCheck.GetSortPhasesRowsFlag());
    EXPECT_EQ(genotypeParameters.GetTransposePhasesMatFlag(), genotypeParametersCheck.GetTransposePhasesMatFlag());

#if GENERATE_TEST_FILES
    std::string name = "TestFiles/GenotypeParameters_seed_";
    name += std::to_string(rand() % 10);

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::util::BitWriter writer(&outputfile);
        genotypeParameters.Write(writer);
        writer.FlushBits();
        outputfile.Close();
#endif
    }
}

TEST_F(GenotypeParametersTests, AttributeParameterSetZeros) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    genie::core::record::annotation_parameter_set::AttributeParameterSet attributeParameterSet;

    EXPECT_EQ(attributeParameterSet.getCompressorID(), 0);
}
