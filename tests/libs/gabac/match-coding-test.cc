#include <gabac/data-block.h>
#include <gabac/match-coding.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>
#include <vector>
#include "common.h"

class MatchCodingTest : public ::testing::Test {
   protected:
    const unsigned int largeTestSize = 1 * 10 * 1024;
    std::vector<int> windowSizes = {1024};  // minimum value: 0,
    // which should not perform a search, maximum reasonable value: 32768
};

TEST_F(MatchCodingTest, transformMatchCoding) {
    gabac::DataBlock symbols(0, 8);
    gabac::DataBlock pointers(0, 4);
    gabac::DataBlock expectedPointers(0, 4);
    gabac::DataBlock lengths(0, 4);
    gabac::DataBlock expectedLengths(0, 4);
    gabac::DataBlock rawValues(0, 8);
    gabac::DataBlock expectedRawValues(0, 8);
    int windowSize;

    for (auto& windowSizeIt : windowSizes) {
        EXPECT_NO_THROW(gabac::transformMatchCoding(windowSizeIt, &rawValues, &pointers, &lengths));
        EXPECT_EQ(pointers.size(), 0);
        EXPECT_EQ(lengths.size(), 0);
        EXPECT_EQ(rawValues.size(), 0);
    }

    // windowSize == 0 - should return raw_values
    // note: windowSize 1 will also result in no match as our implementation
    // requires length to be >2, but according to the standard is allowed
    rawValues = {uint64_t(-1), 2, uint64_t(-3), 4, 4, uint64_t(-3), 2, uint64_t(-1)};
    pointers = {};
    lengths = {};
    expectedRawValues = {uint64_t(-1), 2, uint64_t(-3), 4, 4, uint64_t(-3), 2, uint64_t(-1)};
    windowSize = 0;
    EXPECT_NO_THROW(gabac::transformMatchCoding(windowSize, &rawValues, &pointers, &lengths));
    EXPECT_EQ(pointers.size(), 0);
    EXPECT_EQ(lengths.size(), 8);
    EXPECT_EQ(rawValues.size(), 8);
    EXPECT_EQ(rawValues, expectedRawValues);

    // windowSize == 4
    rawValues = {uint64_t(-1), 2, uint64_t(-3), 4, 2, uint64_t(-3), 4, uint64_t(-1)};
    pointers = {};
    expectedPointers = {3};
    lengths = {};
    expectedLengths = {0, 0, 0, 0, 3, 0};
    expectedRawValues = {uint64_t(-1), 2, uint64_t(-3), 4, uint64_t(-1)};
    windowSize = 4;
    EXPECT_NO_THROW(gabac::transformMatchCoding(windowSize, &rawValues, &pointers, &lengths));
    EXPECT_EQ(pointers.size(), 1);
    EXPECT_EQ(pointers, expectedPointers);
    EXPECT_EQ(lengths.size(), 6);
    EXPECT_EQ(lengths, expectedLengths);
    EXPECT_EQ(rawValues.size(), 5);
    EXPECT_EQ(rawValues, expectedRawValues);
}

TEST_F(MatchCodingTest, inverseTransformMatchCoding) {
    gabac::DataBlock expectedSymbols(0, 8);
    gabac::DataBlock pointers(0, 4);
    gabac::DataBlock lengths(0, 4);
    gabac::DataBlock rawValues(0, 8);

    // Void input shall lead to void output
    pointers = {};
    lengths = {};
    rawValues = {};
    EXPECT_NO_THROW(gabac::inverseTransformMatchCoding(&rawValues, &pointers, &lengths));
    EXPECT_EQ(rawValues.size(), 0);

    // void rawValues (is not allowed) and lengths is not
    pointers = {};
    lengths = {0, 0, 0, 0};
    rawValues = {};

    // void rawValues (is not allowed) and lengths is not
    EXPECT_DEATH(gabac::inverseTransformMatchCoding(&rawValues, &pointers, &lengths), "");
    pointers = {0, 0, 0, 0};
    lengths = {};
    rawValues = {};
    EXPECT_DEATH(gabac::inverseTransformMatchCoding(&rawValues, &pointers, &lengths), "");

    // windowSize == 4
    expectedSymbols = {uint64_t(-1), 2, uint64_t(-3), 4, 2, uint64_t(-3), 4, uint64_t(-1)};
    pointers = {3};
    lengths = {0, 0, 0, 0, 3, 0};
    rawValues = {uint64_t(-1), 2, uint64_t(-3), 4, uint64_t(-1)};
    EXPECT_NO_THROW(gabac::inverseTransformMatchCoding(&rawValues, &pointers, &lengths));
    EXPECT_EQ(rawValues.size(), expectedSymbols.size());
    EXPECT_EQ(rawValues, expectedSymbols);
}

TEST_F(MatchCodingTest, roundTripCoding) {
    gabac::DataBlock symbols(0, 8);
    gabac::DataBlock decodedSymbols(0, 8);
    gabac::DataBlock pointers(0, 4);
    gabac::DataBlock lengths(0, 4);
    gabac::DataBlock rawValues(0, 8);

    // test large file size word size 1
    symbols.resize(largeTestSize);
    gabac_tests::fillVectorRandomUniform(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max(),
                                         &symbols);

    rawValues = symbols;

    for (auto& windowSize : windowSizes) {
        EXPECT_NO_THROW(gabac::transformMatchCoding(windowSize, &rawValues, &pointers, &lengths));
        EXPECT_NO_THROW(gabac::inverseTransformMatchCoding(&rawValues, &pointers, &lengths));
        EXPECT_EQ(symbols.size(), rawValues.size());
        EXPECT_EQ(symbols, rawValues);
    }

    // test large file size word size 2
    symbols.resize(largeTestSize);
    gabac_tests::fillVectorRandomUniform(std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max(),
                                         &symbols);
    rawValues = symbols;
    for (auto& windowSize : windowSizes) {
        EXPECT_NO_THROW(gabac::transformMatchCoding(windowSize, &rawValues, &pointers, &lengths));
        EXPECT_NO_THROW(gabac::inverseTransformMatchCoding(&rawValues, &pointers, &lengths));
        EXPECT_EQ(symbols.size(), rawValues.size());
        EXPECT_EQ(symbols, rawValues);
    }
    // test large file size word size 4
    symbols.resize(largeTestSize);
    gabac_tests::fillVectorRandomUniform(std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max(),
                                         &symbols);
    rawValues = symbols;
    for (auto& windowSize : windowSizes) {
        EXPECT_NO_THROW(gabac::transformMatchCoding(windowSize, &rawValues, &pointers, &lengths));
        EXPECT_NO_THROW(gabac::inverseTransformMatchCoding(&rawValues, &pointers, &lengths));
        EXPECT_EQ(symbols.size(), rawValues.size());
        EXPECT_EQ(symbols, rawValues);
    }
    symbols.clear();
}
