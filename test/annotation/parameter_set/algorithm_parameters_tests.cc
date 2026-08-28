
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>

#include "random_record_fill_in.h"
#include "genie/core/parameter/annotation/algorithm_parameters.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------
#define GENERATE_TEST_FILES false

class AlgorithmParametersTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    AlgorithmParametersTests() = default;

    ~AlgorithmParametersTests() override = default;

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

TEST_F(AlgorithmParametersTests, AlgorithmParametersZeros) {  // NOLINT(cert-err58-cpp)
    genie::core::parameter::annotation::AlgorithmParameters algorithmParameters;
    EXPECT_EQ(algorithmParameters.getNumberOfPars(), 0);
}

TEST_F(AlgorithmParametersTests, AlgorithmParametersValues) {  // NOLINT(cert-err58-cpp)
    uint8_t n_pars = 1;
    std::vector<uint8_t> par_ID{0};
    std::vector<genie::core::DataType> par_type{genie::core::DataType::DOUBLE};
    std::vector<uint8_t> par_num_array_dims{2};
    std::vector<std::vector<uint8_t>> par_array_dims{{1, 2}};
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val;
    for (int i = 0; i < n_pars; ++i) {
        par_val.emplace_back(genie::core::parameter::annotation::AlgorithmParameters::resizeVector(
            par_num_array_dims[i], par_array_dims[i]));
    }
    genie::core::parameter::annotation::AlgorithmParameters algorithmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);

    std::stringstream InOut;
    genie::util::BitWriter strwriter(InOut);
    algorithmParameters.write(strwriter);
    strwriter.FlushBits();

    std::stringstream nullStream;
    genie::util::BitWriter writesize(nullStream);
    auto size = algorithmParameters.getSize(writesize);
    if ((size % 8) != 0) size += (8 - size % 8);

    EXPECT_EQ(InOut.str().size(), size / 8);
}

TEST_F(AlgorithmParametersTests, AlgorithmParametersRandom) {  // NOLINT(cert-err58-cpp)
    RandomAnnotationEncodingParameters randomAlgorithmParameters;
    genie::core::parameter::annotation::AlgorithmParameters algorithmParameters;
    genie::core::parameter::annotation::AlgorithmParameters algorithmParametersCheck;
    uint8_t nPars = 3;
    uint8_t numArray = 0;
    std::vector<uint8_t> parNumArrayDims(nPars, numArray);
    algorithmParameters = randomAlgorithmParameters.randomAlgorithmParameters(nPars, parNumArrayDims);

    std::stringstream InOut;
    genie::util::BitWriter strwriter(InOut);
    genie::util::BitReader strreader(InOut);
    algorithmParameters.write(strwriter);
    strwriter.FlushBits();
    algorithmParametersCheck.read(strreader);
    std::stringstream TestOut;
    genie::util::BitWriter teststrwriter(TestOut);
    algorithmParametersCheck.write(teststrwriter);
    teststrwriter.FlushBits();

    EXPECT_EQ(InOut.str(), TestOut.str());

    std::stringstream nullStream;
    genie::util::BitWriter writeSize(nullStream);
    auto size = algorithmParameters.getSize(writeSize);
    if ((size % 8) != 0) size += (8 - size % 8);

    EXPECT_EQ(InOut.str().size(), size / 8);

    EXPECT_EQ(algorithmParameters.getParArrayDims(), algorithmParametersCheck.getParArrayDims());

    EXPECT_EQ(algorithmParameters.getNumberOfPars(), algorithmParametersCheck.getNumberOfPars());
    EXPECT_EQ(algorithmParameters.getParIDs(), algorithmParametersCheck.getParIDs());
    EXPECT_EQ(algorithmParameters.getParNumberOfArrayDims(), algorithmParametersCheck.getParNumberOfArrayDims());
    EXPECT_EQ(algorithmParameters.getParTypes(), algorithmParametersCheck.getParTypes());
    EXPECT_EQ(algorithmParameters.getParValues(), algorithmParametersCheck.getParValues());
}
