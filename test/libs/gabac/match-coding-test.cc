#include <genie/entropy/gabac/match_sub_seq_transform.h>
#include <genie/util/data_block.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <vector>

#include "common.h"

class MatchCodingTest : public testing::Test {
 protected:
  const unsigned int large_test_size_ = 1 * 10 * 1024;
  std::vector<uint16_t> window_sizes_ = {1024};  // minimum value: 0,
  // which should not perform a search, maximum reasonable value: 32768

  genie::util::DataBlock symbols_;
  genie::util::DataBlock pointers_;
  genie::util::DataBlock expected_pointers_;
  genie::util::DataBlock lengths_;
  genie::util::DataBlock expected_lengths_;
  genie::util::DataBlock raw_values_;
  genie::util::DataBlock expected_raw_values_;
  std::vector<genie::util::DataBlock> transform_subset_;

  void SetUp() override {
    symbols_ = genie::util::DataBlock(0, 4);
    pointers_ = genie::util::DataBlock(0, 4);
    expected_pointers_ = genie::util::DataBlock(0, 4);
    lengths_ = genie::util::DataBlock(0, 4);
    expected_lengths_ = genie::util::DataBlock(0, 4);
    raw_values_ = genie::util::DataBlock(0, 8);
    expected_raw_values_ = genie::util::DataBlock(0, 8);
    transform_subset_ = std::vector<genie::util::DataBlock>();
  }

  static genie::entropy::paramcabac::Subsequence CreateConfig(
      const uint16_t match_buffer_size) {
    genie::entropy::paramcabac::TransformedParameters param(
        genie::entropy::paramcabac::TransformedParameters::TransformIdSubseq::
            MATCH_CODING,
        match_buffer_size);

    std::vector<genie::entropy::paramcabac::TransformedSubSeq> vec;
    genie::entropy::paramcabac::Subsequence cfg(std::move(param), 0, true,
                                                std::move(vec));

    return cfg;
  }
};

TEST_F(MatchCodingTest, voidInput) {
  for (const auto& window_size_it : window_sizes_) {
    auto cfg = CreateConfig(window_size_it);
    transform_subset_ = {raw_values_};

    // encode
    EXPECT_NO_THROW(
        genie::entropy::gabac::TransformMatchCoding(cfg, &transform_subset_));
    EXPECT_EQ(transform_subset_[0].Size(), 0);
    EXPECT_EQ(transform_subset_[1].Size(), 0);
    EXPECT_EQ(transform_subset_[2].Size(), 0);

    // Decode
    EXPECT_NO_THROW(
        genie::entropy::gabac::InverseTransformMatchCoding(&transform_subset_));
    EXPECT_EQ(transform_subset_[0].Size(), 0);
  }
}

TEST_F(MatchCodingTest, transformMatchCoding) {
  // windowSize == 0 - should return raw_values
  // note: windowSize 1 will also result in no match as our implementation
  // requires length to be >2, but according to the standard is allowed
  raw_values_ = {static_cast<uint64_t>(-1), 2, static_cast<uint64_t>(-3), 4, 4,
                 static_cast<uint64_t>(-3), 2, static_cast<uint64_t>(-1)};
  pointers_ = {};
  lengths_ = {};
  expected_raw_values_ = {
      static_cast<uint64_t>(-1), 2, static_cast<uint64_t>(-3), 4, 4,
      static_cast<uint64_t>(-3), 2, static_cast<uint64_t>(-1)};

  auto cfg = CreateConfig(1);
  transform_subset_ = {raw_values_};

  EXPECT_NO_THROW(
      genie::entropy::gabac::TransformMatchCoding(cfg, &transform_subset_));
  pointers_ = transform_subset_[0];
  lengths_ = transform_subset_[1];
  raw_values_ = transform_subset_[2];

  EXPECT_EQ(pointers_.Size(), 0);
  EXPECT_EQ(lengths_.Size(), 8);
  EXPECT_EQ(raw_values_.Size(), 8);
  EXPECT_EQ(raw_values_, expected_raw_values_);

  // Reset
  SetUp();
  // windowSize == 4
  raw_values_ = {static_cast<uint64_t>(-1), 2, static_cast<uint64_t>(-3), 4, 2,
                 static_cast<uint64_t>(-3), 4, static_cast<uint64_t>(-1)};
  pointers_ = {};
  lengths_ = {};

  // reduce wordSize to match returned DataBlocks
  expected_pointers_.SetWordSize(1);
  expected_lengths_.SetWordSize(1);
  expected_pointers_ = {3};
  expected_lengths_ = {0, 0, 0, 0, 3, 0};
  expected_raw_values_ = {static_cast<uint64_t>(-1), 2,
                          static_cast<uint64_t>(-3), 4,
                          static_cast<uint64_t>(-1)};

  cfg = CreateConfig(4);
  transform_subset_ = {raw_values_};

  EXPECT_NO_THROW(
      genie::entropy::gabac::TransformMatchCoding(cfg, &transform_subset_));
  pointers_ = transform_subset_[0];
  lengths_ = transform_subset_[1];
  raw_values_ = transform_subset_[2];

  EXPECT_EQ(pointers_.Size(), 1);
  EXPECT_EQ(pointers_, expected_pointers_);
  EXPECT_EQ(lengths_.Size(), 6);
  EXPECT_EQ(lengths_, expected_lengths_);
  EXPECT_EQ(raw_values_.Size(), 5);
  EXPECT_EQ(raw_values_, expected_raw_values_);
}

