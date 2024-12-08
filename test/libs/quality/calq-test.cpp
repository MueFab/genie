#include <gtest/gtest.h>

#include <iostream>

#include "genie/quality/calq/calq_coder.h"
#include "genie/quality/calq/record_pileup.h"

TEST(CalqTest, recordPileupBasic) {
  auto pileup = genie::quality::calq::RecordPileup();

  // usually done in addRead, but i dont want to craft records
  // first "record"

  genie::quality::calq::EncodingRecord record;

  // first record
  record.sequences = {"AAAA", "GGGG"};
  record.quality_values = {"aaaa", "gggg"};
  record.cigars = {"4=", "4="};
  record.positions = {0, 10};

  pileup.AddRecord(record);

  // second record
  record.sequences = {"CCCC", "TTTTTTTT"};
  record.quality_values = {"cccc", "tttttttt"};
  record.cigars = {"4=", "8="};
  record.positions = {2, 8};

  pileup.AddRecord(record);

  // third record
  record.sequences = {"AAAA", "CCCC"};
  record.quality_values = {"aaaa", "cccc"};
  record.cigars = {"4=", "4="};
  record.positions = {10, 11};

  pileup.AddRecord(record);

  // test results
  std::string seq, qual;

  std::tie(seq, qual) = pileup.GetPileup(0);
  EXPECT_EQ(seq, "A");
  ASSERT_EQ(qual, "a");

  std::tie(seq, qual) = pileup.GetPileup(2);
  EXPECT_EQ(seq, "AC");
  EXPECT_EQ(qual, "ac");

  std::tie(seq, qual) = pileup.GetPileup(4);
  EXPECT_EQ(seq, "C");
  EXPECT_EQ(qual, "c");

  std::tie(seq, qual) = pileup.GetPileup(6);
  EXPECT_EQ(seq, "");
  EXPECT_EQ(qual, "");

  std::tie(seq, qual) = pileup.GetPileup(11);
  EXPECT_EQ(seq, "GTAC");
  EXPECT_EQ(qual, "gtac");

  std::tie(seq, qual) = pileup.GetPileup(15);
  EXPECT_EQ(seq, "T");
  EXPECT_EQ(qual, "t");

  EXPECT_ANY_THROW(pileup.GetPileup(16));
}
//
TEST(CalqTest, recordPileupInsertions) {
  auto pileup = genie::quality::calq::RecordPileup();

  genie::quality::calq::EncodingRecord record;

  // first record
  record.sequences = {"GGAAAA", "AGGAAA"};
  record.quality_values = {"ggaaaa", "aggaaa"};
  record.cigars = {"2+4=", "1=2+3="};
  record.positions = {0, 0};

  pileup.AddRecord(record);

  // second record
  record.sequences = {"AAAGAA", "AAAAGG"};
  record.quality_values = {"aaagaa", "aaaagg"};
  record.cigars = {"3=1+2=", "4=2+"};
  record.positions = {0, 0};

  pileup.AddRecord(record);

  // third record
  record.sequences = {"GGAGGG", "AAAAAA"};
  record.quality_values = {"ggaggg", "aaaaaa"};
  record.cigars = {"2+1=3+", "6="};
  record.positions = {0, 0};

  pileup.AddRecord(record);

  // testing results
  std::string seq, qual;
  for (size_t i = 0; i < 6; ++i) {
    std::tie(seq, qual) = pileup.GetPileup(i);
    EXPECT_EQ(seq.find_first_not_of('A'), std::string::npos);
    EXPECT_EQ(qual.find_first_not_of('a'), std::string::npos);
  }
}

