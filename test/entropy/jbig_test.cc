/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>
#include <random>
#include "genie/entropy/jbig/encoder.h"
#include "genie/entropy/jbig/decoder.h"

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

TEST_F(JBIGTestCase, JBIGEncodeDecodeTest) {  // NOLINT(cert-err58-cpp)
    const size_t NrOfInputBytes = 15;
    std::vector<uint8_t> testDataUncompressed(NrOfInputBytes);
    for (size_t i = 0; i < NrOfInputBytes; ++i) {
        uint8_t byte = static_cast<uint8_t>(rand() % 256);
        testDataUncompressed[i] = byte;
    }
    testDataUncompressed = { 0x7c, 0xe2, 0x38, 0x04, 0x92, 0x40, 0x04, 0xe2,
                                0x5c, 0x44, 0x92, 0x44, 0x38, 0xe2, 0x38 };
    uint32_t ncols = 23;
    uint32_t nrows = 5;
    std::stringstream uncomressed_input;
    std::stringstream compressed_output;
    for (uint8_t byte : testDataUncompressed) uncomressed_input << byte;

    ASSERT_EQ(NrOfInputBytes, uncomressed_input.str().size());
    for (uint8_t i = 0; i < NrOfInputBytes; ++i)
        EXPECT_EQ((uint8_t)uncomressed_input.str().at(i), testDataUncompressed.at(i));

    genie::entropy::jbig::JBIGEncoder encoder;
    encoder.encode(uncomressed_input, compressed_output, ncols, nrows);

    size_t ORIG_COMPRESSED_PAYLOAD_LEN = 37;
    // uint8_t ORIG_COMPRESSED_PAYLOAD[37] = { 0, 0,  1,   0,   0,   0,   0,  23,  0,   0,   0,  5,   255, 255, 255, 255, 127, 0, 0,
    //                              0, 25, 211, 149, 216, 214, 10, 197, 251, 121, 11, 254, 217, 140, 25,  128, 255, 2 };

    ASSERT_EQ(ORIG_COMPRESSED_PAYLOAD_LEN, compressed_output.str().size());

    std::stringstream uncompressed_output;
    genie::entropy::jbig::JBIGDecoder decoder;
    decoder.decode(compressed_output, uncompressed_output, ncols, nrows);

    ASSERT_EQ(NrOfInputBytes, uncompressed_output.str().size());
    for (size_t i = 0; i < NrOfInputBytes; ++i)
        EXPECT_EQ(testDataUncompressed.at(i), static_cast<uint8_t>(uncompressed_output.str().at(i)));
}

TEST_F(JBIGTestCase, JBIGparametersTest) {  // NOLINT(cert-err58-cpp)
     genie::entropy::jbig::JBIGparameters jbigParameters;

    auto parameters = jbigParameters.convertToAlgorithmParameters();
    EXPECT_EQ(parameters.getNumberOfPars(), 5);
}

TEST_F(JBIGTestCase, JBIGEncodeDecodeOptionsTest) {  // NOLINT(cert-err58-cpp)
    const size_t NrOfInputBytes = 15;
    std::vector<uint8_t> testDataUncompressed = { 0x7c, 0xe2, 0x38, 0x04, 0x92, 0x40, 0x04, 0xe2,
                                                  0x5c, 0x44, 0x92, 0x44, 0x38, 0xe2, 0x38 };
    uint32_t ncols = 23;
    uint32_t nrows = 5;

    // Test with non-default options
    genie::entropy::jbig::JBIGparameters params(2, false, true, false, true);
    genie::entropy::jbig::JBIGEncoder encoder;
    encoder.configure(params);

    std::stringstream uncompressed_input;
    for (uint8_t byte : testDataUncompressed) uncompressed_input << byte;
    std::stringstream compressed_output;

    encoder.encode(uncompressed_input, compressed_output, ncols, nrows);

    std::stringstream uncompressed_output;
    uint32_t out_ncols = 0;
    uint32_t out_rows = 0;
    genie::entropy::jbig::JBIGDecoder decoder;
    decoder.decode(compressed_output, uncompressed_output, out_ncols, out_rows);

    ASSERT_EQ(NrOfInputBytes, uncompressed_output.str().size());
    EXPECT_EQ(ncols, out_ncols);
    EXPECT_EQ(nrows, out_rows);
    for (size_t i = 0; i < NrOfInputBytes; ++i) {
        EXPECT_EQ(testDataUncompressed.at(i), static_cast<uint8_t>(uncompressed_output.str().at(i)));
    }
}

