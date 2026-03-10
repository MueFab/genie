/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <gtest/gtest.h>
#include <cstring>
#include <sstream>
#include <vector>
#include "genie/entropy/ser/encoder.h"
//#include "genie/entropy/ser/decoder.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/core/record/annotation_access_unit/TypedData.h"

class SERTestCase : public ::testing::Test {
protected:
    SERTestCase() = default;
    ~SERTestCase() override = default;

    void SetUp() override {
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }

    // Helper function to read uint32_t dimensions from network byte order
    std::vector<uint32_t> readDimensions(const std::string& dimensionsStr) {
        std::vector<uint32_t> dimensions;
        std::istringstream iss(dimensionsStr);
        genie::util::BitReader reader(iss);
        size_t numDimensions = dimensionsStr.size() / sizeof(uint32_t);
        
        for (size_t i = 0; i < numDimensions; ++i) {
            uint32_t value = reader.Read<uint32_t>();
            dimensions.push_back(value);
        }
        
        return dimensions;
    }
};

// Test SER encoding with 1D array (order = false)
TEST_F(SERTestCase, SEREncode1DArrayInOrderTest) {  // NOLINT(cert-err58-cpp)
    // Create input data: [10, 20, 30, 40]
    // Expected output:
    //   Symbols: [10, 20, 30, 40] (flattened, same order)
    //   Dimensions: [4]
    
    std::vector<uint8_t> inputValues = {10, 20, 30, 40};
    
    // Create TypedData for 1D input
    genie::core::record::annotation_access_unit::TypedData inputData(
        genie::core::DataType::UINT8, 
        1, 
        std::vector<uint32_t>{static_cast<uint32_t>(inputValues.size())}
    );
    
    // Write input data to stream
    auto& inputStream = inputData.getDataStream();
    inputStream.write(reinterpret_cast<const char*>(inputValues.data()), 
                      inputValues.size() * sizeof(uint8_t));
    
    // Create encoder and configure
    genie::entropy::ser::SEREncoder encoder;
    genie::entropy::ser::SERParameters params(false);  // order = false
    encoder.configure(params);
    encoder.setInput(0, inputData);
    
    // Perform encoding
    encoder.encode();
    
    // Get outputs
    auto& symbolOutput = encoder.getOutput(0);
    auto& dimensionsOutput = encoder.getOutput(1);
    
    // Verify output types and dimensions
    ASSERT_EQ(symbolOutput.getDataTypeID(), genie::core::DataType::UINT8);
    ASSERT_EQ(dimensionsOutput.getDataTypeID(), genie::core::DataType::UINT32);
    
    // Verify symbols
    std::string symbolStr = symbolOutput.getDataStream().str();
    const uint8_t* symbols = reinterpret_cast<const uint8_t*>(symbolStr.data());
    size_t numSymbols = symbolStr.size() / sizeof(uint8_t);
    
    ASSERT_EQ(numSymbols, 4);
    EXPECT_EQ(symbols[0], 10);
    EXPECT_EQ(symbols[1], 20);
    EXPECT_EQ(symbols[2], 30);
    EXPECT_EQ(symbols[3], 40);
    
    // Verify dimensions (read in network byte order)
    std::string dimensionsStr = dimensionsOutput.getDataStream().str();
    std::vector<uint32_t> dimensions = readDimensions(dimensionsStr);
    
    ASSERT_EQ(dimensions.size(), 1);
    EXPECT_EQ(dimensions[0], 4);
}

