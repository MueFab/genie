#include <gtest/gtest.h>
#include <iostream>
#include "genie/quality/calq/calq_coder.h"
#include "genie/quality/calq/record_pileup.h"

TEST(CalqTest, localReferenceBasic) {
    auto pileup = genie::quality::calq::RecordPileup();

    // usually done in addRead, but i dont want to craft records
    // first "record"

    ::calq::EncodingRecord record;

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
// TEST(CalqTest, localReferenceInsertions) {
//    auto pileup = genie::quality::calq::RecordPileup();
//
//    pileup.nextRecord();
//    pileup.addSingleRead("GGAAAA", "ggaaaa", "2+4=", 0);
//    pileup.addSingleRead("AGGAAA", "aggaaa", "1=2+3=", 0);
//
//    pileup.nextRecord();
//    pileup.addSingleRead("AAAGAA", "aaagaa", "3=1+2=", 0);
//    pileup.addSingleRead("AAAAGG", "aaaagg", "4=2+", 0);
//
//    pileup.nextRecord();
//    pileup.addSingleRead("GGAGGG", "ggaggg", "2+1=3+", 0);
//    pileup.addSingleRead("AAAAAA", "aaaaaa", "6=", 0);
//
//    std::string seq, qual;
//    for (size_t i = 0; i < 6; ++i) {
//        std::tie(seq, qual) = pileup.getPileup(i);
//        EXPECT_EQ(seq.find_first_not_of("A"), std::string::npos);
//        EXPECT_EQ(qual.find_first_not_of("a"), std::string::npos);
//    }
//}
//
// TEST(CalqTest, localReferenceDeletions) {
//    auto pileup = genie::quality::calq::RecordPileup();
//
//    pileup.nextRecord();
//    pileup.addSingleRead("AA", "aa", "1=2-1=", 0);
//    pileup.addSingleRead("GG", "gg", "2=", 1);
//
//    pileup.nextRecord();
//    pileup.addSingleRead("TT", "tt", "1=2-1=", 0);
//    pileup.addSingleRead("CC", "cc", "2=", 1);
//
//    std::string seq, qual;
//
//    std::tie(seq, qual) = pileup.getPileup(0);
//    EXPECT_EQ(seq, "AT");
//    EXPECT_EQ(qual, "at");
//
//    std::tie(seq, qual) = pileup.getPileup(1);
//    EXPECT_EQ(seq, "GC");
//    EXPECT_EQ(qual, "gc");
//
//    std::tie(seq, qual) = pileup.getPileup(2);
//    EXPECT_EQ(seq, "GC");
//    EXPECT_EQ(qual, "gc");
//
//    std::tie(seq, qual) = pileup.getPileup(3);
//    EXPECT_EQ(seq, "AT");
//    EXPECT_EQ(qual, "at");
//}
//
// TEST(CalqTest, localReferenceSoftClips) {
//    auto pileup = genie::quality::calq::RecordPileup();
//
//    pileup.nextRecord();
//    pileup.addSingleRead("AAGGAA", "aaggaa", "2=(2)2=", 0);
//    pileup.addSingleRead("TGTTGT", "tgttgt", "1=(1)2=(1)1=", 0);
//
//    std::string seq, qual;
//
//    std::tie(seq, qual) = pileup.getPileup(0);
//    EXPECT_EQ(seq, "AT");
//    EXPECT_EQ(qual, "at");
//
//    std::tie(seq, qual) = pileup.getPileup(1);
//    EXPECT_EQ(seq, "AT");
//    EXPECT_EQ(qual, "at");
//
//    std::tie(seq, qual) = pileup.getPileup(2);
//    EXPECT_EQ(seq, "AT");
//    EXPECT_EQ(qual, "at");
//
//    std::tie(seq, qual) = pileup.getPileup(3);
//    EXPECT_EQ(seq, "AT");
//    EXPECT_EQ(qual, "at");
//}
//
// TEST(CalqTest, localReferenceGetRecordsBefore) {
//    auto pileup = genie::quality::calq::RecordPileup();
//
//    pileup.nextRecord();
//    pileup.addSingleRead("AA", "aa", "2=", 1);
//    pileup.addSingleRead("AA", "aa", "2=", 3);
//
//    pileup.nextRecord();
//    pileup.addSingleRead("GG", "gg", "2=", 5);
//    pileup.addSingleRead("GG", "gg", "2=", 7);
//
//    EXPECT_EQ(pileup.getMinPos(), 1);
//
//    auto pileup_copy = pileup;
//
//    std::vector<std::vector<std::string>> seqs, quals;
//    std::vector<std::vector<uint64_t>> positions;
//
//    // nothing before pos
//    std::tie(positions, seqs, quals) = pileup_copy.getRecordsBefore(4);
//    EXPECT_TRUE(positions.empty());
//    EXPECT_TRUE(seqs.empty());
//    EXPECT_TRUE(quals.empty());
//
//    // one record before pos
//    std::tie(positions, seqs, quals) = pileup_copy.getRecordsBefore(6);
//    EXPECT_EQ(pileup_copy.getMinPos(), 5);
//    EXPECT_EQ(positions.size(), 1);
//    EXPECT_EQ(positions[0][0], 1);
//    EXPECT_EQ(positions[0][1], 3);
//    EXPECT_EQ(seqs[0][0], "AA");
//    EXPECT_EQ(seqs[0][1], "AA");
//    EXPECT_EQ(quals[0][0], "aa");
//    EXPECT_EQ(quals[0][1], "aa");
//
//    // remove all
//    std::tie(positions, seqs, quals) = pileup.getRecordsBefore(9);
//    EXPECT_TRUE(pileup.empty());
//    EXPECT_EQ(positions.size(), 2);
//    EXPECT_EQ(positions[0][0], 1);
//    EXPECT_EQ(positions[0][1], 3);
//    EXPECT_EQ(positions[1][0], 5);
//    EXPECT_EQ(positions[1][1], 7);
//    EXPECT_EQ(seqs[0][0], "AA");
//    EXPECT_EQ(seqs[0][1], "AA");
//    EXPECT_EQ(seqs[1][0], "GG");
//    EXPECT_EQ(seqs[1][1], "GG");
//    EXPECT_EQ(quals[0][0], "aa");
//    EXPECT_EQ(quals[0][1], "aa");
//    EXPECT_EQ(quals[1][0], "gg");
//    EXPECT_EQ(quals[1][1], "gg");
//}