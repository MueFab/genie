#include <genie/entropy/gabac/match-subseq-transform.h>
#include <genie/util/data-block.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <functional>
#include <vector>
#include "common.h"

class MatchCodingTest : public ::testing::Test {
 protected:
    const unsigned int largeTestSize = 1 * 10 * 1024;
    std::vector<int> windowSizes = {1024};  // minimum value: 0,
    // which should not perform a search, maximum reasonable value: 32768

    genie::util::DataBlock symbols;
    genie::util::DataBlock pointers;
    genie::util::DataBlock expectedPointers;
    genie::util::DataBlock lengths;
    genie::util::DataBlock expectedLengths;
    genie::util::DataBlock rawValues;
    genie::util::DataBlock expectedRawValues;
    std::vector<genie::util::DataBlock> transformSubset;

    void SetUp() override {
        symbols = genie::util::DataBlock(0, 4);
        pointers = genie::util::DataBlock(0, 4);
        expectedPointers = genie::util::DataBlock(0, 4);
        lengths = genie::util::DataBlock(0, 4);
        expectedLengths = genie::util::DataBlock(0, 4);
        rawValues = genie::util::DataBlock(0, 8);
        expectedRawValues = genie::util::DataBlock(0, 8);
        transformSubset = std::vector<genie::util::DataBlock>();
    }

    static genie::entropy::paramcabac::Subsequence createConfig(uint16_t matchBufferSize) {
        genie::entropy::paramcabac::TransformedParameters param(
            genie::entropy::paramcabac::TransformedParameters::TransformIdSubseq::MATCH_CODING, matchBufferSize);

        std::vector<genie::entropy::paramcabac::TransformedSubSeq> vec;
        genie::entropy::paramcabac::Subsequence cfg(std::move(param), 0, true, std::move(vec));

        return cfg;
    }
};

TEST_F(MatchCodingTest, voidInput) {
    for (const int& windowSizeIt : windowSizes) {
        auto cfg = createConfig(windowSizeIt);
        transformSubset = {rawValues};

        // encode
        EXPECT_NO_THROW(genie::entropy::gabac::transformMatchCoding(cfg, &transformSubset));
        EXPECT_EQ(transformSubset[0].size(), 0);
        EXPECT_EQ(transformSubset[1].size(), 0);
        EXPECT_EQ(transformSubset[2].size(), 0);

        // decode
        EXPECT_NO_THROW(genie::entropy::gabac::transformMatchCoding(cfg, &transformSubset));
        EXPECT_EQ(transformSubset[0].size(), 0);
        EXPECT_EQ(transformSubset[1].size(), 0);
        EXPECT_EQ(transformSubset[2].size(), 0);
    }
}

TEST_F(MatchCodingTest, transformMatchCoding) {
    // windowSize == 0 - should return raw_values
    // note: windowSize 1 will also result in no match as our implementation
    // requires length to be >2, but according to the standard is allowed
    rawValues = {uint64_t(-1), 2, uint64_t(-3), 4, 4, uint64_t(-3), 2, uint64_t(-1)};
    pointers = {};
    lengths = {};
    expectedRawValues = {uint64_t(-1), 2, uint64_t(-3), 4, 4, uint64_t(-3), 2, uint64_t(-1)};

    auto cfg = createConfig(1);  // TODO: ask about 0
    transformSubset = {rawValues};

    EXPECT_NO_THROW(genie::entropy::gabac::transformMatchCoding(cfg, &transformSubset));
    pointers = transformSubset[0];
    lengths = transformSubset[1];
    rawValues = transformSubset[2];

    EXPECT_EQ(pointers.size(), 0);
    EXPECT_EQ(lengths.size(), 8);
    EXPECT_EQ(rawValues.size(), 8);
    EXPECT_EQ(rawValues, expectedRawValues);

    // reset
    SetUp();
    // windowSize == 4
    int windowSize = 4;
    rawValues = {uint64_t(-1), 2, uint64_t(-3), 4, 2, uint64_t(-3), 4, uint64_t(-1)};
    pointers = {};
    lengths = {};

    // reduce wordSize to match returned DataBlocks
    expectedPointers.setWordSize(1);
    expectedLengths.setWordSize(1);
    expectedPointers = {3};
    expectedLengths = {0, 0, 0, 0, 3, 0};
    expectedRawValues = {uint64_t(-1), 2, uint64_t(-3), 4, uint64_t(-1)};

    cfg = createConfig(windowSize);
    transformSubset = {rawValues};

    EXPECT_NO_THROW(genie::entropy::gabac::transformMatchCoding(cfg, &transformSubset));
    pointers = transformSubset[0];
    lengths = transformSubset[1];
    rawValues = transformSubset[2];

    EXPECT_EQ(pointers.size(), 1);
    EXPECT_EQ(pointers, expectedPointers);
    EXPECT_EQ(lengths.size(), 6);
    EXPECT_EQ(lengths, expectedLengths);
    EXPECT_EQ(rawValues.size(), 5);
    EXPECT_EQ(rawValues, expectedRawValues);
}