// Test SER encoding with 2D array (order = false)
TEST_F(SERTestCase, SEREncode2DArrayInOrderTest) {  // NOLINT(cert-err58-cpp)
    // Create input data: 2x3 matrix
    // [[1, 2, 3],
    //  [4, 5, 6]]
    // Expected output with order=false:
    //   Symbols: [1, 2, 3, 4, 5, 6] (row-major order)
    //   Dimensions: [2, 3]
    
    std::vector<uint8_t> inputValues = {1, 2, 3, 4, 5, 6};
    
    // Create TypedData for 2D input (2 rows, 3 columns)
    genie::core::record::annotation_access_unit::TypedData inputData(
        genie::core::DataType::UINT8, 
        2, 
        std::vector<uint32_t>{2, 3}
    );
    
    // Write input data to stream (row-major order)
    auto& inputStream = inputData.getDataStream();
    inputStream.write(reinterpret_cast<const char*>(inputValues.data()), 
                      inputValues.size() * sizeof(uint8_t));
    
    // Create encoder and configure
    genie::entropy::ser::SEREncoder encoder;
    genie::entropy::ser::SERParameters params(false);  // order = false
    encoder.configure(params);
    encoder.setInput(0, inputData);
    
    // Perform encoding
    encoder.encode();
    
    // Get outputs
    auto& symbolOutput = encoder.getOutput(0);
    auto& dimensionsOutput = encoder.getOutput(1);
    
    // Verify symbols (should be flattened in row-major order)
    std::string symbolStr = symbolOutput.getDataStream().str();
    const uint8_t* symbols = reinterpret_cast<const uint8_t*>(symbolStr.data());
    size_t numSymbols = symbolStr.size() / sizeof(uint8_t);
    
    ASSERT_EQ(numSymbols, 6);
    EXPECT_EQ(symbols[0], 1);
    EXPECT_EQ(symbols[1], 2);
    EXPECT_EQ(symbols[2], 3);
    EXPECT_EQ(symbols[3], 4);
    EXPECT_EQ(symbols[4], 5);
    EXPECT_EQ(symbols[5], 6);
    
    // Verify dimensions (read in network byte order)
    std::string dimensionsStr = dimensionsOutput.getDataStream().str();
    std::vector<uint32_t> dimensions = readDimensions(dimensionsStr);
    
    ASSERT_EQ(dimensions.size(), 2);
    EXPECT_EQ(dimensions[0], 2);
    EXPECT_EQ(dimensions[1], 3);
}

// Test SER encoding with 2D array (order = true, reverse order)
TEST_F(SERTestCase, SEREncode2DArrayReverseOrderTest) {  // NOLINT(cert-err58-cpp)
    // Create input data: 2x3 matrix
    // [[1, 2, 3],
    //  [4, 5, 6]]
    // Expected output with order=true:
    //   Symbols: [1, 4, 2, 5, 3, 6] (column-major order)
    //   Dimensions: [2, 3]
    
    std::vector<uint8_t> inputValues = {1, 2, 3, 4, 5, 6};
    
    // Create TypedData for 2D input (2 rows, 3 columns)
    genie::core::record::annotation_access_unit::TypedData inputData(
        genie::core::DataType::UINT8, 
        2, 
        std::vector<uint32_t>{2, 3}
    );
    
    // Write input data to stream (row-major order)
    auto& inputStream = inputData.getDataStream();
    inputStream.write(reinterpret_cast<const char*>(inputValues.data()), 
                      inputValues.size() * sizeof(uint8_t));
    
    // Create encoder and configure with reverse order
    genie::entropy::ser::SEREncoder encoder;
    genie::entropy::ser::SERParameters params(true);  // order = true (reverse)
    encoder.configure(params);
    encoder.setInput(0, inputData);
    
    // Perform encoding
    encoder.encode();
    
    // Get outputs
    auto& symbolOutput = encoder.getOutput(0);
    auto& dimensionsOutput = encoder.getOutput(1);
    
    // Verify symbols (should be in column-major order)
    std::string symbolStr = symbolOutput.getDataStream().str();
    const uint8_t* symbols = reinterpret_cast<const uint8_t*>(symbolStr.data());
    size_t numSymbols = symbolStr.size() / sizeof(uint8_t);
    
    ASSERT_EQ(numSymbols, 6);
    EXPECT_EQ(symbols[0], 1);
    EXPECT_EQ(symbols[1], 4);
    EXPECT_EQ(symbols[2], 2);
    EXPECT_EQ(symbols[3], 5);
    EXPECT_EQ(symbols[4], 3);
    EXPECT_EQ(symbols[5], 6);
    
    // Verify dimensions (read in network byte order)
    std::string dimensionsStr = dimensionsOutput.getDataStream().str();
    std::vector<uint32_t> dimensions = readDimensions(dimensionsStr);
    
    ASSERT_EQ(dimensions.size(), 2);
    EXPECT_EQ(dimensions[0], 2);
    EXPECT_EQ(dimensions[1], 3);
}

