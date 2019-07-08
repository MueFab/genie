#include <algorithm>
#include <functional>
#include <iterator>
#include <iostream>
#include <limits>
#include <vector>

#include "gabac/data_block.h"
#include "gabac/diff_coding.h"
#include "./test_common.h"

#include "gtest/gtest.h"


class DiffCodingTest : public ::testing::Test
{
 protected:
    DiffCodingTest() = default;

    ~DiffCodingTest() override = default;

    void SetUp() override{
    }

    void TearDown() override{
    }
};

TEST_F(DiffCodingTest, transformDiffCoding){
    {
        // Void input
        gabac::DataBlock symbols(0, 8);
        gabac::DataBlock transformedSymbols(0, 8);
        EXPECT_NO_THROW(gabac::transformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.size(), 0);
    }

    {
        gabac::DataBlock transformedSymbols(0, 8);
        transformedSymbols = {42};
        EXPECT_NO_THROW(gabac::transformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.size(), 1);
        EXPECT_EQ(transformedSymbols.get(0), 42);
    }

    {
        // Corner cases
        gabac::DataBlock transformedSymbols(0, 8);
        transformedSymbols = {
                0,
                static_cast<uint64_t>(std::numeric_limits<int64_t>::max()),
                static_cast<uint64_t>(std::numeric_limits<int64_t>::max()) + 1,
                0
        };
        gabac::DataBlock expectedTransformedSymbols(0, 8);
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
        gabac::DataBlock symbols(0, 8);
        gabac::DataBlock transformedSymbols(0, 8);
        size_t largeTestSize = 1024 * 1024;
        symbols.resize(largeTestSize);
        fillVectorRandomUniform(
                0,
                std::numeric_limits<int64_t>::max(),
                &symbols
        );
        EXPECT_NO_THROW(gabac::transformDiffCoding(&transformedSymbols));
    }

    {
        // Too small symbol diff
        gabac::DataBlock transformedSymbols(0, 8);
        transformedSymbols = {uint64_t(std::numeric_limits<int64_t>::max()) + 2, 0};
        EXPECT_NO_THROW(gabac::transformDiffCoding(&transformedSymbols));
        EXPECT_NO_THROW(gabac::inverseTransformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.get(0), uint64_t(std::numeric_limits<int64_t>::max()) + 2);
        EXPECT_EQ(transformedSymbols.get(1), 0);
    }

    {
        // Too great symbol diff
        gabac::DataBlock transformedSymbols(0, 8);
        transformedSymbols = {0, uint64_t(std::numeric_limits<int64_t>::max()) + 1};
        EXPECT_NO_THROW(gabac::transformDiffCoding(&transformedSymbols));
        EXPECT_NO_THROW(gabac::inverseTransformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.get(0), 0);
        EXPECT_EQ(transformedSymbols.get(1), uint64_t(std::numeric_limits<int64_t>::max()) + 1);
    }
}


TEST_F(DiffCodingTest, inverseTransformDiffCoding){
    {
        // Void input shall lead to void output
        gabac::DataBlock transformedSymbols(0, 8);
        EXPECT_NO_THROW(gabac::inverseTransformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.size(), 0);
    }

    {
        // Correct libs of a single positive-valued symbol
        gabac::DataBlock transformedSymbols(0, 8);
        transformedSymbols = {42};
        EXPECT_NO_THROW(gabac::inverseTransformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.size(), 1);
        EXPECT_EQ(transformedSymbols.get(0), 42);
    }
    {
        // Correct libs of a negative-valued symbol
        gabac::DataBlock transformedSymbols(0, 8);
        transformedSymbols = {43, uint64_t(-42)};
        EXPECT_NO_THROW(gabac::inverseTransformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.size(), 2);
        EXPECT_EQ(transformedSymbols.get(1), 1);
    }
    {
        // pos/neg sequence
        gabac::DataBlock expected(0, 8);
        expected = {100, 90, 80, 70, 60, 50};
        gabac::DataBlock transformedSymbols(0, 8);
        transformedSymbols = {100, uint64_t(-10),
                              uint64_t(-10), uint64_t(-10),
                              uint64_t(-10), uint64_t(-10)};
        EXPECT_NO_THROW(gabac::inverseTransformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.size(), 6);
        EXPECT_EQ(transformedSymbols, expected);
    }
    {
        // Cornercases
        // Edges of symbol range
        gabac::DataBlock expected(0, 8);
        expected = {std::numeric_limits<int64_t>::max(),
                    uint64_t(std::numeric_limits<int64_t>::max()) + 1,
                    0,
                    std::numeric_limits<int64_t>::max(),
                    std::numeric_limits<uint64_t>::max() - 1,
                    uint64_t(std::numeric_limits<int64_t>::max()) - 1};
        gabac::DataBlock transformedSymbols(0, 8);
        transformedSymbols = {std::numeric_limits<int64_t>::max(), 1, uint64_t(std::numeric_limits<int64_t>::min()),
                              std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::max(),
                              uint64_t(std::numeric_limits<int64_t>::min())};
        EXPECT_NO_THROW(gabac::inverseTransformDiffCoding(&transformedSymbols));
        EXPECT_EQ(transformedSymbols.size(), 6);
        EXPECT_EQ(transformedSymbols, expected);
    }
}

TEST_F(DiffCodingTest, roundTripCoding){
    gabac::DataBlock symbols(0, 8);
    gabac::DataBlock transformedSymbols(0, 8);

    size_t largeTestSize = 1024 * 1024;

    symbols.resize(largeTestSize);
    fillVectorRandomUniform(0, std::numeric_limits<int64_t>::max(), &symbols);
    transformedSymbols = symbols;
    EXPECT_NO_THROW(gabac::transformDiffCoding(&transformedSymbols));
    EXPECT_NO_THROW(gabac::inverseTransformDiffCoding(&transformedSymbols));
    EXPECT_EQ(transformedSymbols.size(), symbols.size());
    EXPECT_EQ(transformedSymbols, symbols);
    symbols.clear();
}