TEST_F(MatchCodingTest, inverseTransformMatchCoding) {
  // Void input shall lead to void output
  pointers_ = {};
  lengths_ = {};
  raw_values_ = {};
  transform_subset_ = {pointers_, lengths_, raw_values_};
  EXPECT_NO_THROW(
      genie::entropy::gabac::InverseTransformMatchCoding(&transform_subset_));
  EXPECT_EQ(transform_subset_[0].Size(), 0);

  // void rawValues (is not allowed) and lengths is not
  pointers_ = {};
  lengths_ = {0, 0, 0, 0};
  raw_values_ = {};
  transform_subset_ = {pointers_, lengths_, raw_values_};

  // void rawValues (is not allowed) and lengths is not
  EXPECT_DEATH(
      genie::entropy::gabac::InverseTransformMatchCoding(&transform_subset_),
      "");

  pointers_ = {0, 0, 0, 0};
  lengths_ = {};
  raw_values_ = {};
  transform_subset_ = {pointers_, lengths_, raw_values_};
  EXPECT_DEATH(
      genie::entropy::gabac::InverseTransformMatchCoding(&transform_subset_),
      "");

  // windowSize == 4
  expected_raw_values_ = {
      static_cast<uint64_t>(-1), 2, static_cast<uint64_t>(-3), 4, 2,
      static_cast<uint64_t>(-3), 4, static_cast<uint64_t>(-1)};
  pointers_ = {3};
  lengths_ = {0, 0, 0, 0, 3, 0};
  raw_values_ = {static_cast<uint64_t>(-1), 2, static_cast<uint64_t>(-3), 4,
                 static_cast<uint64_t>(-1)};
  transform_subset_ = {pointers_, lengths_, raw_values_};
  EXPECT_NO_THROW(
      genie::entropy::gabac::InverseTransformMatchCoding(&transform_subset_));
  EXPECT_EQ(transform_subset_[0].Size(), expected_raw_values_.Size());
  EXPECT_EQ(transform_subset_[0], expected_raw_values_);
}

TEST_F(MatchCodingTest, roundTripCoding) {
  // test large file size word Size 1
  symbols_.Resize(large_test_size_);
  gabac_tests::FillVectorRandomUniform(std::numeric_limits<uint8_t>::min(),
                                       std::numeric_limits<uint8_t>::max(),
                                       &symbols_);

  raw_values_ = symbols_;

  for (auto& window_size : window_sizes_) {
    auto cfg = CreateConfig(window_size);
    transform_subset_ = {raw_values_};
    EXPECT_NO_THROW(
        genie::entropy::gabac::TransformMatchCoding(cfg, &transform_subset_));
    EXPECT_NO_THROW(
        genie::entropy::gabac::InverseTransformMatchCoding(&transform_subset_));
    EXPECT_EQ(symbols_.Size(), transform_subset_[0].Size());
    EXPECT_EQ(symbols_, transform_subset_[0]);
  }

  // test large file size word Size 2
  symbols_.Resize(large_test_size_);
  gabac_tests::FillVectorRandomUniform(std::numeric_limits<uint16_t>::min(),
                                       std::numeric_limits<uint16_t>::max(),
                                       &symbols_);
  raw_values_ = symbols_;
  for (auto& window_size : window_sizes_) {
    auto cfg = CreateConfig(window_size);
    transform_subset_ = {raw_values_};
    EXPECT_NO_THROW(
        genie::entropy::gabac::TransformMatchCoding(cfg, &transform_subset_));
    EXPECT_NO_THROW(
        genie::entropy::gabac::InverseTransformMatchCoding(&transform_subset_));
    EXPECT_EQ(symbols_.Size(), transform_subset_[0].Size());
    EXPECT_EQ(symbols_, transform_subset_[0]);
  }

  // test large file size word Size 4
  symbols_.Resize(large_test_size_);
  gabac_tests::FillVectorRandomUniform(std::numeric_limits<uint32_t>::min(),
                                       std::numeric_limits<uint32_t>::max(),
                                       &symbols_);
  raw_values_ = symbols_;
  for (auto& window_size : window_sizes_) {
    auto cfg = CreateConfig(window_size);
    transform_subset_ = {raw_values_};
    EXPECT_NO_THROW(
        genie::entropy::gabac::TransformMatchCoding(cfg, &transform_subset_));
    EXPECT_NO_THROW(
        genie::entropy::gabac::InverseTransformMatchCoding(&transform_subset_));
    EXPECT_EQ(symbols_.Size(), transform_subset_[0].Size());
    EXPECT_EQ(symbols_, transform_subset_[0]);
  }
  symbols_.Clear();
}