// Test SER encoding with 3D array (order = false)
TEST_F(SERTestCase, SEREncode3DArrayInOrderTest) {  // NOLINT(cert-err58-cpp)
    // Create input data: 2x2x2 cube
    // [[[1, 2], [3, 4]], [[5, 6], [7, 8]]]
    // Expected output with order=false:
    //   Symbols: [1, 2, 3, 4, 5, 6, 7, 8] (row-major order)
    //   Dimensions: [2, 2, 2]
    
    std::vector<uint8_t> inputValues = {1, 2, 3, 4, 5, 6, 7, 8};
    
    // Create TypedData for 3D input
    genie::core::record::annotation_access_unit::TypedData inputData(
        genie::core::DataType::UINT8, 
        3, 
        std::vector<uint32_t>{2, 2, 2}
    );
    
    // Write input data to stream
    auto& inputStream = inputData.getDataStream();
    inputStream.write(reinterpret_cast<const char*>(inputValues.data()), 
                      inputValues.size() * sizeof(uint8_t));
    
    // Create encoder and configure
    genie::entropy::ser::SEREncoder encoder;
    genie::entropy::ser::SERParameters params(false);  // order = false
    encoder.configure(params);
    encoder.setInput(0, inputData);
    
    // Perform encoding
    encoder.encode();
    
    // Get outputs
    auto& symbolOutput = encoder.getOutput(0);
    auto& dimensionsOutput = encoder.getOutput(1);
    
    // Verify symbols
    std::string symbolStr = symbolOutput.getDataStream().str();
    const uint8_t* symbols = reinterpret_cast<const uint8_t*>(symbolStr.data());
    size_t numSymbols = symbolStr.size() / sizeof(uint8_t);
    
    ASSERT_EQ(numSymbols, 8);
    for (size_t i = 0; i < numSymbols; ++i) {
        EXPECT_EQ(symbols[i], inputValues[i]);
    }
    
    // Verify dimensions (read in network byte order)
    std::string dimensionsStr = dimensionsOutput.getDataStream().str();
    std::vector<uint32_t> dimensions = readDimensions(dimensionsStr);
    
    ASSERT_EQ(dimensions.size(), 3);
    EXPECT_EQ(dimensions[0], 2);
    EXPECT_EQ(dimensions[1], 2);
    EXPECT_EQ(dimensions[2], 2);
}

