#include <gtest/gtest.h>

#include <vector>

#include "common.h"
#include "genie/entropy/gabac/equality_sub_seq_transform.h"
#include "genie/util/data_block.h"

TEST(EqualityCodingTest, voidInput) {
  genie::util::DataBlock values(0, 8);
  genie::util::DataBlock flags(0, 1);
  std::vector transform_subset = {values, flags};

  // encoding
  EXPECT_NO_THROW(
      genie::entropy::gabac::TransformEqualityCoding(&transform_subset));
  flags = transform_subset[0];
  values = transform_subset[1];
  EXPECT_EQ(flags.Size(), 0);
  EXPECT_EQ(values.Size(), 0);

  // decoding
  EXPECT_NO_THROW(
      genie::entropy::gabac::InverseTransformEqualityCoding(&transform_subset));
  values = transform_subset[0];
  EXPECT_EQ(values.Size(), 0);
}

TEST(EqualityCodingTest, singlePositiveValue) {
  genie::util::DataBlock values(0, 8);
  genie::util::DataBlock flags(0, 1);
  values = {42};
  std::vector transform_subset = {values, flags};

  // encoding
  EXPECT_NO_THROW(
      genie::entropy::gabac::TransformEqualityCoding(&transform_subset));
  flags = transform_subset[0];
  values = transform_subset[1];
  EXPECT_EQ(flags.Size(), 1);
  EXPECT_EQ(flags.Get(0), 0);
  EXPECT_EQ(values.Size(), 1);
  EXPECT_EQ(values.Get(0), 41);

  // decoding
  EXPECT_NO_THROW(
      genie::entropy::gabac::InverseTransformEqualityCoding(&transform_subset));
  values = transform_subset[0];
  EXPECT_EQ(values.Size(), 1);
  EXPECT_EQ(values.Get(0), 42);
}

TEST(EqualityCodingTest, semiRandom) {
  genie::util::DataBlock flags(0, 1);
  genie::util::DataBlock values(0, 8);
  values = {static_cast<uint64_t>(-3438430427565543845LL),
            static_cast<uint64_t>(-3438430427565543845LL),
            8686590606261860295LL,
            810438489069303389LL,
            810438489069303389LL,
            810438489069303389LL,
            0};

  std::vector transform_subset = {values, flags};

  genie::util::DataBlock expected_output(0, 8);
  expected_output = {static_cast<uint64_t>(-3438430427565543845LL) - 1,
                     8686590606261860295LL, 810438489069303389LL, 0};
  genie::util::DataBlock expected_flags(0, 1);
  expected_flags = {0, 1, 0, 0, 1, 1, 0};

  // encoding
  EXPECT_NO_THROW(
      genie::entropy::gabac::TransformEqualityCoding(&transform_subset));
  auto res_flags = transform_subset[0];
  auto res_values = transform_subset[1];
  EXPECT_EQ(res_flags, expected_flags);
  EXPECT_EQ(res_values, expected_output);

  // decoding
  EXPECT_NO_THROW(
      genie::entropy::gabac::InverseTransformEqualityCoding(&transform_subset));
  res_values = transform_subset[0];
  EXPECT_EQ(res_values, values);
}

TEST(EqualityCodingTest, roundTripCoding) {
  genie::util::DataBlock values(0, 8);
  const genie::util::DataBlock flags(0, 1);

  // A lot of input data - WordSize
  values.Resize(1024 * 1024);
  gabac_tests::FillVectorRandomGeometric(&values);

  std::vector transform_subset = {values, flags};
  // auto subsetCopy = transformSubset;
  EXPECT_NO_THROW(
      genie::entropy::gabac::TransformEqualityCoding(&transform_subset));

  // run another version to test against
  // EXPECT_NO_THROW(equalityEncodeTest(subsetCopy));
  // EXPECT_EQ(subsetCopy, transformSubset);

  EXPECT_NO_THROW(
      genie::entropy::gabac::InverseTransformEqualityCoding(&transform_subset));
  const auto result = transform_subset[0];
  EXPECT_EQ(values.Size(), result.Size());
  EXPECT_EQ(values, result);
}