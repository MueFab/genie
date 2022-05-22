#include <genie/entropy/gabac/rle-subseq-transform.h>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include "common.h"

genie::entropy::paramcabac::Subsequence createConfig(uint16_t guard) {
    genie::entropy::paramcabac::TransformedParameters param(
        genie::entropy::paramcabac::TransformedParameters::TransformIdSubseq::RLE_CODING, guard);

    std::vector<genie::entropy::paramcabac::TransformedSubSeq> vec;
    genie::entropy::paramcabac::Subsequence cfg(std::move(param), 0, true, std::move(vec));

    return cfg;
}

TEST(RleCodingTest, voidInput) {
    genie::util::DataBlock values(0, 1);
    genie::util::DataBlock lengths(0, 2);

    std::vector<genie::util::DataBlock> transformSubset = {values, lengths};
    auto cfg = createConfig(10);

    // encode
    genie::entropy::gabac::transformRleCoding(cfg, &transformSubset);
    lengths = transformSubset[0];
    values = transformSubset[1];
    EXPECT_EQ(lengths.size(), 0);
    EXPECT_EQ(values.size(), 0);

    // decode
    genie::entropy::gabac::inverseTransformRleCoding(cfg, &transformSubset);
    values = transformSubset[0];
    EXPECT_EQ(values.size(), 0);
}

TEST(RleCodingTest, singlePositivValue) {
    genie::util::DataBlock values(0, 8);
    genie::util::DataBlock lengths(0, 1);
    genie::util::DataBlock expectedValues(0, 8);
    genie::util::DataBlock expectedLengths(0, 1);

    values = {42};

    std::vector<genie::util::DataBlock> transformSubset = {values, lengths};
    auto cfg = createConfig(32);

    // encode
    genie::entropy::gabac::transformRleCoding(cfg, &transformSubset);
    lengths = transformSubset[0];
    values = transformSubset[1];

    expectedLengths = {0};
    expectedValues = {42};

    EXPECT_EQ(lengths, expectedLengths);
    EXPECT_EQ(values, expectedValues);

    // decode
    genie::entropy::gabac::inverseTransformRleCoding(cfg, &transformSubset);
    values = transformSubset[0];

    expectedValues = {42};

    EXPECT_EQ(values, expectedValues);
}

TEST(RleCodingTest, singleNegativeValue) {
    genie::util::DataBlock values(0, 8);
    genie::util::DataBlock lengths(0, 1);
    genie::util::DataBlock expectedValues(0, 8);
    genie::util::DataBlock expectedLengths(0, 1);

    values = {uint64_t(-42)};

    std::vector<genie::util::DataBlock> transformSubset = {values, lengths};
    auto cfg = createConfig(32);

    // encode
    genie::entropy::gabac::transformRleCoding(cfg, &transformSubset);
    lengths = transformSubset[0];
    values = transformSubset[1];

    expectedLengths = {0};
    expectedValues = {uint64_t(-42)};

    EXPECT_EQ(lengths, expectedLengths);
    EXPECT_EQ(values, expectedValues);

    // decode
    genie::entropy::gabac::inverseTransformRleCoding(cfg, &transformSubset);
    values = transformSubset[0];

    expectedValues = {uint64_t(-42)};

    EXPECT_EQ(values, expectedValues);
}

TEST(RleCodingTest, triggerGuard) {
    genie::util::DataBlock values(0, 8);
    genie::util::DataBlock lengths(0, 1);
    genie::util::DataBlock expectedValues(0, 8);
    genie::util::DataBlock expectedLengths(0, 1);

    values = {1, 1, 1, 1, 1, 3, 2, 2, 2, 2, 1, 1, 1, 3};

    std::vector<genie::util::DataBlock> transformSubset = {values, lengths};
    auto cfg = createConfig(3);

    // encode
    genie::entropy::gabac::transformRleCoding(cfg, &transformSubset);
    lengths = transformSubset[0];
    values = transformSubset[1];

    expectedLengths = {3, 1, 0, 3, 0, 2, 0};
    expectedValues = {1, 3, 2, 1, 3};

    EXPECT_EQ(lengths, expectedLengths);
    EXPECT_EQ(values, expectedValues);

    // decode
    genie::entropy::gabac::inverseTransformRleCoding(cfg, &transformSubset);
    values = transformSubset[0];

    expectedValues = {1, 1, 1, 1, 1, 3, 2, 2, 2, 2, 1, 1, 1, 3};

    EXPECT_EQ(values, expectedValues);
}

TEST(RleCodingTest, semiRandom) {
    genie::util::DataBlock values(0, 8);
    genie::util::DataBlock lengths(0, 1);
    genie::util::DataBlock expectedValues(0, 8);
    genie::util::DataBlock expectedLengths(0, 1);

    values = {uint64_t(-3438430427565543845LL),
              uint64_t(-3438430427565543845LL),
              8686590606261860295LL,
              810438489069303389LL,
              810438489069303389LL,
              810438489069303389LL,
              0};

    std::vector<genie::util::DataBlock> transformSubset = {values, lengths};
    auto cfg = createConfig(std::numeric_limits<uint16_t>::max());

    // encode
    genie::entropy::gabac::transformRleCoding(cfg, &transformSubset);
    lengths = transformSubset[0];
    values = transformSubset[1];

    expectedLengths = {1, 0, 2, 0};
    expectedValues = {uint64_t(-3438430427565543845LL), 8686590606261860295LL, 810438489069303389LL, 0};

    EXPECT_EQ(lengths, expectedLengths);
    EXPECT_EQ(values, expectedValues);

    // decode
    genie::entropy::gabac::inverseTransformRleCoding(cfg, &transformSubset);
    values = transformSubset[0];

    expectedValues = {uint64_t(-3438430427565543845LL),
                      uint64_t(-3438430427565543845LL),
                      8686590606261860295LL,
                      810438489069303389LL,
                      810438489069303389LL,
                      810438489069303389LL,
                      0};

    EXPECT_EQ(values, expectedValues);
}

TEST(RleCodingTest, roundTripCoding) {
    genie::util::DataBlock values(0, 8);
    genie::util::DataBlock lengths(0, 1);
    genie::util::DataBlock expectedValues(0, 8);
    genie::util::DataBlock expectedLengths(0, 1);

    values.resize(1024 * 1024);
    gabac_tests::fillVectorRandomGeometric(&values);

    std::vector<genie::util::DataBlock> transformSubset = {values, lengths};
    auto cfg = createConfig(42);

    // encode + decode
    genie::entropy::gabac::transformRleCoding(cfg, &transformSubset);
    genie::entropy::gabac::inverseTransformRleCoding(cfg, &transformSubset);

    EXPECT_EQ(values, transformSubset[0]);
}