// Test SER encoding with UINT16 data type
TEST_F(SERTestCase, SEREncodeUINT16Test) {  // NOLINT(cert-err58-cpp)
    // Create input data with uint16_t
    std::vector<uint16_t> inputValues = {100, 200, 300, 400};
    
    // Create TypedData for 1D input
    genie::core::record::annotation_access_unit::TypedData inputData(
        genie::core::DataType::UINT16, 
        1, 
        std::vector<uint32_t>{static_cast<uint32_t>(inputValues.size())}
    );
    
    // Write input data to stream
    auto& inputStream = inputData.getDataStream();
    inputStream.write(reinterpret_cast<const char*>(inputValues.data()), 
                      inputValues.size() * sizeof(uint16_t));
    
    // Create encoder and configure
    genie::entropy::ser::SEREncoder encoder;
    genie::entropy::ser::SERParameters params(false);
    encoder.configure(params);
    encoder.setInput(0, inputData);
    
    // Perform encoding
    encoder.encode();
    
    // Get outputs
    auto& symbolOutput = encoder.getOutput(0);
    auto& dimensionsOutput = encoder.getOutput(1);
    
    // Verify output type
    ASSERT_EQ(symbolOutput.getDataTypeID(), genie::core::DataType::UINT16);
    
    // Verify symbols
    std::string symbolStr = symbolOutput.getDataStream().str();
    const uint16_t* symbols = reinterpret_cast<const uint16_t*>(symbolStr.data());
    size_t numSymbols = symbolStr.size() / sizeof(uint16_t);
    
    ASSERT_EQ(numSymbols, 4);
    EXPECT_EQ(symbols[0], 100);
    EXPECT_EQ(symbols[1], 200);
    EXPECT_EQ(symbols[2], 300);
    EXPECT_EQ(symbols[3], 400);
    
    // Verify dimensions (read in network byte order)
    std::string dimensionsStr = dimensionsOutput.getDataStream().str();
    std::vector<uint32_t> dimensions = readDimensions(dimensionsStr);
    
    ASSERT_EQ(dimensions.size(), 1);
    EXPECT_EQ(dimensions[0], 4);
}

// Test SER encoding with STRING data type (1D array, order = false)
TEST_F(SERTestCase, SEREncodeString1DInOrderTest) {  // NOLINT(cert-err58-cpp)
    // Create input data: ["hello", "world", "test"]
    // Expected output with order=false:
    //   Symbols: "hello\0world\0test\0" (concatenated with null terminators)
    //   Dimensions: [3]
    
    std::vector<std::string> inputStrings = {"hello", "world", "test"};
    
    // Create TypedData for 1D string input
    genie::core::record::annotation_access_unit::TypedData inputData(
        genie::core::DataType::STRING, 
        1, 
        std::vector<uint32_t>{static_cast<uint32_t>(inputStrings.size())}
    );
    
    // Write input data to stream (each string null-terminated)
    auto& inputStream = inputData.getDataStream();
    for (const auto& str : inputStrings) {
        inputStream.write(str.c_str(), str.length() + 1);  // Include null terminator
    }
    
    // Create encoder and configure
    genie::entropy::ser::SEREncoder encoder;
    genie::entropy::ser::SERParameters params(false);  // order = false
    encoder.configure(params);
    encoder.setInput(0, inputData);
    
    // Perform encoding
    encoder.encode();
    
    // Get outputs
    auto& symbolOutput = encoder.getOutput(0);
    auto& dimensionsOutput = encoder.getOutput(1);
    
    // Verify output type
    ASSERT_EQ(symbolOutput.getDataTypeID(), genie::core::DataType::STRING);
    
    // Verify symbols (strings should be concatenated with null terminators)
    std::string symbolStr = symbolOutput.getDataStream().str();
    const char* symbols = symbolStr.c_str();
    
    // Parse strings from output
    std::vector<std::string> outputStrings;
    size_t offset = 0;
    while (offset < symbolStr.size()) {
        std::string str(symbols + offset);
        outputStrings.push_back(str);
        offset += str.length() + 1;  // Move past null terminator
    }
    
    ASSERT_EQ(outputStrings.size(), inputStrings.size());
    for (size_t i = 0; i < outputStrings.size(); ++i) {
        EXPECT_EQ(outputStrings[i], inputStrings[i]);
    }
    
    // Verify dimensions (read in network byte order)
    std::string dimensionsStr = dimensionsOutput.getDataStream().str();
    std::vector<uint32_t> dimensions = readDimensions(dimensionsStr);
    
    ASSERT_EQ(dimensions.size(), 1);
    EXPECT_EQ(dimensions[0], 3);
}

