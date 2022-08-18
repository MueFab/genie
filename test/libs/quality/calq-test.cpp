#include <gtest/gtest.h>
#include <iostream>
#include "genie/quality/calq/local-reference.h"

TEST(CalqTest, localReferenceBasic) {
    auto pileup = genie::quality::calq::LocalReference(1024);

    // usually done in addRead, but i dont want to craft records
    // first "record"
    pileup.nextRecord();

    pileup.addSingleRead("AAAA", "aaaa", "4=", 0);
    pileup.addSingleRead("GGGG", "gggg", "4=", 10);

    // second "record"
    pileup.nextRecord();

    pileup.addSingleRead("CCCC", "cccc", "4=", 2);
    pileup.addSingleRead("TTTTTTTT", "tttttttt", "8=", 8);

    // third "record"
    pileup.nextRecord();

    pileup.addSingleRead("AAAA", "aaaa", "4=", 10);
    pileup.addSingleRead("CCCC", "cccc", "4=", 11);

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

TEST(CalqTest, localReferenceInsertions) {
    auto pileup = genie::quality::calq::LocalReference(1024);

    pileup.nextRecord();
    pileup.addSingleRead("GGAAAA", "ggaaaa", "2+4=", 0);
    pileup.addSingleRead("AGGAAA", "aggaaa", "1=2+3=", 0);

    pileup.nextRecord();
    pileup.addSingleRead("AAAGAA", "aaagaa", "3=1+2=", 0);
    pileup.addSingleRead("AAAAGG", "aaaagg", "4=2+", 0);

    pileup.nextRecord();
    pileup.addSingleRead("GGAGGG", "ggaggg", "2+1=3+", 0);
    pileup.addSingleRead("AAAAAA", "aaaaaa", "6=", 0);

    std::string seq, qual;
    for (size_t i = 0; i < 6; ++i) {
        std::tie(seq, qual) = pileup.getPileup(i);
        EXPECT_EQ(seq.find_first_not_of("A"), std::string::npos);
        EXPECT_EQ(qual.find_first_not_of("a"), std::string::npos);
    }
}

TEST(CalqTest, localReferenceDeletions) {
    auto pileup = genie::quality::calq::LocalReference(1024);

    pileup.nextRecord();
    pileup.addSingleRead("AA", "aa", "1=2-1=", 0);
    pileup.addSingleRead("GG", "gg", "2=", 1);

    pileup.nextRecord();
    pileup.addSingleRead("TT", "tt", "1=2-1=", 0);
    pileup.addSingleRead("CC", "cc", "2=", 1);

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

TEST(CalqTest, localReferenceSoftClips) {
    auto pileup = genie::quality::calq::LocalReference(1024);

    pileup.nextRecord();
    pileup.addSingleRead("AAGGAA", "aaggaa", "2=(2)2=", 0);
    pileup.addSingleRead("TGTTGT", "tgttgt", "1=(1)2=(1)1=", 0);

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