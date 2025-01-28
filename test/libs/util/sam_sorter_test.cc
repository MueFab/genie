#include "gtest/gtest.h"
#include "genie/format/sam/sam_to_mgrec/sam_sorter.h"
#include "genie/format/sam/sam_record.h"

namespace genie::format::sam::sam_to_mgrec {

// Test Fixture for SamSorter
class SamSorterFullTest : public ::testing::Test {
 protected:
  SamSorter sorter_{101};  // Initialize with max distance of 100
};

TEST_F(SamSorterFullTest, HandlePairedReads) {

  // Instant Match
  SamRecord read1;
  read1.flag_ = BAM_FPAIRED | BAM_FPROPER_PAIR | BAM_FREAD1;
  read1.qname_ = "pair1";
  read1.pos_ = 100;
  read1.mate_pos_ = 200;
  read1.rid_ = 1;
  read1.mate_rid_ = 1;

  SamRecord read2;
  read2.flag_ = BAM_FPAIRED | BAM_FPROPER_PAIR | BAM_FREAD2;
  read2.qname_ = "pair1";
  read2.pos_ = 200;
  read2.mate_pos_ = 100;
  read2.rid_ = 1;
  read2.mate_rid_ = 1;

  // Pair blocked by a third read
  SamRecord read3;
  read3.flag_ = BAM_FPAIRED | BAM_FPROPER_PAIR | BAM_FREAD1;
  read3.qname_ = "pair2";
  read3.pos_ = 201;
  read3.mate_pos_ = 205;
  read3.rid_ = 1;
  read3.mate_rid_ = 1;

  SamRecord read4;
  read4.flag_ = BAM_FPAIRED | BAM_FPROPER_PAIR | BAM_FREAD1;
  read4.qname_ = "pair3";
  read4.pos_ = 202;
  read4.mate_pos_ = 210;
  read4.rid_ = 1;
  read4.mate_rid_ = 1;

  SamRecord read5;
  read5.flag_ = BAM_FPAIRED | BAM_FPROPER_PAIR | BAM_FREAD2;
  read5.qname_ = "pair2";
  read5.pos_ = 205;
  read5.mate_pos_ = 201;
  read5.rid_ = 1;
  read5.mate_rid_ = 1;

  sorter_.AddSamRead(read1);
  sorter_.AddSamRead(read2);
  sorter_.AddSamRead(read3);
  sorter_.AddSamRead(read4);
  sorter_.AddSamRead(read5);

  // Only first pair should be matched, second pair is blocked by read4
  auto pairs = sorter_.GetPairs();

  ASSERT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first.qname_, "pair1");
  EXPECT_EQ(pairs[0].second->qname_, "pair1");

  SamRecord read6;
  read6.flag_ = BAM_FPAIRED | BAM_FPROPER_PAIR | BAM_FREAD2;
  read6.qname_ = "pair3";
  read6.pos_ = 210;
  read6.mate_pos_ = 202;
  read6.rid_ = 1;
  read6.mate_rid_ = 1;

  sorter_.AddSamRead(read6);

  // Now all pairs should be matched
  pairs = sorter_.GetPairs();

  ASSERT_EQ(pairs.size(), 2);
  EXPECT_EQ(pairs[0].first.qname_, "pair2");
  EXPECT_EQ(pairs[0].second->qname_, "pair2");
  EXPECT_EQ(pairs[1].first.qname_, "pair3");
  EXPECT_EQ(pairs[1].second->qname_, "pair3");

  // Instant orphan
  SamRecord read7;
  read7.flag_ = BAM_FPAIRED | BAM_FPROPER_PAIR | BAM_FREAD2;
  read7.qname_ = "pair4";
  read7.pos_ = 211;
  read7.mate_pos_ = 202;
  read7.rid_ = 1;
  read7.mate_rid_ = 1;

  sorter_.AddSamRead(read7);

  // Now all pairs should be matched
  pairs = sorter_.GetPairs();

