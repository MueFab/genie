#include <genie/entropy/gabac/equality-coding.h>
#include <genie/util/data-block.h>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include "common.h"

TEST(EqualityCodingTest, transformEqualityCoding) {
    {
        // Void input
        genie::util::DataBlock symbols(0, 8);
        genie::util::DataBlock flags(0, 1);
        flags = {1};
        genie::util::DataBlock rawSymbols(0, 8);
        EXPECT_NO_THROW(genie::entropy::gabac::transformEqualityCoding(&rawSymbols, &flags));
        EXPECT_EQ(flags.size(), 0);
        EXPECT_EQ(rawSymbols.size(), 0);
    }
    {
        // Single positive-valued symbol
        genie::util::DataBlock flags(0, 1);
        genie::util::DataBlock rawSymbols(0, 8);
        rawSymbols = {42};
        EXPECT_NO_THROW(genie::entropy::gabac::transformEqualityCoding(&rawSymbols, &flags));
        EXPECT_EQ(rawSymbols.size(), 1);
        EXPECT_EQ(rawSymbols.get(0), 41);
        EXPECT_EQ(flags.size(), 1);
        EXPECT_EQ(flags.get(0), 0);
    }
    {
        // Random sequence
        genie::util::DataBlock flags(0, 1);
        genie::util::DataBlock rawSymbols(0, 8);
        rawSymbols = {uint64_t(-3438430427565543845LL),
                      uint64_t(-3438430427565543845LL),
                      8686590606261860295LL,
                      810438489069303389LL,
                      810438489069303389LL,
                      810438489069303389LL,
                      0};
        genie::util::DataBlock expectedRawSymbols(0, 8);
        expectedRawSymbols = {uint64_t(-3438430427565543845LL) - 1, 8686590606261860295LL, 810438489069303389LL, 0};
        genie::util::DataBlock expectedFlags(0, 1);
        expectedFlags = {0, 1, 0, 0, 1, 1, 0};
        genie::util::DataBlock symbols = rawSymbols;
        EXPECT_NO_THROW(genie::entropy::gabac::transformEqualityCoding(&rawSymbols, &flags));
        EXPECT_EQ(flags.size(), symbols.size());
        EXPECT_EQ(flags, expectedFlags);

        EXPECT_EQ(rawSymbols.size(), expectedRawSymbols.size());
        EXPECT_EQ(rawSymbols, expectedRawSymbols);
    }
}

TEST(EqualityCodingTest, inverseTransformEqualityCoding) {
    {
        // Void input
        genie::util::DataBlock flags(0, 1);
        flags = {1};
        genie::util::DataBlock rawSymbols(0, 8);
        EXPECT_NO_THROW(genie::entropy::gabac::inverseTransformEqualityCoding(&rawSymbols, &flags));
        EXPECT_EQ(rawSymbols.size(), 1);
        EXPECT_EQ(rawSymbols.get(0), 0);
    }
    {
        // void input
        genie::util::DataBlock flags(0, 1);
        genie::util::DataBlock rawSymbols(0, 8);
        rawSymbols = {1};
        EXPECT_NO_THROW(genie::entropy::gabac::inverseTransformEqualityCoding(&rawSymbols, &flags));
        EXPECT_EQ(rawSymbols.size(), 0);
    }
    {
        // Single positive-valued symbol
        genie::util::DataBlock flags(0, 1);
        flags = {0};
        genie::util::DataBlock rawSymbols(0, 8);
        rawSymbols = {41};
        EXPECT_NO_THROW(genie::entropy::gabac::inverseTransformEqualityCoding(&rawSymbols, &flags));
        EXPECT_EQ(rawSymbols.size(), 1);
        EXPECT_EQ(rawSymbols.get(0), 42);
    }
    {
        // Single negative-valued symbol
        genie::util::DataBlock flags(0, 1);
        flags = {0};
        genie::util::DataBlock rawSymbols(0, 8);
        rawSymbols = {uint64_t(-42)};
        EXPECT_NO_THROW(genie::entropy::gabac::inverseTransformEqualityCoding(&rawSymbols, &flags));
        EXPECT_EQ(rawSymbols.size(), 1);
        EXPECT_EQ(rawSymbols.get(0), uint64_t(-42) + 1);
    }
    {
        // Random sequence with positive and negative values
        genie::util::DataBlock flags(0, 1);
        flags = {0, 1, 0, 0, 1, 1, 0};
        genie::util::DataBlock expectedSymbols(0, 8);
        expectedSymbols = {uint64_t(-3438430427565543845LL) + 1,
                           uint64_t(-3438430427565543845LL) + 1,
                           8686590606261860294LL,
                           810438489069303389LL,
                           810438489069303389LL,
                           810438489069303389LL,
                           0};
        genie::util::DataBlock rawSymbols(0, 8);
        rawSymbols = {uint64_t(-3438430427565543845LL), 8686590606261860294LL, 810438489069303389LL, 0};
        EXPECT_NO_THROW(genie::entropy::gabac::inverseTransformEqualityCoding(&rawSymbols, &flags));
        EXPECT_EQ(rawSymbols.size(), expectedSymbols.size());
        EXPECT_EQ(rawSymbols, expectedSymbols);
    }
}

TEST(EqualityCodingTest, roundTripCoding) {
    genie::util::DataBlock symbols(0, 8);
    genie::util::DataBlock rawSymbols(0, 8);
    genie::util::DataBlock flags(0, 1);
    genie::util::DataBlock decodedSymbols(0, 8);

    // A lot of input data - WordSize
    symbols.resize(1024 * 1024);
    gabac_tests::fillVectorRandomGeometric(&symbols);
    rawSymbols = symbols;
    EXPECT_NO_THROW(genie::entropy::gabac::transformEqualityCoding(&symbols, &flags));
    EXPECT_NO_THROW(genie::entropy::gabac::inverseTransformEqualityCoding(&symbols, &flags));
    EXPECT_EQ(rawSymbols.size(), symbols.size());
    EXPECT_EQ(rawSymbols, symbols);
    symbols.clear();
}