// Test SER encoding with STRING data type (2D array, order = false)
TEST_F(SERTestCase, SEREncodeString2DInOrderTest) {  // NOLINT(cert-err58-cpp)
    // Create input data: 2x2 matrix of strings
    // [["a", "b"],
    //  ["c", "d"]]
    // Expected output with order=false:
    //   Symbols: "a\0b\0c\0d\0" (row-major order)
    //   Dimensions: [2, 2]
    
    std::vector<std::string> inputStrings = {"a", "b", "c", "d"};
    
    // Create TypedData for 2D string input (2 rows, 2 columns)
    genie::core::record::annotation_access_unit::TypedData inputData(
        genie::core::DataType::STRING, 
        2, 
        std::vector<uint32_t>{2, 2}
    );
    
    // Write input data to stream (row-major order)
    auto& inputStream = inputData.getDataStream();
    for (const auto& str : inputStrings) {
        inputStream.write(str.c_str(), str.length() + 1);
    }
    
    // Create encoder and configure
    genie::entropy::ser::SEREncoder encoder;
    genie::entropy::ser::SERParameters params(false);  // order = false
    encoder.configure(params);
    encoder.setInput(0, inputData);
    
    // Perform encoding
    encoder.encode();
    
    // Get outputs
    auto& symbolOutput = encoder.getOutput(0);
    auto& dimensionsOutput = encoder.getOutput(1);
    
    // Verify symbols
    std::string symbolStr = symbolOutput.getDataStream().str();
    const char* symbols = symbolStr.c_str();
    
    // Parse strings from output
    std::vector<std::string> outputStrings;
    size_t offset = 0;
    while (offset < symbolStr.size()) {
        std::string str(symbols + offset);
        outputStrings.push_back(str);
        offset += str.length() + 1;
    }
    
    ASSERT_EQ(outputStrings.size(), 4);
    EXPECT_EQ(outputStrings[0], "a");
    EXPECT_EQ(outputStrings[1], "b");
    EXPECT_EQ(outputStrings[2], "c");
    EXPECT_EQ(outputStrings[3], "d");
    
    // Verify dimensions
    std::string dimensionsStr = dimensionsOutput.getDataStream().str();
    std::vector<uint32_t> dimensions = readDimensions(dimensionsStr);
    
    ASSERT_EQ(dimensions.size(), 2);
    EXPECT_EQ(dimensions[0], 2);
    EXPECT_EQ(dimensions[1], 2);
}

// Test SER encoding with STRING data type (2D array, order = true, reverse order)
TEST_F(SERTestCase, SEREncodeString2DReverseOrderTest) {  // NOLINT(cert-err58-cpp)
    // Create input data: 2x3 matrix of strings
    // [["a", "b", "c"],
    //  ["d", "e", "f"]]
    // Expected output with order=true:
    //   Symbols: "a\0d\0b\0e\0c\0f\0" (column-major order)
    //   Dimensions: [2, 3]
    
    std::vector<std::string> inputStrings = {"a", "b", "c", "d", "e", "f"};
    
    // Create TypedData for 2D string input (2 rows, 3 columns)
    genie::core::record::annotation_access_unit::TypedData inputData(
        genie::core::DataType::STRING, 
        2, 
        std::vector<uint32_t>{2, 3}
    );
    
    // Write input data to stream (row-major order)
    auto& inputStream = inputData.getDataStream();
    for (const auto& str : inputStrings) {
        inputStream.write(str.c_str(), str.length() + 1);
    }
    
    // Create encoder and configure with reverse order
    genie::entropy::ser::SEREncoder encoder;
    genie::entropy::ser::SERParameters params(true);  // order = true (reverse)
    encoder.configure(params);
    encoder.setInput(0, inputData);
    
    // Perform encoding
    encoder.encode();
    
    // Get outputs
    auto& symbolOutput = encoder.getOutput(0);
    auto& dimensionsOutput = encoder.getOutput(1);
    
    // Verify symbols (should be in column-major order)
    std::string symbolStr = symbolOutput.getDataStream().str();
    const char* symbols = symbolStr.c_str();
    
    // Parse strings from output
    std::vector<std::string> outputStrings;
    size_t offset = 0;
    while (offset < symbolStr.size()) {
        std::string str(symbols + offset);
        outputStrings.push_back(str);
        offset += str.length() + 1;
    }
    
    ASSERT_EQ(outputStrings.size(), 6);
    EXPECT_EQ(outputStrings[0], "a");
    EXPECT_EQ(outputStrings[1], "d");
    EXPECT_EQ(outputStrings[2], "b");
    EXPECT_EQ(outputStrings[3], "e");
    EXPECT_EQ(outputStrings[4], "c");
    EXPECT_EQ(outputStrings[5], "f");
    
    // Verify dimensions
    std::string dimensionsStr = dimensionsOutput.getDataStream().str();
    std::vector<uint32_t> dimensions = readDimensions(dimensionsStr);
    
    ASSERT_EQ(dimensions.size(), 2);
    EXPECT_EQ(dimensions[0], 2);
    EXPECT_EQ(dimensions[1], 3);
}

