

#include "genie/format/sam/sam_to_mgrec/sam_sorter.h"

#include <gtest/gtest.h>

#include "genie/format/sam/sam_to_mgrec/sam_record.h"

TEST(SamSorterTest, easyExample) {
  auto sam_sorter =
      genie::format::sam::sam_to_mgrec::SamSorter(100);
  genie::format::sam::sam_to_mgrec::SamRecord record;
  std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;

  record.qname_ = "pair1";
  record.pos_ = 1;
  record.mate_pos_ = 5;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair1";
  record.pos_ = 5;
  record.mate_pos_ = 1;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  sam_sorter.EndFile();

  std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;
  result.push_back(records);
  EXPECT_EQ(sam_sorter.GetQueries(), result);
}

TEST(SamSorterTest, complexExample) {
  genie::format::sam::sam_to_mgrec::SamSorter sam_sorter(100);
  genie::format::sam::sam_to_mgrec::SamRecord record;
  std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;
  std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;

  record.qname_ = "pair1";
  record.pos_ = 0;
  record.mate_pos_ = 0;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair2";
  record.pos_ = 1;
  record.mate_pos_ = 99;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair3";
  record.pos_ = 5;
  record.mate_pos_ = 201;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair4";
  record.pos_ = 13;
  record.mate_pos_ = 13;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair4";
  record.pos_ = 13;
  record.mate_pos_ = 13;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair5";
  record.pos_ = 25;
  record.mate_pos_ = 67;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair6";
  record.pos_ = 39;
  record.mate_pos_ = 67;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair6";
  record.pos_ = 67;
  record.mate_pos_ = 39;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair2";
  record.pos_ = 99;
  record.mate_pos_ = 1;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair3";
  record.pos_ = 201;
  record.mate_pos_ = 5;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair7";
  record.pos_ = std::numeric_limits<uint32_t>::max();
  record.mate_pos_ = std::numeric_limits<uint32_t>::max();
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  sam_sorter.EndFile();

  std::vector<genie::format::sam::sam_to_mgrec::SamRecord> tmp;

  tmp.push_back(records.at(0));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(1));
  tmp.push_back(records.at(8));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(2));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(3));
  tmp.push_back(records.at(4));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(5));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(6));
  tmp.push_back(records.at(7));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(9));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(10));
  result.push_back(tmp);
  tmp.clear();

  EXPECT_EQ(sam_sorter.GetQueries(), result);
}

TEST(SamSorterTest, waitingDistance) {
  genie::format::sam::sam_to_mgrec::SamSorter sam_sorter(100);
  genie::format::sam::sam_to_mgrec::SamRecord record;
  std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;
  std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;

  record.qname_ = "pair1";
  record.pos_ = 1;
  record.mate_pos_ = 123;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair2";
  record.pos_ = 4;
  record.mate_pos_ = 105;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair3";
  record.pos_ = 56;
  record.mate_pos_ = 57;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair3";
  record.pos_ = 57;
  record.mate_pos_ = 56;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair4";
  record.pos_ = 93;
  record.mate_pos_ = 145;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair5";
  record.pos_ = 102;
  record.mate_pos_ = 256;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair2";
  record.pos_ = 105;
  record.mate_pos_ = 4;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair1";
  record.pos_ = 123;
  record.mate_pos_ = 1;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair4";
  record.pos_ = 145;
  record.mate_pos_ = 93;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair5";
  record.pos_ = 256;
  record.mate_pos_ = 102;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  sam_sorter.EndFile();

  std::vector<genie::format::sam::sam_to_mgrec::SamRecord> tmp;

  tmp.push_back(records.at(0));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(1));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(2));
  tmp.push_back(records.at(3));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(4));
  tmp.push_back(records.at(8));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(5));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(6));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(7));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(9));
  result.push_back(tmp);
  tmp.clear();

  EXPECT_EQ(sam_sorter.GetQueries(), result);
}

TEST(SamSorterTest, unalignedPairWithZero) {
  genie::format::sam::sam_to_mgrec::SamSorter sam_sorter(100);
  genie::format::sam::sam_to_mgrec::SamRecord record;
  std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;
  std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;

  record.qname_ = "pair1";
  record.pos_ = 0;
  record.mate_pos_ = 0;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair1";
  record.pos_ = 0;
  record.mate_pos_ = 0;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair2";
  record.pos_ = 0;
  record.mate_pos_ = 0;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair2";
  record.pos_ = 0;
  record.mate_pos_ = 0;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair3";
  record.pos_ = 0;
  record.mate_pos_ = 0;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair3";
  record.pos_ = 0;
  record.mate_pos_ = 0;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair4";
  record.pos_ = 0;
  record.mate_pos_ = 0;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair4";
  record.pos_ = 0;
  record.mate_pos_ = 0;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair5";
  record.pos_ = 0;
  record.mate_pos_ = 0;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair5";
  record.pos_ = 0;
  record.mate_pos_ = 0;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  sam_sorter.EndFile();

  std::vector<genie::format::sam::sam_to_mgrec::SamRecord> tmp;

  tmp.push_back(records.at(0));
  tmp.push_back(records.at(1));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(2));
  tmp.push_back(records.at(3));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(4));
  tmp.push_back(records.at(5));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(6));
  tmp.push_back(records.at(7));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(8));
  tmp.push_back(records.at(9));
  result.push_back(tmp);
  tmp.clear();

  EXPECT_EQ(sam_sorter.GetQueries(), result);
}

