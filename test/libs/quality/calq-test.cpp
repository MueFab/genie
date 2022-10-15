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
    record.qvalues = {"aaaa", "gggg"};
    record.cigars = {"4=", "4="};
    record.positions = {0, 10};

    pileup.addRecord(record);

    // second record
    record.sequences = {"CCCC", "TTTTTTTT"};
    record.qvalues = {"cccc", "tttttttt"};
    record.cigars = {"4=", "8="};
    record.positions = {2, 8};

    pileup.addRecord(record);

    // third record
    record.sequences = {"AAAA", "CCCC"};
    record.qvalues = {"aaaa", "cccc"};
    record.cigars = {"4=", "4="};
    record.positions = {10, 11};

    pileup.addRecord(record);

    // test results
    std::string seq, qual;

    std::tie(seq, qual) = pileup.getPileup(0);
    EXPECT_EQ(seq, "A");
    ASSERT_EQ(qual, "a");

    std::tie(seq, qual) = pileup.getPileup(2);
    EXPECT_EQ(seq, "AC");
    EXPECT_EQ(qual, "ac");

    std::tie(seq, qual) = pileup.getPileup(4);
    EXPECT_EQ(seq, "C");
    EXPECT_EQ(qual, "c");

    std::tie(seq, qual) = pileup.getPileup(6);
    EXPECT_EQ(seq, "");
    EXPECT_EQ(qual, "");

    std::tie(seq, qual) = pileup.getPileup(11);
    EXPECT_EQ(seq, "GTAC");
    EXPECT_EQ(qual, "gtac");

    std::tie(seq, qual) = pileup.getPileup(15);
    EXPECT_EQ(seq, "T");
    EXPECT_EQ(qual, "t");

    EXPECT_ANY_THROW(pileup.getPileup(16));
}
//
TEST(CalqTest, recordPileupInsertions) {
    auto pileup = genie::quality::calq::RecordPileup();

    genie::quality::calq::EncodingRecord record;

    // first record
    record.sequences = {"GGAAAA", "AGGAAA"};
    record.qvalues = {"ggaaaa", "aggaaa"};
    record.cigars = {"2+4=", "1=2+3="};
    record.positions = {0, 0};

    pileup.addRecord(record);

    // second record
    record.sequences = {"AAAGAA", "AAAAGG"};
    record.qvalues = {"aaagaa", "aaaagg"};
    record.cigars = {"3=1+2=", "4=2+"};
    record.positions = {0, 0};

    pileup.addRecord(record);

    // third record
    record.sequences = {"GGAGGG", "AAAAAA"};
    record.qvalues = {"ggaggg", "aaaaaa"};
    record.cigars = {"2+1=3+", "6="};
    record.positions = {0, 0};

    pileup.addRecord(record);

    // testing results
    std::string seq, qual;
    for (size_t i = 0; i < 6; ++i) {
        std::tie(seq, qual) = pileup.getPileup(i);
        EXPECT_EQ(seq.find_first_not_of('A'), std::string::npos);
        EXPECT_EQ(qual.find_first_not_of('a'), std::string::npos);
    }
}

TEST(CalqTest, recordPileupDeletions) {
    auto pileup = genie::quality::calq::RecordPileup();

    genie::quality::calq::EncodingRecord record;

    // first record
    record.sequences = {"AA", "GG"};
    record.qvalues = {"aa", "gg"};
    record.cigars = {"1=2-1=", "2="};
    record.positions = {0, 1};

    pileup.addRecord(record);

    // second record
    record.sequences = {"TT", "CC"};
    record.qvalues = {"tt", "cc"};
    record.cigars = {"1=2-1=", "2="};
    record.positions = {0, 1};

    pileup.addRecord(record);

    std::string seq, qual;

    std::tie(seq, qual) = pileup.getPileup(0);
    EXPECT_EQ(seq, "AT");
    EXPECT_EQ(qual, "at");

    std::tie(seq, qual) = pileup.getPileup(1);
    EXPECT_EQ(seq, "GC");
    EXPECT_EQ(qual, "gc");

    std::tie(seq, qual) = pileup.getPileup(2);
    EXPECT_EQ(seq, "GC");
    EXPECT_EQ(qual, "gc");

    std::tie(seq, qual) = pileup.getPileup(3);
    EXPECT_EQ(seq, "AT");
    EXPECT_EQ(qual, "at");
}

