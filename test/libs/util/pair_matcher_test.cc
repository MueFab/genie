#include "../../../src/genie/format/sam/pair_matcher.h"

#include "genie/format/sam/sam_record.h"
#include "gtest/gtest.h"

namespace genie::format::sam::sam_to_mgrec {

// Test Fixture for PairMatcher
class PairMatcherTest : public testing::Test {
 protected:
  PairMatcher matcher_;
};

TEST_F(PairMatcherTest, AddUnmatchedReadAndRetrieveLowestPosition) {
  SamRecord record1;
  record1.qname_ = "read1";
  record1.pos_ = 10;

  SamRecord record2;
  record2.qname_ = "read2";
  record2.pos_ = 20;

  matcher_.AddUnmatchedRead(record1);
  matcher_.AddUnmatchedRead(record2);

  EXPECT_EQ(matcher_.GetLowestUnmatchedPosition(), 10);
}

TEST_F(PairMatcherTest, AbandonReadsBeforePosition) {
  SamRecord record1;
  record1.qname_ = "read1";
  record1.pos_ = 10;
  record1.mate_pos_ = 11;

  SamRecord record2;
  record2.qname_ = "read2";
  record2.pos_ = 20;
  record2.mate_pos_ = 21;

  SamRecord record3;
  record3.qname_ = "read3";
  record3.pos_ = 30;
  record3.mate_pos_ = 31;

  matcher_.AddUnmatchedRead(record1);
  matcher_.AddUnmatchedRead(record2);
  matcher_.AddUnmatchedRead(record3);

  auto abandoned = matcher_.Abandon(25);

  ASSERT_EQ(abandoned.size(), 2);
  EXPECT_EQ(abandoned[0].qname_, "read1");
  EXPECT_EQ(abandoned[1].qname_, "read2");
}

TEST_F(PairMatcherTest, MatchReads) {
  SamRecord record1;
  record1.qname_ = "read1";
  record1.pos_ = 10;
  record1.mate_pos_ = 15;

  SamRecord record2;
  record2.qname_ = "read1";
  record2.pos_ = 15;
  record2.mate_pos_ = 10;

  matcher_.AddUnmatchedRead(record1);

  auto mate = matcher_.Match(record2);

  ASSERT_TRUE(mate.has_value());
  EXPECT_EQ(mate->qname_, "read1");
  EXPECT_EQ(mate->pos_, 10);
}

TEST_F(PairMatcherTest, MatchNoMate) {
  SamRecord record1;
  record1.qname_ = "read1";
  record1.pos_ = 10;
  record1.mate_pos_ = 15;

  SamRecord record2;
  record2.qname_ = "read2";
  record2.pos_ = 15;
  record2.mate_pos_ = 10;

  matcher_.AddUnmatchedRead(record1);

  auto mate = matcher_.Match(record2);

  EXPECT_FALSE(mate.has_value());
}

TEST_F(PairMatcherTest, AbandonAndRetrieveLowestPosition) {
  SamRecord record1;
  record1.qname_ = "read1";
  record1.pos_ = 10;
  record1.mate_pos_ = 11;

  SamRecord record2;
  record2.qname_ = "read2";
  record2.pos_ = 20;
  record2.mate_pos_ = 21;

  matcher_.AddUnmatchedRead(record1);
  matcher_.AddUnmatchedRead(record2);

  auto abandoned = matcher_.Abandon(15);

  EXPECT_EQ(abandoned.size(), 1);
  EXPECT_EQ(abandoned[0].qname_, "read1");
  EXPECT_EQ(abandoned[0].pos_, 10);
  EXPECT_EQ(matcher_.GetLowestUnmatchedPosition(), 20);
}

}  // namespace genie::format::sam::sam_to_mgrec
