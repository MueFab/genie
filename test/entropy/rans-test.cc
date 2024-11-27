//
// Created by Irvan on 20.11.2024.
//

#include <gtest/gtest.h>
#include <random>
#include "genie/entropy/rans/encoder.h"
#include "genie/entropy/rans/decoder.h"
#include "genie/entropy/rans/commons.h"

class RANSTestCase : public ::testing::Test {
   protected:
    // Do any necessary setup for your tests here
      RANSTestCase() = default;

    ~RANSTestCase() override = default;

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


TEST_F(RANSTestCase, RANSEncodeDecodeTest) {  // NOLINT(cert-err58-cpp)
    const size_t NrOfInputBytes = 100;
    std::vector<uint8_t> testDataUncompressed(NrOfInputBytes);
    for (size_t i = 0; i < NrOfInputBytes; ++i) {
        uint8_t byte = static_cast<uint8_t>(rand() % 256);
        testDataUncompressed[i] = byte;
    }
    std::stringstream uncompressed_input;
    std::stringstream compressed_output;

	uncompressed_input.write(reinterpret_cast<const char*>(testDataUncompressed.data()), testDataUncompressed.size());

    genie::entropy::rans::RANSEncoder encoder;
	genie::entropy::rans::RANSDecoder decoder;
    encoder.encode(uncompressed_input, compressed_output);

    std::stringstream uncompressed_output;
    decoder.decode(compressed_output, uncompressed_output);
	std::string decoded_str = uncompressed_output.str();

    ASSERT_EQ(NrOfInputBytes, decoded_str.size()) << "Decoded data size does not match original.";
    for (size_t i = 0; i < NrOfInputBytes; ++i) {
        uint8_t original_byte = testDataUncompressed.at(i);
        uint8_t decoded_byte = static_cast<uint8_t>(decoded_str.at(i));
        EXPECT_EQ(original_byte, decoded_byte) << "Mismatch at byte index " << i;
    }
}