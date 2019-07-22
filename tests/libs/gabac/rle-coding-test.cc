#include <vector>

#include <gabac/rle-coding.h>
#include <iostream>

#include <gtest/gtest.h>

#include "test-common.h"


class rleCodingTest : public ::testing::Test
{
 protected:
    void SetUp() override{
    }

    void TearDown() override{
        // Code here will be called immediately after each test
    }
};


TEST_F(rleCodingTest, transformRleCoding){
    {
        // Void input
        uint64_t guard = 42;
        gabac::DataBlock rawSymbols(0, 8);
        rawSymbols = {};
        gabac::DataBlock lengths(0, 8);
        lengths = {1};
        EXPECT_NO_THROW(gabac::transformRleCoding(guard, &rawSymbols, &lengths));
        EXPECT_EQ(rawSymbols.size(), 0);
        EXPECT_EQ(lengths.size(), 0);
    }
    {
        // Single positive-valued symbol
        uint64_t guard = 42;
        gabac::DataBlock rawSymbols(0, 8);
        rawSymbols = {42};
        gabac::DataBlock lengths(0, 8);
        EXPECT_NO_THROW(gabac::transformRleCoding(guard, &rawSymbols, &lengths));
        EXPECT_EQ(rawSymbols.size(), 1);
        EXPECT_EQ(rawSymbols.get(0), 42);
        EXPECT_EQ(lengths.size(), 1);
        EXPECT_EQ(lengths.get(0), 0);
    }
    {
        // Single negative-valued symbol
        uint64_t guard = 42;
        gabac::DataBlock rawSymbols(0, 8);
        rawSymbols = {uint64_t(-42)};
        gabac::DataBlock lengths(0, 8);
        EXPECT_NO_THROW(gabac::transformRleCoding(guard, &rawSymbols, &lengths));
        EXPECT_EQ(rawSymbols.size(), 1);
        EXPECT_EQ(rawSymbols.get(0), uint64_t(-42));
        EXPECT_EQ(lengths.size(), 1);
        EXPECT_EQ(lengths.get(0), 0);
    }
    {
        // Guard triggered
        uint64_t guard = 2;
        gabac::DataBlock rawSymbols(0, 8);
        rawSymbols = {1, 1, 1, 1, 1};
        gabac::DataBlock lengths(0, 8);
        gabac::DataBlock expectedLengths(0, 8);
        expectedLengths = {2, 2, 0};
        EXPECT_NO_THROW(gabac::transformRleCoding(guard, &rawSymbols, &lengths));
        EXPECT_EQ(rawSymbols.size(), 1);
        EXPECT_EQ(rawSymbols.get(0), 1);
        EXPECT_EQ(lengths.size(), 3);
        EXPECT_EQ(lengths, expectedLengths);
    }
    {
        // Random sequence with positive and negative values
        gabac::DataBlock rawSymbols(0, 8);
        rawSymbols = {
                uint64_t(-3438430427565543845LL),
                uint64_t(-3438430427565543845LL),
                8686590606261860295LL,
                810438489069303389LL,
                810438489069303389LL,
                810438489069303389LL,
                0
        };
        gabac::DataBlock lengths(0, 8);
        gabac::DataBlock expectedRawSymbols(0, 8);
        expectedRawSymbols = {
                uint64_t(-3438430427565543845LL),
                8686590606261860295LL,
                810438489069303389LL,
                0
        };
        gabac::DataBlock expectedLengths(0, 8);
        expectedLengths = {
                1, 0, 2, 0
        };
        uint64_t guard = std::numeric_limits<uint64_t>::max();
        EXPECT_NO_THROW(gabac::transformRleCoding(guard, &rawSymbols, &lengths));
        EXPECT_EQ(rawSymbols.size(), expectedRawSymbols.size());
        EXPECT_EQ(rawSymbols, expectedRawSymbols);

        EXPECT_EQ(lengths.size(), expectedLengths.size());
        EXPECT_EQ(lengths, expectedLengths);
    }
}

