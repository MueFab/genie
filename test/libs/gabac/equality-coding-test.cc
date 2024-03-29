#include <genie/entropy/gabac/equality-subseq-transform.h>
#include <genie/util/data-block.h>
#include <gtest/gtest.h>
#include <vector>
#include "common.h"

TEST(EqualityCodingTest, voidInput) {
    genie::util::DataBlock values(0, 8);
    genie::util::DataBlock flags(0, 1);
    std::vector<genie::util::DataBlock> transformSubset = {values, flags};

    // encoding
    EXPECT_NO_THROW(genie::entropy::gabac::transformEqualityCoding(&transformSubset));
    flags = transformSubset[0];
    values = transformSubset[1];
    EXPECT_EQ(flags.size(), 0);
    EXPECT_EQ(values.size(), 0);

    // decoding
    EXPECT_NO_THROW(genie::entropy::gabac::inverseTransformEqualityCoding(&transformSubset));
    values = transformSubset[0];
    EXPECT_EQ(values.size(), 0);
}

TEST(EqualityCodingTest, singlePositiveValue) {
    genie::util::DataBlock values(0, 8);
    genie::util::DataBlock flags(0, 1);
    values = {42};
    std::vector<genie::util::DataBlock> transformSubset = {values, flags};

    // encoding
    EXPECT_NO_THROW(genie::entropy::gabac::transformEqualityCoding(&transformSubset));
    flags = transformSubset[0];
    values = transformSubset[1];
    EXPECT_EQ(flags.size(), 1);
    EXPECT_EQ(flags.get(0), 0);
    EXPECT_EQ(values.size(), 1);
    EXPECT_EQ(values.get(0), 41);

    // decoding
    EXPECT_NO_THROW(genie::entropy::gabac::inverseTransformEqualityCoding(&transformSubset));
    values = transformSubset[0];
    EXPECT_EQ(values.size(), 1);
    EXPECT_EQ(values.get(0), 42);
}

TEST(EqualityCodingTest, semiRandom) {
    genie::util::DataBlock flags(0, 1);
    genie::util::DataBlock values(0, 8);
    values = {uint64_t(-3438430427565543845LL),
              uint64_t(-3438430427565543845LL),
              8686590606261860295LL,
              810438489069303389LL,
              810438489069303389LL,
              810438489069303389LL,
              0};

    std::vector<genie::util::DataBlock> transformSubset = {values, flags};

    genie::util::DataBlock expectedOutput(0, 8);
    expectedOutput = {uint64_t(-3438430427565543845LL) - 1, 8686590606261860295LL, 810438489069303389LL, 0};
    genie::util::DataBlock expectedFlags(0, 1);
    expectedFlags = {0, 1, 0, 0, 1, 1, 0};

    // encoding
    EXPECT_NO_THROW(genie::entropy::gabac::transformEqualityCoding(&transformSubset));
    auto resFlags = transformSubset[0];
    auto resValues = transformSubset[1];
    EXPECT_EQ(resFlags, expectedFlags);
    EXPECT_EQ(resValues, expectedOutput);

    // decoding
    EXPECT_NO_THROW(genie::entropy::gabac::inverseTransformEqualityCoding(&transformSubset));
    resValues = transformSubset[0];
    EXPECT_EQ(resValues, values);
}

TEST(EqualityCodingTest, roundTripCoding) {
    genie::util::DataBlock values(0, 8);
    genie::util::DataBlock flags(0, 1);
    genie::util::DataBlock decodedSymbols(0, 8);

    // A lot of input data - WordSize
    values.resize(1024 * 1024);
    gabac_tests::fillVectorRandomGeometric(&values);

    std::vector<genie::util::DataBlock> transformSubset = {values, flags};
    // auto subsetCopy = transformSubset;
    EXPECT_NO_THROW(genie::entropy::gabac::transformEqualityCoding(&transformSubset));

    // run another version to test against
    // EXPECT_NO_THROW(equalityEncodeTest(subsetCopy));
    // EXPECT_EQ(subsetCopy, transformSubset);

    EXPECT_NO_THROW(genie::entropy::gabac::inverseTransformEqualityCoding(&transformSubset));
    auto result = transformSubset[0];
    EXPECT_EQ(values.size(), result.size());
    EXPECT_EQ(values, result);
}