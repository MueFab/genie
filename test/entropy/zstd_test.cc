/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>
#include <random>
#include "genie/entropy/zstd/encoder.h"
#include "genie/entropy/zstd/decoder.h"

class ZSTDTestCase : public ::testing::Test {
   protected:
    // Do any necessary setup for your tests here
      ZSTDTestCase() = default;

    ~ZSTDTestCase() override = default;

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

TEST_F(ZSTDTestCase, ZSTDEncodeDecodeTest) {  // NOLINT(cert-err58-cpp)
    const size_t NrOfInputBytes = 100;
    std::vector<uint8_t> testDataUncompressed(NrOfInputBytes);
    for (size_t i = 0; i < NrOfInputBytes; ++i) {
        uint8_t byte = static_cast<uint8_t>(rand() % 256);
        testDataUncompressed[i] = byte;
    }
    std::stringstream uncomressed_input;
    std::stringstream compressed_output;
    for (uint8_t byte : testDataUncompressed) uncomressed_input.write((char*)&byte, 1);

    genie::entropy::zstd::ZSTDEncoder encoder;
    encoder.encode(uncomressed_input, compressed_output);

    std::stringstream uncompressed_output;
    genie::entropy::zstd::ZSTDDecoder decoder;
    decoder.decode(compressed_output, uncompressed_output);
    
    ASSERT_EQ(NrOfInputBytes, uncompressed_output.str().size());
    for (size_t i = 0; i < NrOfInputBytes; ++i)
        EXPECT_EQ(testDataUncompressed.at(i), static_cast<uint8_t>(uncompressed_output.str().at(i)));
}

TEST_F(ZSTDTestCase, ZSTDVectorEncodeDecodeTest) {  // NOLINT(cert-err58-cpp)
    const size_t NrOfInputBytes = 100;
    std::vector<uint8_t> testDataUncompressed(NrOfInputBytes);
    for (size_t i = 0; i < NrOfInputBytes; ++i) {
        uint8_t byte = static_cast<uint8_t>(rand() % 256);
        testDataUncompressed[i] = byte;
    }

    genie::entropy::zstd::ZSTDEncoder encoder;
    std::vector<uint8_t> compressed_output;
    encoder.encode(testDataUncompressed, compressed_output);

    genie::entropy::zstd::ZSTDDecoder decoder;
    std::vector<uint8_t> uncompressed_output;
    decoder.decode(compressed_output, uncompressed_output);

    ASSERT_EQ(testDataUncompressed.size(), uncompressed_output.size());
    for (size_t i = 0; i < NrOfInputBytes; ++i) {
        EXPECT_EQ(testDataUncompressed.at(i), uncompressed_output.at(i));
    }
}

TEST_F(ZSTDTestCase, ZSTDparametersTest) {  // NOLINT(cert-err58-cpp)
    genie::entropy::zstd::ZSTDParameters zstdParameters;

    auto parameters = zstdParameters.convertToAlgorithmParameters();
    EXPECT_EQ(parameters.getNumberOfPars(), 3);

}

TEST_F(ZSTDTestCase, ZSTDEmptyInputTest) { // NOLINT(cert-err58-cpp)
    // 1. Stringstream interface
    std::stringstream empty_input;
    std::stringstream compressed_output;
    genie::entropy::zstd::ZSTDEncoder encoder;
    encoder.encode(empty_input, compressed_output);
    EXPECT_EQ(compressed_output.str().size(), 0);

    std::stringstream decompressed_output;
    genie::entropy::zstd::ZSTDDecoder decoder;
    decoder.decode(compressed_output, decompressed_output);
    EXPECT_EQ(decompressed_output.str().size(), 0);

    // 2. Vector interface
    std::vector<uint8_t> empty_vec;
    std::vector<uint8_t> compressed_vec;
    encoder.encode(empty_vec, compressed_vec);
    EXPECT_EQ(compressed_vec.size(), 0);

    std::vector<uint8_t> decompressed_vec;
    decoder.decode(compressed_vec, decompressed_vec);
    EXPECT_EQ(decompressed_vec.size(), 0);
}

TEST_F(ZSTDTestCase, ZSTDCorruptedInputTest) { // NOLINT(cert-err58-cpp)
    // Create random corrupted bytes
    std::vector<uint8_t> garbage(50);
    std::mt19937 rng(12345);
    for (size_t i = 0; i < garbage.size(); ++i) {
        garbage[i] = static_cast<uint8_t>(rng() % 256);
    }

    genie::entropy::zstd::ZSTDDecoder decoder;

    // Test vector interface with corrupted data
    std::vector<uint8_t> output_vec;
    // Should not crash and should fail cleanly
    decoder.decode(garbage, output_vec);
    // Since it failed, the output should remain empty
    EXPECT_TRUE(output_vec.empty());

    // Test stringstream interface with corrupted data
    std::stringstream garbage_stream;
    garbage_stream.write(reinterpret_cast<const char*>(garbage.data()), garbage.size());
    std::stringstream output_stream;
    decoder.decode(garbage_stream, output_stream);
    EXPECT_EQ(output_stream.str().size(), 0);

    // Test truncated compressed stream
    std::vector<uint8_t> valid_data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    genie::entropy::zstd::ZSTDEncoder encoder;
    std::vector<uint8_t> valid_compressed;
    encoder.encode(valid_data, valid_compressed);

    // Truncate it
    if (valid_compressed.size() > 5) {
        valid_compressed.resize(valid_compressed.size() - 5);
        std::vector<uint8_t> truncated_output;
        decoder.decode(valid_compressed, truncated_output);
        // Decompression fails on truncated frame, output should be empty or size mismatch
        EXPECT_TRUE(truncated_output.empty() || truncated_output.size() != valid_data.size());
    }
}

TEST_F(ZSTDTestCase, ZSTDLargePayloadTest) { // NOLINT(cert-err58-cpp)
    const size_t LargeSize = 1024 * 1024; // 1 MB
    std::mt19937 rng(54321);

    // 1. Highly compressible data (repeating pattern)
    std::vector<uint8_t> compressible_data(LargeSize);
    for (size_t i = 0; i < LargeSize; ++i) {
        compressible_data[i] = static_cast<uint8_t>((i / 100) % 256);
    }

    genie::entropy::zstd::ZSTDEncoder encoder;
    genie::entropy::zstd::ZSTDDecoder decoder;

    std::vector<uint8_t> comp_compressible;
    encoder.encode(compressible_data, comp_compressible);
    // Compressible data should shrink significantly
    EXPECT_LT(comp_compressible.size(), LargeSize / 10);

    std::vector<uint8_t> decomp_compressible;
    decoder.decode(comp_compressible, decomp_compressible);
    ASSERT_EQ(decomp_compressible.size(), LargeSize);
    EXPECT_EQ(compressible_data, decomp_compressible);

    // 2. High entropy random data
    std::vector<uint8_t> random_data(LargeSize);
    for (size_t i = 0; i < LargeSize; ++i) {
        random_data[i] = static_cast<uint8_t>(rng() % 256);
    }

    std::vector<uint8_t> comp_random;
    encoder.encode(random_data, comp_random);

    std::vector<uint8_t> decomp_random;
    decoder.decode(comp_random, decomp_random);
    ASSERT_EQ(decomp_random.size(), LargeSize);
    EXPECT_EQ(random_data, decomp_random);
}