TEST_F(MatchCodingTest, inverseTransformMatchCoding) {
    // Void input shall lead to void output
    pointers = {};
    lengths = {};
    rawValues = {};
    transformSubset = {pointers, lengths, rawValues};
    EXPECT_NO_THROW(genie::entropy::gabac::inverseTransformMatchCoding(&transformSubset));
    EXPECT_EQ(transformSubset[0].size(), 0);

    // void rawValues (is not allowed) and lengths is not
    pointers = {};
    lengths = {0, 0, 0, 0};
    rawValues = {};
    transformSubset = {pointers, lengths, rawValues};

    // void rawValues (is not allowed) and lengths is not
    EXPECT_DEATH(genie::entropy::gabac::inverseTransformMatchCoding(&transformSubset), "");

    pointers = {0, 0, 0, 0};
    lengths = {};
    rawValues = {};
    transformSubset = {pointers, lengths, rawValues};
    EXPECT_DEATH(genie::entropy::gabac::inverseTransformMatchCoding(&transformSubset), "");

    // windowSize == 4
    expectedRawValues = {uint64_t(-1), 2, uint64_t(-3), 4, 2, uint64_t(-3), 4, uint64_t(-1)};
    pointers = {3};
    lengths = {0, 0, 0, 0, 3, 0};
    rawValues = {uint64_t(-1), 2, uint64_t(-3), 4, uint64_t(-1)};
    transformSubset = {pointers, lengths, rawValues};
    EXPECT_NO_THROW(genie::entropy::gabac::inverseTransformMatchCoding(&transformSubset));
    EXPECT_EQ(transformSubset[0].size(), expectedRawValues.size());
    EXPECT_EQ(transformSubset[0], expectedRawValues);
}

TEST_F(MatchCodingTest, roundTripCoding) {
    // test large file size word size 1
    symbols.resize(largeTestSize);
    gabac_tests::fillVectorRandomUniform(std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max(),
                                         &symbols);

    rawValues = symbols;

    for (auto& windowSize : windowSizes) {
        auto cfg = createConfig(windowSize);
        transformSubset = {rawValues};
        EXPECT_NO_THROW(genie::entropy::gabac::transformMatchCoding(cfg, &transformSubset));
        EXPECT_NO_THROW(genie::entropy::gabac::inverseTransformMatchCoding(&transformSubset));
        EXPECT_EQ(symbols.size(), transformSubset[0].size());
        EXPECT_EQ(symbols, transformSubset[0]);
    }

    // test large file size word size 2
    symbols.resize(largeTestSize);
    gabac_tests::fillVectorRandomUniform(std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max(),
                                         &symbols);
    rawValues = symbols;
    for (auto& windowSize : windowSizes) {
        auto cfg = createConfig(windowSize);
        transformSubset = {rawValues};
        EXPECT_NO_THROW(genie::entropy::gabac::transformMatchCoding(cfg, &transformSubset));
        EXPECT_NO_THROW(genie::entropy::gabac::inverseTransformMatchCoding(&transformSubset));
        EXPECT_EQ(symbols.size(), transformSubset[0].size());
        EXPECT_EQ(symbols, transformSubset[0]);
    }

    // test large file size word size 4
    symbols.resize(largeTestSize);
    gabac_tests::fillVectorRandomUniform(std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max(),
                                         &symbols);
    rawValues = symbols;
    for (auto& windowSize : windowSizes) {
        auto cfg = createConfig(windowSize);
        transformSubset = {rawValues};
        EXPECT_NO_THROW(genie::entropy::gabac::transformMatchCoding(cfg, &transformSubset));
        EXPECT_NO_THROW(genie::entropy::gabac::inverseTransformMatchCoding(&transformSubset));
        EXPECT_EQ(symbols.size(), transformSubset[0].size());
        EXPECT_EQ(symbols, transformSubset[0]);
    }
    symbols.clear();
}