TEST_F(JBIGTestCase, JBIGVectorEncodeDecodeTest) {  // NOLINT(cert-err58-cpp)
    const size_t NrOfInputBytes = 15;
    std::vector<uint8_t> testDataUncompressed = { 0x7c, 0xe2, 0x38, 0x04, 0x92, 0x40, 0x04, 0xe2,
                                                  0x5c, 0x44, 0x92, 0x44, 0x38, 0xe2, 0x38 };
    uint32_t ncols = 8;
    uint32_t nrows = 15;

    genie::entropy::jbig::JBIGEncoder encoder;
    std::vector<uint8_t> compressed_output;
    encoder.encode(testDataUncompressed, compressed_output, ncols, nrows);

    genie::entropy::jbig::JBIGDecoder decoder;
    std::vector<uint8_t> decompressed_output;
    uint32_t out_ncols = 0;
    uint32_t out_nrows = 0;
    decoder.decode(compressed_output, decompressed_output, out_ncols, out_nrows);

    ASSERT_EQ(NrOfInputBytes, decompressed_output.size());
    EXPECT_EQ(out_ncols, ncols);
    EXPECT_EQ(out_nrows, nrows);
    for (size_t i = 0; i < NrOfInputBytes; ++i) {
        EXPECT_EQ(testDataUncompressed.at(i), decompressed_output.at(i));
    }
}

TEST_F(JBIGTestCase, JBIGRoundTripRandomDimensionsTest) {  // NOLINT(cert-err58-cpp)
    std::mt19937 rng(42); // deterministic seed
    
    // Test various dimensions
    std::vector<std::pair<uint32_t, uint32_t>> dimensions = {
        {8, 1},
        {8, 8},
        {16, 16},
        {64, 50},
        {128, 100},
        {1024, 16}
    };
    
    for (const auto& dim : dimensions) {
        uint32_t ncols = dim.first;
        uint32_t nrows = dim.second;
        
        size_t bytes_per_line = (ncols + 7) / 8;
        size_t total_bytes = bytes_per_line * nrows;
        
        std::vector<uint8_t> original_data(total_bytes);
        for (size_t i = 0; i < total_bytes; ++i) {
            original_data[i] = static_cast<uint8_t>(rng() % 256);
        }
        
        genie::entropy::jbig::JBIGparameters params(static_cast<int32_t>(rng() % 5 + 1), rng() % 2 == 0, rng() % 2 == 0, false, rng() % 2 == 0);
        genie::entropy::jbig::JBIGEncoder encoder;
        encoder.configure(params);
        
        // Test stringstream interface
        std::stringstream uncompressed_input;
        for (uint8_t byte : original_data) uncompressed_input << byte;
        std::stringstream compressed_output;
        
        encoder.encode(uncompressed_input, compressed_output, ncols, nrows);
        
        std::stringstream uncompressed_output;
        uint32_t out_ncols = 0;
        uint32_t out_nrows = 0;
        genie::entropy::jbig::JBIGDecoder decoder;
        decoder.decode(compressed_output, uncompressed_output, out_ncols, out_nrows);
        
        ASSERT_EQ(total_bytes, uncompressed_output.str().size());
        EXPECT_EQ(ncols, out_ncols);
        EXPECT_EQ(nrows, out_nrows);
        for (size_t i = 0; i < total_bytes; ++i) {
            EXPECT_EQ(original_data.at(i), static_cast<uint8_t>(uncompressed_output.str().at(i)));
        }
        
        // Test vector interface
        std::vector<uint8_t> comp_vec;
        encoder.encode(original_data, comp_vec, ncols, nrows);
        
        std::vector<uint8_t> decomp_vec;
        uint32_t out_ncols2 = 0;
        uint32_t out_nrows2 = 0;
        decoder.decode(comp_vec, decomp_vec, out_ncols2, out_nrows2);
        
        ASSERT_EQ(total_bytes, decomp_vec.size());
        EXPECT_EQ(ncols, out_ncols2);
        EXPECT_EQ(nrows, out_nrows2);
        for (size_t i = 0; i < total_bytes; ++i) {
            EXPECT_EQ(original_data.at(i), decomp_vec.at(i));
        }
    }
}

TEST_F(JBIGTestCase, JBIGEmptyInputTest) { // NOLINT(cert-err58-cpp)
    genie::entropy::jbig::JBIGEncoder encoder;
    genie::entropy::jbig::JBIGDecoder decoder;

    // 1. Zero dimensions
    std::vector<uint8_t> testData = {0x00, 0xff};
    std::vector<uint8_t> compressed;
    encoder.encode(testData, compressed, 0, 10);
    EXPECT_TRUE(compressed.empty());

    encoder.encode(testData, compressed, 10, 0);
    EXPECT_TRUE(compressed.empty());

    // 2. Empty stringstream
    std::stringstream empty_in;
    std::stringstream empty_out;
    encoder.encode(empty_in, empty_out, 8, 8);
    EXPECT_EQ(empty_out.str().size(), 0);

    std::stringstream decomp_out;
    uint32_t out_ncols = 0, out_nrows = 0;
    decoder.decode(empty_out, decomp_out, out_ncols, out_nrows);
    EXPECT_EQ(decomp_out.str().size(), 0);

    // 3. Empty vector
    std::vector<uint8_t> empty_vec;
    std::vector<uint8_t> comp_vec;
    encoder.encode(empty_vec, comp_vec, 8, 8);
    EXPECT_TRUE(comp_vec.empty());

    std::vector<uint8_t> decomp_vec;
    decoder.decode(comp_vec, decomp_vec, out_ncols, out_nrows);
    EXPECT_TRUE(decomp_vec.empty());
}

