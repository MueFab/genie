#include "genie/format/sam/sam_to_mgrec/sam_sorter.h"

#include "genie/format/sam/sam_record.h"
#include "gtest/gtest.h"

namespace genie::format::sam::sam_to_mgrec {

// Test Fixture for PairQueue
class PairQueueTest : public testing::Test {
 protected:
  PairQueue queue_;
};

TEST_F(PairQueueTest, AddAndRetrieve) {
  SamRecord record1;
  record1.qname_ = "read1";
  record1.mate_pos_ = 10;

  SamRecord record2;
  record2.qname_ = "read2";
  record2.mate_pos_ = 20;

  SamRecordPair pair1(record1, std::nullopt);
  SamRecordPair pair2(record2, std::nullopt);

  queue_.Add(pair1);
  queue_.Add(pair2);

  auto completed = queue_.CompleteUntil(CmpPairMatePosLess(15));

  ASSERT_EQ(completed.size(), 1);
  EXPECT_EQ(completed[0].first.mate_pos_, 10);
}

TEST_F(PairQueueTest, CompleteUntilEmptyQueue) {
  const auto completed = queue_.CompleteUntil(CmpPairMatePosLess(10));
  EXPECT_TRUE(completed.empty());
}

TEST_F(PairQueueTest, CompleteUntilMultiplePairs) {
  SamRecord record1;
  record1.qname_ = "read1";
  record1.mate_pos_ = 5;

  SamRecord record2;
  record2.qname_ = "read2";
  record2.mate_pos_ = 15;

  SamRecord record3;
  record3.qname_ = "read3";
  record3.mate_pos_ = 25;

  SamRecordPair pair1(record1, std::nullopt);
  SamRecordPair pair2(record2, std::nullopt);
  SamRecordPair pair3(record3, std::nullopt);

  queue_.Add(pair1);
  queue_.Add(pair2);
  queue_.Add(pair3);

  auto completed = queue_.CompleteUntil(CmpPairMatePosLess(20));

  ASSERT_EQ(completed.size(), 2);
  EXPECT_EQ(completed[0].first.mate_pos_, 5);
  EXPECT_EQ(completed[1].first.mate_pos_, 15);
}

TEST_F(PairQueueTest, CompleteUntilNoMatch) {
  SamRecord record1;
  record1.qname_ = "read1";
  record1.mate_pos_ = 50;

  SamRecord record2;
  record2.qname_ = "read2";
  record2.mate_pos_ = 60;

  SamRecordPair pair1(record1, std::nullopt);
  SamRecordPair pair2(record2, std::nullopt);

  queue_.Add(pair1);
  queue_.Add(pair2);

  auto completed = queue_.CompleteUntil(CmpPairMatePosLess(30));

  EXPECT_TRUE(completed.empty());
}

TEST_F(PairQueueTest, PriorityQueueOrder) {
  SamRecord record1;
  record1.qname_ = "read1";
  record1.mate_pos_ = 30;

  SamRecord record2;
  record2.qname_ = "read2";
  record2.mate_pos_ = 10;

  SamRecord record3;
  record3.qname_ = "read3";
  record3.mate_pos_ = 20;

  SamRecordPair pair1(record1, std::nullopt);
  SamRecordPair pair2(record2, std::nullopt);
  SamRecordPair pair3(record3, std::nullopt);

  queue_.Add(pair1);
  queue_.Add(pair2);
  queue_.Add(pair3);

  auto completed = queue_.CompleteUntil(CmpPairMatePosLess(35));

  ASSERT_EQ(completed.size(), 3);
  EXPECT_EQ(completed[0].first.mate_pos_, 10);
  EXPECT_EQ(completed[1].first.mate_pos_, 20);
  EXPECT_EQ(completed[2].first.mate_pos_, 30);
}

TEST_F(PairQueueTest, AddWithTwoReads) {
  SamRecord record1;
  record1.qname_ = "read1";
  record1.mate_pos_ = 10;

  SamRecord record2;
  record2.qname_ = "read2";
  record2.mate_pos_ = 15;

  SamRecord record3;
  record3.qname_ = "read3";
  record3.mate_pos_ = 20;

  SamRecord record4;
  record4.qname_ = "read4";
  record4.mate_pos_ = 25;

  SamRecord record5;
  record5.qname_ = "read5";
  record5.mate_pos_ = 19;

  SamRecord record6;
  record6.qname_ = "read6";
  record6.mate_pos_ = 35;

  SamRecord record7;
  record7.qname_ = "read7";
  record7.mate_pos_ = 31;

  SamRecord record8;
  record8.qname_ = "read8";
  record8.mate_pos_ = 35;

  SamRecordPair pair1(record1, record2);
  SamRecordPair pair2(record3, record4);
  SamRecordPair pair3(record5, record6);
  SamRecordPair pair4(record7, record8);

  queue_.Add(pair1);
  queue_.Add(pair2);
  queue_.Add(pair3);
  queue_.Add(pair4);

  auto completed = queue_.CompleteUntil(CmpPairMatePosLess(30));

  ASSERT_EQ(completed.size(), 3);
  EXPECT_EQ(completed[0].first.qname_, "read1");
  EXPECT_EQ(completed[0].second->qname_, "read2");
  EXPECT_EQ(completed[1].first.qname_, "read5");
  EXPECT_EQ(completed[1].second->qname_, "read6");
  EXPECT_EQ(completed[2].first.qname_, "read3");
  EXPECT_EQ(completed[2].second->qname_, "read4");
}

}  // namespace genie::format::sam::sam_to_mgrec