TEST(SamSorterTest, unalignedPairWithMaxValue) {
  genie::format::sam::sam_to_mgrec::SamSorter sam_sorter(100);
  genie::format::sam::sam_to_mgrec::SamRecord record;
  std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;
  std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;

  record.qname_ = "pair1";
  record.pos_ = std::numeric_limits<uint32_t>::max();
  record.mate_pos_ = std::numeric_limits<uint32_t>::max();
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair1";
  record.pos_ = std::numeric_limits<uint32_t>::max();
  record.mate_pos_ = std::numeric_limits<uint32_t>::max();
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair2";
  record.pos_ = std::numeric_limits<uint32_t>::max();
  record.mate_pos_ = std::numeric_limits<uint32_t>::max();
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair2";
  record.pos_ = std::numeric_limits<uint32_t>::max();
  record.mate_pos_ = std::numeric_limits<uint32_t>::max();
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair3";
  record.pos_ = std::numeric_limits<uint32_t>::max();
  record.mate_pos_ = std::numeric_limits<uint32_t>::max();
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair3";
  record.pos_ = std::numeric_limits<uint32_t>::max();
  record.mate_pos_ = std::numeric_limits<uint32_t>::max();
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair4";
  record.pos_ = std::numeric_limits<uint32_t>::max();
  record.mate_pos_ = std::numeric_limits<uint32_t>::max();
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair4";
  record.pos_ = std::numeric_limits<uint32_t>::max();
  record.mate_pos_ = std::numeric_limits<uint32_t>::max();
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair5";
  record.pos_ = std::numeric_limits<uint32_t>::max();
  record.mate_pos_ = std::numeric_limits<uint32_t>::max();
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair5";
  record.pos_ = std::numeric_limits<uint32_t>::max();
  record.mate_pos_ = std::numeric_limits<uint32_t>::max();
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  sam_sorter.EndFile();

  std::vector<genie::format::sam::sam_to_mgrec::SamRecord> tmp;

  tmp.push_back(records.at(0));
  tmp.push_back(records.at(1));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(2));
  tmp.push_back(records.at(3));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(4));
  tmp.push_back(records.at(5));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(6));
  tmp.push_back(records.at(7));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(8));
  tmp.push_back(records.at(9));
  result.push_back(tmp);
  tmp.clear();

  EXPECT_EQ(sam_sorter.GetQueries(), result);
}

TEST(SamSorterTest, alignedPair) {
  genie::format::sam::sam_to_mgrec::SamSorter sam_sorter(100);
  genie::format::sam::sam_to_mgrec::SamRecord record;
  std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;
  std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;

  record.qname_ = "pair1";
  record.pos_ = 1;
  record.mate_pos_ = 66;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair2";
  record.pos_ = 13;
  record.mate_pos_ = 51;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair3";
  record.pos_ = 26;
  record.mate_pos_ = 82;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair2";
  record.pos_ = 51;
  record.mate_pos_ = 13;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair1";
  record.pos_ = 66;
  record.mate_pos_ = 1;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair4";
  record.pos_ = 79;
  record.mate_pos_ = 125;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair3";
  record.pos_ = 82;
  record.mate_pos_ = 26;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair5";
  record.pos_ = 111;
  record.mate_pos_ = 129;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair4";
  record.pos_ = 125;
  record.mate_pos_ = 79;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair5";
  record.pos_ = 129;
  record.mate_pos_ = 111;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  sam_sorter.EndFile();

  std::vector<genie::format::sam::sam_to_mgrec::SamRecord> tmp;

  tmp.push_back(records.at(0));
  tmp.push_back(records.at(4));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(1));
  tmp.push_back(records.at(3));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(2));
  tmp.push_back(records.at(6));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(5));
  tmp.push_back(records.at(8));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(7));
  tmp.push_back(records.at(9));
  result.push_back(tmp);
  tmp.clear();

  EXPECT_EQ(sam_sorter.GetQueries(), result);
}

