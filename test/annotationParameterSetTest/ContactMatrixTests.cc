
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>

#include "RandomRecordFillIn.h"
#include "genie/core/record/annotation_parameter_set/ContactMatrixParameters.h"
// ---------------------------------------------------------------------------------------------------------------------
#define GENERATE_TEST_FILES false

class ContactMatrixTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    ContactMatrixTests() = default;

    ~ContactMatrixTests() override = default;

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


TEST_F(ContactMatrixTests, ContactMatrixZeros) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    genie::core::record::annotation_parameter_set::ContactMatrixParameters cmParameters;
    EXPECT_EQ(cmParameters.getInterval(), uint8_t(0));
    EXPECT_EQ(cmParameters.getNumberOfSamples(), uint8_t(0));
    EXPECT_EQ(cmParameters.getNumberOfNormalizationMethods(), uint8_t(0));
}

TEST_F(ContactMatrixTests, ContactMatrixRandom) {  // NOLINT(cert-err58-cpp)
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