// Test SER encoding with STRING data type (3D array, order = false)
TEST_F(SERTestCase, SEREncodeString3DInOrderTest) {  // NOLINT(cert-err58-cpp)
    // Create input data: 2x2x2 cube of strings
    // [[["aa", "bb"], ["cc", "dd"]], [["ee", "ff"], ["gg", "hh"]]]
    // Expected output with order=false:
    //   Symbols: "aa\0bb\0cc\0dd\0ee\0ff\0gg\0hh\0" (row-major order)
    //   Dimensions: [2, 2, 2]
    
    std::vector<std::string> inputStrings = {"aa", "bb", "cc", "dd", "ee", "ff", "gg", "hh"};
    
    // Create TypedData for 3D string input
    genie::core::record::annotation_access_unit::TypedData inputData(
        genie::core::DataType::STRING, 
        3, 
        std::vector<uint32_t>{2, 2, 2}
    );
    
    // Write input data to stream
    auto& inputStream = inputData.getDataStream();
    for (const auto& str : inputStrings) {
        inputStream.write(str.c_str(), str.length() + 1);
    }
    
    // Create encoder and configure
    genie::entropy::ser::SEREncoder encoder;
    genie::entropy::ser::SERParameters params(false);  // order = false
    encoder.configure(params);
    encoder.setInput(0, inputData);
    
    // Perform encoding
    encoder.encode();
    
    // Get outputs
    auto& symbolOutput = encoder.getOutput(0);
    auto& dimensionsOutput = encoder.getOutput(1);
    
    // Verify symbols
    std::string symbolStr = symbolOutput.getDataStream().str();
    const char* symbols = symbolStr.c_str();
    
    // Parse strings from output
    std::vector<std::string> outputStrings;
    size_t offset = 0;
    while (offset < symbolStr.size()) {
        std::string str(symbols + offset);
        outputStrings.push_back(str);
        offset += str.length() + 1;
    }
    
    ASSERT_EQ(outputStrings.size(), 8);
    for (size_t i = 0; i < outputStrings.size(); ++i) {
        EXPECT_EQ(outputStrings[i], inputStrings[i]);
    }
    
    // Verify dimensions
    std::string dimensionsStr = dimensionsOutput.getDataStream().str();
    std::vector<uint32_t> dimensions = readDimensions(dimensionsStr);
    
    ASSERT_EQ(dimensions.size(), 3);
    EXPECT_EQ(dimensions[0], 2);
    EXPECT_EQ(dimensions[1], 2);
    EXPECT_EQ(dimensions[2], 2);
}