TEST(CalqTest, recordPileupDeletions) {
  auto pileup = genie::quality::calq::RecordPileup();

  genie::quality::calq::EncodingRecord record;

  // first record
  record.sequences = {"AA", "GG"};
  record.quality_values = {"aa", "gg"};
  record.cigars = {"1=2-1=", "2="};
  record.positions = {0, 1};

  pileup.AddRecord(record);

  // second record
  record.sequences = {"TT", "CC"};
  record.quality_values = {"tt", "cc"};
  record.cigars = {"1=2-1=", "2="};
  record.positions = {0, 1};

  pileup.AddRecord(record);

  std::string seq, qual;

  std::tie(seq, qual) = pileup.GetPileup(0);
  EXPECT_EQ(seq, "AT");
  EXPECT_EQ(qual, "at");

  std::tie(seq, qual) = pileup.GetPileup(1);
  EXPECT_EQ(seq, "GC");
  EXPECT_EQ(qual, "gc");

  std::tie(seq, qual) = pileup.GetPileup(2);
  EXPECT_EQ(seq, "GC");
  EXPECT_EQ(qual, "gc");

  std::tie(seq, qual) = pileup.GetPileup(3);
  EXPECT_EQ(seq, "AT");
  EXPECT_EQ(qual, "at");
}

TEST(CalqTest, recordPileupSoftClips) {
  auto pileup = genie::quality::calq::RecordPileup();

  genie::quality::calq::EncodingRecord record;

  // first record
  record.sequences = {"AAGGAA", "TGTTGT"};
  record.quality_values = {"aaggaa", "tgttgt"};
  record.cigars = {"2=(2)2=", "1=(1)2=(1)1="};
  record.positions = {0, 0};

  pileup.AddRecord(record);

  std::string seq, qual;

  std::tie(seq, qual) = pileup.GetPileup(0);
  EXPECT_EQ(seq, "AT");
  EXPECT_EQ(qual, "at");

  std::tie(seq, qual) = pileup.GetPileup(1);
  EXPECT_EQ(seq, "AT");
  EXPECT_EQ(qual, "at");

  std::tie(seq, qual) = pileup.GetPileup(2);
  EXPECT_EQ(seq, "AT");
  EXPECT_EQ(qual, "at");

  std::tie(seq, qual) = pileup.GetPileup(3);
  EXPECT_EQ(seq, "AT");
  EXPECT_EQ(qual, "at");
}

TEST(CalqTest, recordPileupGetRecordsBefore) {
  auto pileup = genie::quality::calq::RecordPileup();

  genie::quality::calq::EncodingRecord record;

  // first record
  record.sequences = {"AA", "AA"};
  record.quality_values = {"aa", "aa"};
  record.cigars = {"2=", "2="};
  record.positions = {1, 3};

  pileup.AddRecord(record);

  // second record
  record.sequences = {"GG", "GG"};
  record.quality_values = {"gg", "gg"};
  record.cigars = {"2=", "2="};
  record.positions = {5, 7};

  pileup.AddRecord(record);

  EXPECT_EQ(pileup.GetMinPos(), 1);

  auto pileup_copy = pileup;

  // nothing before pos
  auto records = pileup_copy.GetRecordsBefore(4);
  EXPECT_TRUE(records.empty());

  // one record before pos
  records = pileup_copy.GetRecordsBefore(6);
  EXPECT_EQ(pileup_copy.GetMinPos(), 5);
  EXPECT_EQ(records.size(), 1);
  EXPECT_EQ(records[0].positions[0], 1);
  EXPECT_EQ(records[0].positions[1], 3);
  EXPECT_EQ(records[0].sequences[0], "AA");
  EXPECT_EQ(records[0].sequences[1], "AA");
  EXPECT_EQ(records[0].quality_values[0], "aa");
  EXPECT_EQ(records[0].quality_values[1], "aa");

  // remove all
  records = pileup.GetRecordsBefore(9);
  EXPECT_TRUE(pileup.empty());
  EXPECT_EQ(records.size(), 2);
  EXPECT_EQ(records[0].positions[0], 1);
  EXPECT_EQ(records[0].positions[1], 3);
  EXPECT_EQ(records[0].sequences[0], "AA");
  EXPECT_EQ(records[0].sequences[1], "AA");
  EXPECT_EQ(records[0].quality_values[0], "aa");
  EXPECT_EQ(records[0].quality_values[1], "aa");
  EXPECT_EQ(records[1].positions[0], 5);
  EXPECT_EQ(records[1].positions[1], 7);
  EXPECT_EQ(records[1].sequences[0], "GG");
  EXPECT_EQ(records[1].sequences[1], "GG");
  EXPECT_EQ(records[1].quality_values[0], "gg");
  EXPECT_EQ(records[1].quality_values[1], "gg");
}