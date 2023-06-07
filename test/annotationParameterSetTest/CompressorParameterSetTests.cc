
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include "genie/core/writer.h"

#include "RandomRecordFillIn.h"
#include "genie/core/record/annotation_parameter_set/CompressorParameterSet.h"
// ---------------------------------------------------------------------------------------------------------------------
#define GENERATE_TEST_FILES false

class CompressorParameterSetTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    CompressorParameterSetTests() = default;

    ~CompressorParameterSetTests() override = default;

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


TEST_F(CompressorParameterSetTests, CompressorParameterSetZeros) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    genie::core::record::annotation_parameter_set::CompressorParameterSet compressorParameterset;
    EXPECT_EQ(compressorParameterset.getCompressorID(), uint8_t(1));
    EXPECT_EQ(compressorParameterset.getCompressorStepIDs().size(), 0);
    EXPECT_EQ(compressorParameterset.getNumberOfCompletedOutVars().size(), 0);
}

TEST_F(CompressorParameterSetTests, CompressorParameterSetRandom) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    RandomAnnotationEncodingParameters RandomContactMatrixParameters;
    genie::core::record::annotation_parameter_set::CompressorParameterSet compressorParameterSet;
    genie::core::record::annotation_parameter_set::CompressorParameterSet compressorParameterSetCheck;
    genie::core::record::annotation_parameter_set::CompressorParameterSet compressorParameterSetCheck2;

    compressorParameterSet = RandomContactMatrixParameters.randomCompressorParameterSet();

    std::stringstream InOut;
    //(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
    genie::core::Writer strwriter(&InOut);
    genie::util::BitReader strreader(InOut);
    compressorParameterSet.write(strwriter);
    compressorParameterSet.write(strwriter);
    strwriter.flush();
    compressorParameterSetCheck.read(strreader);
    compressorParameterSetCheck2.read(strreader);

    EXPECT_EQ(compressorParameterSet.getCompressorID(), compressorParameterSetCheck.getCompressorID());
    EXPECT_EQ(compressorParameterSet.getCompressorID(), compressorParameterSetCheck2.getCompressorID());
    EXPECT_EQ(compressorParameterSet.getNumberOfCompressorSteps(),
              compressorParameterSetCheck.getNumberOfCompressorSteps());
    EXPECT_EQ(compressorParameterSet.getCompressorStepIDs(), compressorParameterSetCheck.getCompressorStepIDs());
    EXPECT_EQ(compressorParameterSet.getAlgorithmIDs(), compressorParameterSetCheck.getAlgorithmIDs());
    EXPECT_EQ(compressorParameterSet.IsDefaultParsUsed(), compressorParameterSetCheck.IsDefaultParsUsed());
    EXPECT_EQ(compressorParameterSet.getAlgorithmParameters().size(),
              compressorParameterSetCheck.getAlgorithmParameters().size());
    EXPECT_EQ(compressorParameterSet.getInVarsIDs(), compressorParameterSetCheck.getInVarsIDs());
    EXPECT_EQ(compressorParameterSet.getPreviousStepIDs(), compressorParameterSetCheck.getPreviousStepIDs());
    EXPECT_EQ(compressorParameterSet.getNumberInVars(), compressorParameterSetCheck.getNumberInVars());
    EXPECT_EQ(compressorParameterSet.getPreviousOutVarIDs(), compressorParameterSetCheck.getPreviousOutVarIDs());
    EXPECT_EQ(compressorParameterSet.getNumberOfCompletedOutVars(),
              compressorParameterSetCheck.getNumberOfCompletedOutVars());
    EXPECT_EQ(compressorParameterSet.getCompetedOutVarIDs(), compressorParameterSetCheck.getCompetedOutVarIDs());

#if GENERATE_TEST_FILES
    std::string name = "TestFiles/CompressorParameterSet_seed_";
    name += std::to_string(rand() % 10);

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::core::Writer writer(&outputfile);
        compressorParameterSet.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        genie::core::Writer txtWriter(&txtfile, true);
        compressorParameterSet.write(txtWriter);
        txtfile.close();
    }
#endif
}