// Test SER encoding with STRING data type (variable length strings)
TEST_F(SERTestCase, SEREncodeStringVariableLengthTest) {  // NOLINT(cert-err58-cpp)
    // Create input data with strings of varying lengths
    std::vector<std::string> inputStrings = {"", "a", "hello world", "test123"};
    
    // Create TypedData for 1D string input
    genie::core::record::annotation_access_unit::TypedData inputData(
        genie::core::DataType::STRING, 
        1, 
        std::vector<uint32_t>{static_cast<uint32_t>(inputStrings.size())}
    );
    
    // Write input data to stream
    auto& inputStream = inputData.getDataStream();
    for (const auto& str : inputStrings) {
        inputStream.write(str.c_str(), str.length() + 1);
    }
    
    // Create encoder and configure
    genie::entropy::ser::SEREncoder encoder;
    genie::entropy::ser::SERParameters params(false);
    encoder.configure(params);
    encoder.setInput(0, inputData);
    
    // Perform encoding
    encoder.encode();
    
    // Get outputs
    auto& symbolOutput = encoder.getOutput(0);
    
    // Verify symbols
    std::string symbolStr = symbolOutput.getDataStream().str();
    const char* symbols = symbolStr.c_str();
    
    // Parse strings from output
    std::vector<std::string> outputStrings;
    size_t offset = 0;
    while (offset < symbolStr.size()) {
        std::string str(symbols + offset);
        outputStrings.push_back(str);
        offset += str.length() + 1;
    }
    
    ASSERT_EQ(outputStrings.size(), inputStrings.size());
    for (size_t i = 0; i < outputStrings.size(); ++i) {
        EXPECT_EQ(outputStrings[i], inputStrings[i]);
    }
}

// Test SER encoding with STRING data type (3D array, order = true)
TEST_F(SERTestCase, SEREncodeString3DReverseOrderTest) {  // NOLINT(cert-err58-cpp)
    // Create input data: 2x2x2 cube of strings
    // Index layout (i, j, k):
    // (0,0,0)="0", (0,0,1)="1", (0,1,0)="2", (0,1,1)="3"
    // (1,0,0)="4", (1,0,1)="5", (1,1,0)="6", (1,1,1)="7"
    // With order=true (reverse), iterate k first, then j, then i:
    // Expected: "0", "4", "2", "6", "1", "5", "3", "7"
    
    std::vector<std::string> inputStrings = {"0", "1", "2", "3", "4", "5", "6", "7"};
    
    // Create TypedData for 3D string input
    genie::core::record::annotation_access_unit::TypedData inputData(
        genie::core::DataType::STRING, 
        3, 
        std::vector<uint32_t>{2, 2, 2}
    );
    
    // Write input data to stream (row-major order)
    auto& inputStream = inputData.getDataStream();
    for (const auto& str : inputStrings) {
        inputStream.write(str.c_str(), str.length() + 1);
    }
    
    // Create encoder and configure with reverse order
    genie::entropy::ser::SEREncoder encoder;
    genie::entropy::ser::SERParameters params(true);  // order = true (reverse)
    encoder.configure(params);
    encoder.setInput(0, inputData);
    
    // Perform encoding
    encoder.encode();
    
    // Get outputs
    auto& symbolOutput = encoder.getOutput(0);
    
    // Verify symbols (should be in reverse dimension order)
    std::string symbolStr = symbolOutput.getDataStream().str();
    const char* symbols = symbolStr.c_str();
    
    // Parse strings from output
    std::vector<std::string> outputStrings;
    size_t offset = 0;
    while (offset < symbolStr.size()) {
        std::string str(symbols + offset);
        outputStrings.push_back(str);
        offset += str.length() + 1;
    }
    
    ASSERT_EQ(outputStrings.size(), 8);
    std::vector<std::string> expectedOrder = {"0", "4", "2", "6", "1", "5", "3", "7"};
    for (size_t i = 0; i < outputStrings.size(); ++i) {
        EXPECT_EQ(outputStrings[i], expectedOrder[i]);
    }
}
