/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>
#include <random>
#include "genie/entropy/bsc/encoder.h"
#include "genie/entropy/bsc/decoder.h"

class BSCTestCase : public ::testing::Test {
   protected:
      BSCTestCase() = default;
      ~BSCTestCase() override = default;

    void SetUp() override {
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }
};

TEST_F(BSCTestCase, BSCEncodeDecodeTest) {  // NOLINT(cert-err58-cpp)
    const size_t NrOfInputBytes = 100;
    std::vector<uint8_t> testDataUncompressed(NrOfInputBytes);
    for (size_t i = 0; i < NrOfInputBytes; ++i) {
        uint8_t byte = static_cast<uint8_t>(rand() % 256);
        testDataUncompressed[i] = byte;
    }
    std::stringstream uncompressed_input;
    std::stringstream compressed_output;
    for (uint8_t byte : testDataUncompressed) uncompressed_input.write((char*)&byte, 1);

    genie::entropy::bsc::BSCEncoder encoder;
    encoder.encode(uncompressed_input, compressed_output);

    std::stringstream uncompressed_output;
    genie::entropy::bsc::BSCDecoder decoder;
    decoder.decode(compressed_output, uncompressed_output);
    
    ASSERT_EQ(NrOfInputBytes, uncompressed_output.str().size());
    for (size_t i = 0; i < NrOfInputBytes; ++i)
        EXPECT_EQ(testDataUncompressed.at(i), static_cast<uint8_t>(uncompressed_output.str().at(i)));
}

TEST_F(BSCTestCase, BSCVectorEncodeDecodeTest) {  // NOLINT(cert-err58-cpp)
    const size_t NrOfInputBytes = 100;
    std::vector<uint8_t> testDataUncompressed(NrOfInputBytes);
    for (size_t i = 0; i < NrOfInputBytes; ++i) {
        uint8_t byte = static_cast<uint8_t>(rand() % 256);
        testDataUncompressed[i] = byte;
    }

    genie::entropy::bsc::BSCEncoder encoder;
    std::vector<uint8_t> compressed_output;
    encoder.encode(testDataUncompressed, compressed_output);

    genie::entropy::bsc::BSCDecoder decoder;
    std::vector<uint8_t> uncompressed_output;
    decoder.decode(compressed_output, uncompressed_output);

    ASSERT_EQ(testDataUncompressed.size(), uncompressed_output.size());
    for (size_t i = 0; i < NrOfInputBytes; ++i) {
        EXPECT_EQ(testDataUncompressed.at(i), uncompressed_output.at(i));
    }
}

TEST_F(BSCTestCase, BSCparametersTest) {  // NOLINT(cert-err58-cpp)
    genie::entropy::bsc::BSCParameters bscParameters;
    EXPECT_EQ(bscParameters.lzpHashSize, LIBBSC_DEFAULT_LZPHASHSIZE);
    EXPECT_EQ(bscParameters.lzpMinLen, LIBBSC_DEFAULT_LZPMINLEN);
    EXPECT_EQ(bscParameters.blockSorter, LIBBSC_DEFAULT_BLOCKSORTER);
    EXPECT_EQ(bscParameters.coder, LIBBSC_CODER_QLFC_STATIC);

    auto parameters = bscParameters.convertToAlgorithmParameters();
    EXPECT_EQ(parameters.getNumberOfPars(), 4);
}

TEST_F(BSCTestCase, BSCEmptyInputTest) { // NOLINT(cert-err58-cpp)
    // 1. Stringstream interface
    std::stringstream empty_input;
    std::stringstream compressed_output;
    genie::entropy::bsc::BSCEncoder encoder;
    encoder.encode(empty_input, compressed_output);
    EXPECT_EQ(compressed_output.str().size(), 0);

    std::stringstream decompressed_output;
    genie::entropy::bsc::BSCDecoder decoder;
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

TEST_F(BSCTestCase, BSCCorruptedInputTest) { // NOLINT(cert-err58-cpp)
    // Create random corrupted bytes
    std::vector<uint8_t> garbage(50);
    std::mt19937 rng(12345);
    for (size_t i = 0; i < garbage.size(); ++i) {
        garbage[i] = static_cast<uint8_t>(rng() % 256);
    }

    genie::entropy::bsc::BSCDecoder decoder;

    // Test vector interface with corrupted data
    std::vector<uint8_t> output_vec;
    decoder.decode(garbage, output_vec);
    EXPECT_TRUE(output_vec.empty());

    // Test stringstream interface with corrupted data
    std::stringstream garbage_stream;
    garbage_stream.write(reinterpret_cast<const char*>(garbage.data()), garbage.size());
    std::stringstream output_stream;
    decoder.decode(garbage_stream, output_stream);
    EXPECT_EQ(output_stream.str().size(), 0);

    // Test truncated compressed stream
    std::vector<uint8_t> valid_data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    genie::entropy::bsc::BSCEncoder encoder;
    std::vector<uint8_t> valid_compressed;
    encoder.encode(valid_data, valid_compressed);

    // Truncate it
    if (valid_compressed.size() > 5) {
        valid_compressed.resize(valid_compressed.size() - 5);
        std::vector<uint8_t> truncated_output;
        decoder.decode(valid_compressed, truncated_output);
        EXPECT_TRUE(truncated_output.empty() || truncated_output.size() != valid_data.size());
    }
}

TEST_F(BSCTestCase, BSCLargePayloadTest) { // NOLINT(cert-err58-cpp)
    const size_t LargeSize = 1024 * 1024; // 1 MB
    std::mt19937 rng(54321);

    // 1. Highly compressible data
    std::vector<uint8_t> compressible_data(LargeSize);
    for (size_t i = 0; i < LargeSize; ++i) {
        compressible_data[i] = static_cast<uint8_t>((i / 100) % 256);
    }

    genie::entropy::bsc::BSCEncoder encoder;
    genie::entropy::bsc::BSCDecoder decoder;

    std::vector<uint8_t> comp_compressible;
    encoder.encode(compressible_data, comp_compressible);
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
