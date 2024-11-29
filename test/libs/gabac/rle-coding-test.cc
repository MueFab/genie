#include <genie/entropy/gabac/rle_sub_seq_transform.h>
#include <gtest/gtest.h>

#include <vector>

#include "common.h"

genie::entropy::paramcabac::Subsequence CreateConfig(const uint16_t guard) {
  genie::entropy::paramcabac::TransformedParameters param(
      genie::entropy::paramcabac::TransformedParameters::TransformIdSubseq::
          RLE_CODING,
      guard);

  std::vector<genie::entropy::paramcabac::TransformedSubSeq> vec;
  genie::entropy::paramcabac::Subsequence cfg(std::move(param), 0, true,
                                              std::move(vec));

  return cfg;
}

TEST(RleCodingTest, voidInput) {
  genie::util::DataBlock values(0, 1);
  genie::util::DataBlock lengths(0, 2);

  std::vector transform_subset = {values, lengths};
  const auto cfg = CreateConfig(10);

  // encode
  genie::entropy::gabac::TransformRleCoding(cfg, &transform_subset);
  lengths = transform_subset[0];
  values = transform_subset[1];
  EXPECT_EQ(lengths.Size(), 0);
  EXPECT_EQ(values.Size(), 0);

  // Decode
  genie::entropy::gabac::InverseTransformRleCoding(cfg, &transform_subset);
  values = transform_subset[0];
  EXPECT_EQ(values.Size(), 0);
}

TEST(RleCodingTest, singlePositivValue) {
  genie::util::DataBlock values(0, 8);
  genie::util::DataBlock lengths(0, 1);
  genie::util::DataBlock expected_values(0, 8);
  genie::util::DataBlock expected_lengths(0, 1);

  values = {42};

  std::vector transform_subset = {values, lengths};
  const auto cfg = CreateConfig(32);

  // encode
  genie::entropy::gabac::TransformRleCoding(cfg, &transform_subset);
  lengths = transform_subset[0];
  values = transform_subset[1];

  expected_lengths = {0};
  expected_values = {42};

  EXPECT_EQ(lengths, expected_lengths);
  EXPECT_EQ(values, expected_values);

  // Decode
  genie::entropy::gabac::InverseTransformRleCoding(cfg, &transform_subset);
  values = transform_subset[0];
  expected_values = {42};

  EXPECT_EQ(values, expected_values);
}

TEST(RleCodingTest, singleNegativeValue) {
  genie::util::DataBlock values(0, 8);
  genie::util::DataBlock lengths(0, 1);
  genie::util::DataBlock expected_values(0, 8);
  genie::util::DataBlock expected_lengths(0, 1);

  values = {static_cast<uint64_t>(-42)};

  std::vector transform_subset = {values, lengths};
  const auto cfg = CreateConfig(32);

  // encode
  genie::entropy::gabac::TransformRleCoding(cfg, &transform_subset);
  lengths = transform_subset[0];
  values = transform_subset[1];

  expected_lengths = {0};
  expected_values = {static_cast<uint64_t>(-42)};

  EXPECT_EQ(lengths, expected_lengths);
  EXPECT_EQ(values, expected_values);

  // Decode
  genie::entropy::gabac::InverseTransformRleCoding(cfg, &transform_subset);
  values = transform_subset[0];
  expected_values = {static_cast<uint64_t>(-42)};

  EXPECT_EQ(values, expected_values);
}

TEST(RleCodingTest, triggerGuard) {
  genie::util::DataBlock values(0, 8);
  genie::util::DataBlock lengths(0, 1);
  genie::util::DataBlock expected_values(0, 8);
  genie::util::DataBlock expected_lengths(0, 1);

  values = {1, 1, 1, 1, 1, 3, 2, 2, 2, 2, 1, 1, 1, 3};

  std::vector transform_subset = {values, lengths};
  const auto cfg = CreateConfig(3);

  // encode
  genie::entropy::gabac::TransformRleCoding(cfg, &transform_subset);
  lengths = transform_subset[0];
  values = transform_subset[1];

  expected_lengths = {3, 1, 0, 3, 0, 2, 0};
  expected_values = {1, 3, 2, 1, 3};

  EXPECT_EQ(lengths, expected_lengths);
  EXPECT_EQ(values, expected_values);

  // Decode
  genie::entropy::gabac::InverseTransformRleCoding(cfg, &transform_subset);
  values = transform_subset[0];
  expected_values = {1, 1, 1, 1, 1, 3, 2, 2, 2, 2, 1, 1, 1, 3};

  EXPECT_EQ(values, expected_values);
}

TEST(RleCodingTest, semiRandom) {
  genie::util::DataBlock values(0, 8);
  genie::util::DataBlock lengths(0, 1);
  genie::util::DataBlock expected_values(0, 8);
  genie::util::DataBlock expected_lengths(0, 1);

  values = {static_cast<uint64_t>(-3438430427565543845LL),
            static_cast<uint64_t>(-3438430427565543845LL),
            8686590606261860295LL,
            810438489069303389LL,
            810438489069303389LL,
            810438489069303389LL,
            0};

  std::vector transform_subset = {values, lengths};
  const auto cfg = CreateConfig(std::numeric_limits<uint16_t>::max());

  // encode
  genie::entropy::gabac::TransformRleCoding(cfg, &transform_subset);
  lengths = transform_subset[0];
  values = transform_subset[1];

  expected_lengths = {1, 0, 2, 0};
  expected_values = {static_cast<uint64_t>(-3438430427565543845LL),
                     8686590606261860295LL, 810438489069303389LL, 0};

  EXPECT_EQ(lengths, expected_lengths);
  EXPECT_EQ(values, expected_values);

  // Decode
  genie::entropy::gabac::InverseTransformRleCoding(cfg, &transform_subset);
  values = transform_subset[0];
  expected_values = {static_cast<uint64_t>(-3438430427565543845LL),
                     static_cast<uint64_t>(-3438430427565543845LL),
                     8686590606261860295LL,
                     810438489069303389LL,
                     810438489069303389LL,
                     810438489069303389LL,
                     0};

  EXPECT_EQ(values, expected_values);
}

TEST(RleCodingTest, roundTripCoding) {
  genie::util::DataBlock values(0, 8);
  const genie::util::DataBlock lengths(0, 1);

  values.Resize(1024 * 1024);
  gabac_tests::FillVectorRandomGeometric(&values);

  std::vector transform_subset = {values, lengths};
  const auto cfg = CreateConfig(2);

  // encode + Decode
  EXPECT_NO_THROW(
      genie::entropy::gabac::TransformRleCoding(cfg, &transform_subset));
  EXPECT_NO_THROW(
      genie::entropy::gabac::InverseTransformRleCoding(cfg, &transform_subset));

  EXPECT_EQ(values, transform_subset[0]);
}