TEST(CalqTest, recordPileupSoftClips) {
    auto pileup = genie::quality::calq::RecordPileup();

    genie::quality::calq::EncodingRecord record;

    // first record
    record.sequences = {"AAGGAA", "TGTTGT"};
    record.qvalues = {"aaggaa", "tgttgt"};
    record.cigars = {"2=(2)2=", "1=(1)2=(1)1="};
    record.positions = {0, 0};

    pileup.addRecord(record);

    std::string seq, qual;

    std::tie(seq, qual) = pileup.getPileup(0);
    EXPECT_EQ(seq, "AT");
    EXPECT_EQ(qual, "at");

    std::tie(seq, qual) = pileup.getPileup(1);
    EXPECT_EQ(seq, "AT");
    EXPECT_EQ(qual, "at");

    std::tie(seq, qual) = pileup.getPileup(2);
    EXPECT_EQ(seq, "AT");
    EXPECT_EQ(qual, "at");

    std::tie(seq, qual) = pileup.getPileup(3);
    EXPECT_EQ(seq, "AT");
    EXPECT_EQ(qual, "at");
}

TEST(CalqTest, recordPileupGetRecordsBefore) {
    auto pileup = genie::quality::calq::RecordPileup();

    genie::quality::calq::EncodingRecord record;

    // first record
    record.sequences = {"AA", "AA"};
    record.qvalues = {"aa", "aa"};
    record.cigars = {"2=", "2="};
    record.positions = {1, 3};

    pileup.addRecord(record);

    // second record
    record.sequences = {"GG", "GG"};
    record.qvalues = {"gg", "gg"};
    record.cigars = {"2=", "2="};
    record.positions = {5, 7};

    pileup.addRecord(record);

    EXPECT_EQ(pileup.getMinPos(), 1);

    auto pileup_copy = pileup;

    // nothing before pos
    auto records = pileup_copy.getRecordsBefore(4);
    EXPECT_TRUE(records.empty());

    // one record before pos
    records = pileup_copy.getRecordsBefore(6);
    EXPECT_EQ(pileup_copy.getMinPos(), 5);
    EXPECT_EQ(records.size(), 1);
    EXPECT_EQ(records[0].positions[0], 1);
    EXPECT_EQ(records[0].positions[1], 3);
    EXPECT_EQ(records[0].sequences[0], "AA");
    EXPECT_EQ(records[0].sequences[1], "AA");
    EXPECT_EQ(records[0].qvalues[0], "aa");
    EXPECT_EQ(records[0].qvalues[1], "aa");

    // remove all
    records = pileup.getRecordsBefore(9);
    EXPECT_TRUE(pileup.empty());
    EXPECT_EQ(records.size(), 2);
    EXPECT_EQ(records[0].positions[0], 1);
    EXPECT_EQ(records[0].positions[1], 3);
    EXPECT_EQ(records[0].sequences[0], "AA");
    EXPECT_EQ(records[0].sequences[1], "AA");
    EXPECT_EQ(records[0].qvalues[0], "aa");
    EXPECT_EQ(records[0].qvalues[1], "aa");
    EXPECT_EQ(records[1].positions[0], 5);
    EXPECT_EQ(records[1].positions[1], 7);
    EXPECT_EQ(records[1].sequences[0], "GG");
    EXPECT_EQ(records[1].sequences[1], "GG");
    EXPECT_EQ(records[1].qvalues[0], "gg");
    EXPECT_EQ(records[1].qvalues[1], "gg");
}