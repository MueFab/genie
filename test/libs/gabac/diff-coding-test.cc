#include <gabac/diff-coding.h>
#include <gtest/gtest.h>
#include <util/data-block.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <vector>
#include "common.h"

TEST(DiffCodingTest, transformDiffCoding) {
    {
        // Void input
        util::DataBlock symbols(0, 8);
        util::DataBlock transformedSymbols(0, 8);
        EXPECT_NO_THROW(gabac::transformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.size(), 0);
    }

    {
        util::DataBlock transformedSymbols(0, 8);
        transformedSymbols = {42};
        EXPECT_NO_THROW(gabac::transformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.size(), 1);
        EXPECT_EQ(transformedSymbols.get(0), 42);
    }

    {
        // Corner cases
        util::DataBlock transformedSymbols(0, 8);
        transformedSymbols = {0, static_cast<uint64_t>(std::numeric_limits<int64_t>::max()),
                              static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) + 1, 0};
        util::DataBlock expectedTransformedSymbols(0, 8);
        expectedTransformedSymbols = {
            0,
            std::numeric_limits<int64_t>::max(),
            1,
            uint64_t(std::numeric_limits<int64_t>::min()),
        };
        EXPECT_NO_THROW(gabac::transformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.size(), expectedTransformedSymbols.size());
        EXPECT_EQ(transformedSymbols, expectedTransformedSymbols);
    }
    {
        // Large input
        util::DataBlock symbols(0, 8);
        util::DataBlock transformedSymbols(0, 8);
        size_t largeTestSize = 1024 * 1024;
        symbols.resize(largeTestSize);
        gabac_tests::fillVectorRandomUniform(0, std::numeric_limits<int64_t>::max(), &symbols);
        EXPECT_NO_THROW(gabac::transformDiffCoding(&transformedSymbols));
    }

    {
        // Too small symbol diff
        util::DataBlock transformedSymbols(0, 8);
        transformedSymbols = {uint64_t(std::numeric_limits<int64_t>::max()) + 2, 0};
        EXPECT_NO_THROW(gabac::transformDiffCoding(&transformedSymbols));
        EXPECT_NO_THROW(gabac::inverseTransformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.get(0), uint64_t(std::numeric_limits<int64_t>::max()) + 2);
        EXPECT_EQ(transformedSymbols.get(1), 0);
    }

    {
        // Too great symbol diff
        util::DataBlock transformedSymbols(0, 8);
        transformedSymbols = {0, uint64_t(std::numeric_limits<int64_t>::max()) + 1};
        EXPECT_NO_THROW(gabac::transformDiffCoding(&transformedSymbols));
        EXPECT_NO_THROW(gabac::inverseTransformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.get(0), 0);
        EXPECT_EQ(transformedSymbols.get(1), uint64_t(std::numeric_limits<int64_t>::max()) + 1);
    }
}

TEST(DiffCodingTest, inverseTransformDiffCoding) {
    {
        // Void input shall lead to void output
        util::DataBlock transformedSymbols(0, 8);
        EXPECT_NO_THROW(gabac::inverseTransformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.size(), 0);
    }

    {
        // Correct libs of a single positive-valued symbol
        util::DataBlock transformedSymbols(0, 8);
        transformedSymbols = {42};
        EXPECT_NO_THROW(gabac::inverseTransformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.size(), 1);
        EXPECT_EQ(transformedSymbols.get(0), 42);
    }
    {
        // Correct libs of a negative-valued symbol
        util::DataBlock transformedSymbols(0, 8);
        transformedSymbols = {43, uint64_t(-42)};
        EXPECT_NO_THROW(gabac::inverseTransformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.size(), 2);
        EXPECT_EQ(transformedSymbols.get(1), 1);
    }
    {
        // pos/neg sequence
        util::DataBlock expected(0, 8);
        expected = {100, 90, 80, 70, 60, 50};
        util::DataBlock transformedSymbols(0, 8);
        transformedSymbols = {100, uint64_t(-10), uint64_t(-10), uint64_t(-10), uint64_t(-10), uint64_t(-10)};
        EXPECT_NO_THROW(gabac::inverseTransformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.size(), 6);
        EXPECT_EQ(transformedSymbols, expected);
    }
    {
        // Cornercases
        // Edges of symbol range
        util::DataBlock expected(0, 8);
        expected = {std::numeric_limits<int64_t>::max(),
                    uint64_t(std::numeric_limits<int64_t>::max()) + 1,
                    0,
                    std::numeric_limits<int64_t>::max(),
                    std::numeric_limits<uint64_t>::max() - 1,
                    uint64_t(std::numeric_limits<int64_t>::max()) - 1};
        util::DataBlock transformedSymbols(0, 8);
        transformedSymbols = {
            std::numeric_limits<int64_t>::max(),           1,
            uint64_t(std::numeric_limits<int64_t>::min()), std::numeric_limits<int64_t>::max(),
            std::numeric_limits<int64_t>::max(),           uint64_t(std::numeric_limits<int64_t>::min())};
        EXPECT_NO_THROW(gabac::inverseTransformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.size(), 6);
        EXPECT_EQ(transformedSymbols, expected);
    }
}

TEST(DiffCodingTest, roundTripCoding) {
    util::DataBlock symbols(0, 8);
    util::DataBlock transformedSymbols(0, 8);

    size_t largeTestSize = 1024 * 1024;

    symbols.resize(largeTestSize);
    gabac_tests::fillVectorRandomUniform(0, std::numeric_limits<int64_t>::max(), &symbols);
    transformedSymbols = symbols;
    EXPECT_NO_THROW(gabac::transformDiffCoding(&transformedSymbols));
    EXPECT_NO_THROW(gabac::inverseTransformDiffCoding(&transformedSymbols));
    EXPECT_EQ(transformedSymbols.size(), symbols.size());
    EXPECT_EQ(transformedSymbols, symbols);
    symbols.clear();
}