  ASSERT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first.qname_, "pair4");
  EXPECT_EQ(pairs[0].second, std::nullopt);

  // Retained orphan
  SamRecord read8;
  read8.flag_ = BAM_FPAIRED | BAM_FPROPER_PAIR | BAM_FREAD1;
  read8.qname_ = "pair5";
  read8.pos_ = 211;
  read8.mate_pos_ = 220;
  read8.rid_ = 1;
  read8.mate_rid_ = 1;

  sorter_.AddSamRead(read8);

  // No Match
  pairs = sorter_.GetPairs();

  ASSERT_EQ(pairs.size(), 0);

  sorter_.Finish();
  pairs = sorter_.GetPairs();
  ASSERT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first.qname_, "pair5");
  EXPECT_EQ(pairs[0].second, std::nullopt);
}

TEST_F(SamSorterFullTest, HandleDistantMates) {
  SamRecord read1;
  read1.qname_ = "distant_pair";
  read1.pos_ = 100;
  read1.mate_pos_ = 500;
  read1.rid_ = 1;
  read1.mate_rid_ = 1;
  read1.flag_ = BAM_FPAIRED | BAM_FPROPER_PAIR | BAM_FREAD1;

  SamRecord read2;
  read2.qname_ = "distant_pair";
  read2.pos_ = 500;
  read2.mate_pos_ = 100;
  read2.rid_ = 1;
  read2.mate_rid_ = 1;
  read2.flag_ = BAM_FPAIRED | BAM_FPROPER_PAIR | BAM_FREAD2;

  sorter_.AddSamRead(read1);
  sorter_.AddSamRead(read2);

  auto pairs = sorter_.GetPairs();

  ASSERT_EQ(pairs.size(), 2);
  EXPECT_EQ(pairs[0].first.qname_, "distant_pair");
  EXPECT_FALSE(pairs[0].second.has_value());
  EXPECT_EQ(pairs[1].first.qname_, "distant_pair");
  EXPECT_FALSE(pairs[1].second.has_value());
}

TEST_F(SamSorterFullTest, HandleUnmappedReadPairs) {
  SamRecord read1;
  read1.qname_ = "pair1";
  read1.pos_ = 0;
  read1.mate_pos_ = 0;
  read1.rid_ = 0;
  read1.mate_rid_ = 0;
  read1.flag_ = BAM_FPAIRED | BAM_FUNMAP | BAM_FMUNMAP | BAM_FREAD1;

  SamRecord read2;
  read2.qname_ = "pair1";
  read2.pos_ = 0;
  read2.mate_pos_ = 0;
  read2.rid_ = 0;
  read2.mate_rid_ = 0;
  read2.flag_ = BAM_FPAIRED | BAM_FUNMAP | BAM_FMUNMAP | BAM_FREAD2;

  sorter_.AddSamRead(read1);
  sorter_.AddSamRead(read2);

  auto pairs = sorter_.GetPairs();

  ASSERT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first.qname_, "pair1");
  EXPECT_EQ(pairs[0].second->qname_, "pair1");
}

TEST_F(SamSorterFullTest, HandleHalfMappedPairs) {
  SamRecord read1;
  read1.qname_ = "pair1";
  read1.pos_ = 100;
  read1.mate_pos_ = 100;
  read1.rid_ = 1;
  read1.mate_rid_ = 1;
  read1.flag_ = BAM_FPAIRED | BAM_FUNMAP | BAM_FREAD2;

  SamRecord read2;
  read2.qname_ = "pair1";
  read2.pos_ = 100;
  read2.mate_pos_ = 100;
  read2.rid_ = 1;
  read2.mate_rid_ = 1;
  read2.flag_ = BAM_FPAIRED | BAM_FMUNMAP | BAM_FREAD1;

  sorter_.AddSamRead(read1);
  sorter_.AddSamRead(read2);

  const auto pairs = sorter_.GetPairs();

  ASSERT_EQ(pairs.size(), 1);
  EXPECT_EQ(pairs[0].first.qname_, "pair1");
  EXPECT_EQ(pairs[0].second->qname_, "pair1");
}

}  // namespace genie::format::sam::sam_to_mgrec
