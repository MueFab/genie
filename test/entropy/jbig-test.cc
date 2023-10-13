#include <gtest/gtest.h>
#include <random>
#include "genie/entropy/jbig/encoder.h"

class JBIGTestCase : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    JBIGTestCase() = default;

    ~JBIGTestCase() override = default;

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

TEST_F(JBIGTestCase, DISABLED_BSCEncodeDecodeTest) {  // NOLINT(cert-err58-cpp)
    const size_t NrOfInputBytes = 15;
    std::vector<uint8_t> testDataUncompressed(NrOfInputBytes);
    for (size_t i = 0; i < NrOfInputBytes; ++i) {
        uint8_t byte = static_cast<uint8_t>(rand() % 256);
        testDataUncompressed[i] = byte;
    }
    uint32_t ncols = 23;
    uint32_t nrows = 5;
    std::stringstream uncomressed_input;
    std::stringstream compressed_output;
    for (uint8_t byte : testDataUncompressed) uncomressed_input.write((char*)&byte, 1);

    genie::entropy::jbig::JBIGEncoder encoder;
    encoder.encode(uncomressed_input, compressed_output, ncols, nrows);

    std::stringstream uncompressed_output;
    encoder.decode(compressed_output, uncompressed_output, ncols, nrows);

    ASSERT_EQ(NrOfInputBytes, uncompressed_output.str().size());
    for (size_t i = 0; i < NrOfInputBytes; ++i)
        EXPECT_EQ(testDataUncompressed.at(i), static_cast<uint8_t>(uncompressed_output.str().at(i)));
}

TEST_F(JBIGTestCase, BSCparametersTest) {  // NOLINT(cert-err58-cpp)
     genie::entropy::jbig::JBIGparameters jbigParameters;

    auto parameters = jbigParameters.convertToAlgorithmParameters();
    EXPECT_EQ(parameters.getNumberOfPars(), 5);
}
