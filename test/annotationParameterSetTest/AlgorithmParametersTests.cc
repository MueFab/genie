
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
#include "genie/core/record/annotation_parameter_set/AlgorithmParameters.h"
#define GENERATE_TEST_FILES false

// ---------------------------------------------------------------------------------------------------------------------

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

TEST_F(AlgorithmParametersTests, AlgorithmParametersZero) {  // NOLINT(cert-err58-cpp)
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

TEST_F(AlgorithmParametersTests, AlgorithmParametersFixedValues) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    uint8_t n_pars = 4;
    std::vector<uint8_t> par_ID{1, 2, 3, 4};
    std::vector<genie::core::DataType> par_type{genie::core::DataType::UINT8, genie::core::DataType::UINT8,
                                                genie::core::DataType::UINT8, genie::core::DataType::UINT8};
    std::vector<uint8_t> par_num_array_dims(4, 0);
    std::vector<uint8_t> values{16, 128, 1, 1};
    std::vector<std::vector<uint8_t>> par_array_dims(n_pars, std::vector<uint8_t>(1, 0));
    std::vector<std::vector<std::vector<std::vector<std::vector<uint8_t>>>>> par_val;
    for (auto i = 0; i < n_pars; ++i) {
        par_val.emplace_back(genie::core::record::annotation_parameter_set::AlgorithmParameters::resizeVector(
            par_num_array_dims[i], par_array_dims[i]));

        genie::core::ArrayType arrayType;

        for (auto& j : par_val[i]) {
            for (auto& k : j)
                for (auto& l : k) l = arrayType.toArray(par_type[i], values[i]);
        }
    }
    genie::core::record::annotation_parameter_set::AlgorithmParameters algorithmParameters(
        n_pars, par_ID, par_type, par_num_array_dims, par_array_dims, par_val);

    std::stringstream InOut;
    genie::core::Writer strwriter(&InOut);
    algorithmParameters.write(strwriter);
    strwriter.flush();

    genie::core::Writer writesize;
    auto size = algorithmParameters.getSize(writesize);
    if ((size % 8) != 0) size += (8 - size % 8);

    EXPECT_EQ(InOut.str().size(), size / 8);
}
TEST_F(AlgorithmParametersTests, AlgorithmParametersRandom) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    RandomAnnotationEncodingParameters randomAlgorithmParameters;
    genie::core::record::annotation_parameter_set::AlgorithmParameters algorithmParameters;
    genie::core::record::annotation_parameter_set::AlgorithmParameters algorithmParametersCheck;
    uint8_t nPars = 3;
    uint8_t numArray = 0;
    std::vector<uint8_t> parNumArrayDims(nPars, numArray);
    algorithmParameters = randomAlgorithmParameters.randomAlgorithmParameters(nPars, parNumArrayDims);

    std::stringstream InOut;
    genie::core::Writer strwriter(&InOut);
    genie::util::BitReader strreader(InOut);
    algorithmParameters.write(strwriter);
    strwriter.flush();
    algorithmParametersCheck.read(strreader);
    std::stringstream TestOut;
    genie::core::Writer teststrwriter(&TestOut);
    algorithmParametersCheck.write(teststrwriter);
    teststrwriter.flush();

    EXPECT_EQ(InOut.str(), TestOut.str());

    genie::core::Writer writeSize;
    auto size = algorithmParameters.getSize(writeSize);
    if ((size % 8) != 0) size += (8 - size % 8);

    EXPECT_EQ(InOut.str().size(), size / 8);

    EXPECT_EQ(algorithmParameters.getParArrayDims(), algorithmParametersCheck.getParArrayDims());

    EXPECT_EQ(algorithmParameters.getNumberOfPars(), algorithmParametersCheck.getNumberOfPars());
    EXPECT_EQ(algorithmParameters.getParIDs(), algorithmParametersCheck.getParIDs());
    EXPECT_EQ(algorithmParameters.getParNumberOfArrayDims(), algorithmParametersCheck.getParNumberOfArrayDims());
    EXPECT_EQ(algorithmParameters.getParTypes(), algorithmParametersCheck.getParTypes());
    EXPECT_EQ(algorithmParameters.getParValues(), algorithmParametersCheck.getParValues());

#if GENERATE_TEST_FILES
    std::string name = "TestFiles/AlgorithmParameters_";
    name += std::to_string(numArray);
    name += "_seed_";
    name += std::to_string(rand() % 10);

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::core::Writer writer(&outputfile);
        algorithmParameters.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        genie::core::Writer txtwriter(&txtfile, true);
        algorithmParameters.write(txtwriter);
        txtfile.close();
    }

#endif
}