TEST_F(JBIGTestCase, JBIGDimensionMismatchTest) { // NOLINT(cert-err58-cpp)
    genie::entropy::jbig::JBIGEncoder encoder;

    // Input buffer only has 2 bytes, but we request 100 x 100 dimensions
    // Expected bytes per line: (100+7)/8 = 13 bytes. Total expected: 13 * 100 = 1300 bytes.
    std::vector<uint8_t> tiny_input = {0xaa, 0xbb};
    std::vector<uint8_t> compressed;
    
    // This should fail early because input is smaller than expected size
    encoder.encode(tiny_input, compressed, 100, 100);
    EXPECT_TRUE(compressed.empty());

    std::stringstream tiny_in_stream;
    tiny_in_stream.write(reinterpret_cast<const char*>(tiny_input.data()), tiny_input.size());
    std::stringstream compressed_stream;
    encoder.encode(tiny_in_stream, compressed_stream, 100, 100);
    EXPECT_EQ(compressed_stream.str().size(), 0);
}

TEST_F(JBIGTestCase, JBIGCorruptedInputTest) { // NOLINT(cert-err58-cpp)
    std::vector<uint8_t> garbage(100);
    std::mt19937 rng(54321);
    for (size_t i = 0; i < garbage.size(); ++i) {
        garbage[i] = static_cast<uint8_t>(rng() % 256);
    }

    genie::entropy::jbig::JBIGDecoder decoder;
    std::vector<uint8_t> decompressed;
    uint32_t out_ncols = 0, out_nrows = 0;

    // Should fail cleanly and not crash/exit the process
    decoder.decode(garbage, decompressed, out_ncols, out_nrows);
    EXPECT_TRUE(decompressed.empty());

    std::stringstream garbage_stream;
    garbage_stream.write(reinterpret_cast<const char*>(garbage.data()), garbage.size());
    std::stringstream decompressed_stream;
    decoder.decode(garbage_stream, decompressed_stream, out_ncols, out_nrows);
    EXPECT_EQ(decompressed_stream.str().size(), 0);

    // Test truncated compressed stream
    std::vector<uint8_t> valid_input = { 0x7c, 0xe2, 0x38, 0x04, 0x92, 0x40, 0x04, 0xe2,
                                         0x5c, 0x44, 0x92, 0x44, 0x38, 0xe2, 0x38 };
    genie::entropy::jbig::JBIGEncoder encoder;
    std::vector<uint8_t> valid_compressed;
    encoder.encode(valid_input, valid_compressed, 23, 5);

    if (valid_compressed.size() > 10) {
        valid_compressed.resize(valid_compressed.size() - 10); // truncate significantly
        std::vector<uint8_t> truncated_output;
        decoder.decode(valid_compressed, truncated_output, out_ncols, out_nrows);
        EXPECT_TRUE(truncated_output.empty() || truncated_output.size() != valid_input.size());
    }
}

TEST_F(JBIGTestCase, JBIGLargePayloadTest) { // NOLINT(cert-err58-cpp)
    const uint32_t ncols = 2048;
    const uint32_t nrows = 1024;
    const size_t bytes_per_line = (ncols + 7) / 8;
    const size_t total_bytes = bytes_per_line * nrows;

    std::vector<uint8_t> original_data(total_bytes);
    std::mt19937 rng(98765);
    for (size_t i = 0; i < total_bytes; ++i) {
        original_data[i] = static_cast<uint8_t>(rng() % 256);
    }

    genie::entropy::jbig::JBIGEncoder encoder;
    std::vector<uint8_t> compressed;
    encoder.encode(original_data, compressed, ncols, nrows);
    EXPECT_FALSE(compressed.empty());

    genie::entropy::jbig::JBIGDecoder decoder;
    std::vector<uint8_t> decompressed;
    uint32_t out_ncols = 0, out_nrows = 0;
    decoder.decode(compressed, decompressed, out_ncols, out_nrows);

    ASSERT_EQ(original_data.size(), decompressed.size());
    EXPECT_EQ(ncols, out_ncols);
    EXPECT_EQ(nrows, out_nrows);
    EXPECT_EQ(original_data, decompressed);
}