TEST_F(rleCodingTest, inverseTransformRleCoding){
    {
        // Void input
        uint64_t guard = 42;
        gabac::DataBlock symbols(0, 8);
        gabac::DataBlock rawSymbols(0, 8);
        gabac::DataBlock lengths(0, 8);
        lengths = {3};
        EXPECT_DEATH(gabac::inverseTransformRleCoding(guard, &symbols, &lengths), "");
    }
    {
        // Void input
        uint64_t guard = 42;
        gabac::DataBlock symbols(0, 8);
        gabac::DataBlock rawSymbols(0, 8);
        rawSymbols = {5};
        gabac::DataBlock lengths(0, 8);
        EXPECT_DEATH(gabac::inverseTransformRleCoding(guard, &symbols, &lengths), "");
    }
    {
        // Single positive-valued symbol
        uint64_t guard = 42;
        gabac::DataBlock rawSymbols(0, 8);
        rawSymbols = {42};
        gabac::DataBlock lengths(0, 8);
        lengths = {0};
        EXPECT_NO_THROW(gabac::inverseTransformRleCoding(guard, &rawSymbols, &lengths));
        EXPECT_EQ(rawSymbols.size(), 1);
        EXPECT_EQ(rawSymbols.get(0), 42);
    }
    {
        // Two negative-valued symbols
        uint64_t guard = 42;
        gabac::DataBlock rawSymbols(0, 8);
        rawSymbols = {uint64_t(-42)};
        gabac::DataBlock lengths(0, 8);
        lengths = {2};
        gabac::DataBlock expected(0, 8);
        expected = {uint64_t(-42), uint64_t(-42), uint64_t(-42)};
        EXPECT_NO_THROW(gabac::inverseTransformRleCoding(guard, &rawSymbols, &lengths));
        EXPECT_EQ(rawSymbols.size(), 3);
        EXPECT_EQ(rawSymbols, expected);
    }
    {
        gabac::DataBlock symbols(0, 8);
        uint64_t guard = 42;
        // Random sequence with positive and negative values
        gabac::DataBlock expected(0, 8);
        expected = {
                uint64_t(-3438430427565543845LL),
                uint64_t(-3438430427565543845LL),
                8686590606261860295LL,
                810438489069303389LL,
                810438489069303389LL,
                810438489069303389LL,
                0
        };

        gabac::DataBlock rawSymbols(0, 8);
        rawSymbols = {
                uint64_t(-3438430427565543845LL),
                8686590606261860295LL,
                810438489069303389LL,
                0
        };
        gabac::DataBlock lengths(0, 8);
        lengths = {
                1, 0, 2, 0
        };
        EXPECT_NO_THROW(gabac::inverseTransformRleCoding(guard, &rawSymbols, &lengths));
        EXPECT_EQ(rawSymbols.size(), expected.size());
        EXPECT_EQ(rawSymbols, expected);
    }
}

TEST_F(rleCodingTest, roundTripCoding){
    gabac::DataBlock rawSymbols(0, 8);
    gabac::DataBlock lengths(0, 8);
    gabac::DataBlock decodedSymbols(0, 8);
    uint64_t guard = 42;

    // A lot of input data - WordSize
    rawSymbols.resize(1 * 1024 * 1024);  // 256M symbols -> 1GB
    fillVectorRandomGeometric(&rawSymbols);
    lengths = {};
    decodedSymbols = rawSymbols;
    EXPECT_NO_THROW(gabac::transformRleCoding(guard, &rawSymbols, &lengths));
    EXPECT_NO_THROW(gabac::inverseTransformRleCoding(guard, &rawSymbols, &lengths));
    EXPECT_EQ(decodedSymbols.size(), rawSymbols.size());
    EXPECT_EQ(decodedSymbols, rawSymbols);
}