TEST(SamSorterTest, singleRecordWithoutMate) {
  genie::format::sam::sam_to_mgrec::SamSorter sam_sorter(100);
  genie::format::sam::sam_to_mgrec::SamRecord record;
  std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;
  std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;

  record.qname_ = "pair1";
  record.pos_ = 1;
  record.mate_pos_ = 123;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair2";
  record.pos_ = 23;
  record.mate_pos_ = 89;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair3";
  record.pos_ = 56;
  record.mate_pos_ = 57;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair4";
  record.pos_ = 67;
  record.mate_pos_ = 25;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair5";
  record.pos_ = 81;
  record.mate_pos_ = 504;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair6";
  record.pos_ = 93;
  record.mate_pos_ = 145;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair7";
  record.pos_ = 102;
  record.mate_pos_ = 156;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair8";
  record.pos_ = 124;
  record.mate_pos_ = 2;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair6";
  record.pos_ = 145;
  record.mate_pos_ = 93;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair7";
  record.pos_ = 156;
  record.mate_pos_ = 102;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  sam_sorter.EndFile();

  std::vector<genie::format::sam::sam_to_mgrec::SamRecord> tmp;

  tmp.push_back(records.at(0));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(1));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(2));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(3));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(4));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(5));
  tmp.push_back(records.at(8));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(6));
  tmp.push_back(records.at(9));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(7));
  result.push_back(tmp);
  tmp.clear();

  EXPECT_EQ(sam_sorter.GetQueries(), result);
}

TEST(SamSorterTest, samePositionAsMate) {
  genie::format::sam::sam_to_mgrec::SamSorter sam_sorter(100);
  genie::format::sam::sam_to_mgrec::SamRecord record;
  std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;
  std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;

  record.qname_ = "pair1";
  record.pos_ = 1;
  record.mate_pos_ = 1;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair1";
  record.pos_ = 1;
  record.mate_pos_ = 1;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair2";
  record.pos_ = 56;
  record.mate_pos_ = 56;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair2";
  record.pos_ = 56;
  record.mate_pos_ = 56;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair3";
  record.pos_ = 89;
  record.mate_pos_ = 89;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair3";
  record.pos_ = 89;
  record.mate_pos_ = 89;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair4";
  record.pos_ = 102;
  record.mate_pos_ = 156;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair4";
  record.pos_ = 156;
  record.mate_pos_ = 102;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair5";
  record.pos_ = 178;
  record.mate_pos_ = 178;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair5";
  record.pos_ = 178;
  record.mate_pos_ = 178;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  sam_sorter.EndFile();

  std::vector<genie::format::sam::sam_to_mgrec::SamRecord> tmp;

  tmp.push_back(records.at(0));
  tmp.push_back(records.at(1));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(2));
  tmp.push_back(records.at(3));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(4));
  tmp.push_back(records.at(5));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(6));
  tmp.push_back(records.at(7));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(8));
  tmp.push_back(records.at(9));
  result.push_back(tmp);
  tmp.clear();

  EXPECT_EQ(sam_sorter.GetQueries(), result);
}

TEST(SamSorterTest, positionMultipleTimes) {
  genie::format::sam::sam_to_mgrec::SamSorter sam_sorter(100);
  genie::format::sam::sam_to_mgrec::SamRecord record;
  std::vector<genie::format::sam::sam_to_mgrec::SamRecord> records;
  std::vector<std::vector<genie::format::sam::sam_to_mgrec::SamRecord>> result;

  record.qname_ = "pair1";
  record.pos_ = 1;
  record.mate_pos_ = 23;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair2";
  record.pos_ = 23;
  record.mate_pos_ = 89;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair1";
  record.pos_ = 23;
  record.mate_pos_ = 1;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair3";
  record.pos_ = 57;
  record.mate_pos_ = 145;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair2";
  record.pos_ = 89;
  record.mate_pos_ = 23;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair4";
  record.pos_ = 93;
  record.mate_pos_ = 145;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair5";
  record.pos_ = 102;
  record.mate_pos_ = 156;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair4";
  record.pos_ = 145;
  record.mate_pos_ = 93;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair3";
  record.pos_ = 145;
  record.mate_pos_ = 57;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  record.qname_ = "pair5";
  record.pos_ = 156;
  record.mate_pos_ = 102;
  sam_sorter.AddSamRead(record);
  records.push_back(record);

  sam_sorter.EndFile();

  std::vector<genie::format::sam::sam_to_mgrec::SamRecord> tmp;

  tmp.push_back(records.at(0));
  tmp.push_back(records.at(2));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(1));
  tmp.push_back(records.at(4));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(3));
  tmp.push_back(records.at(8));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(5));
  tmp.push_back(records.at(7));
  result.push_back(tmp);
  tmp.clear();

  tmp.push_back(records.at(6));
  tmp.push_back(records.at(9));
  result.push_back(tmp);
  tmp.clear();

  EXPECT_EQ(sam_sorter.GetQueries(), result);
}