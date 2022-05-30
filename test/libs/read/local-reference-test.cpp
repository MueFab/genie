#include <genie/read/localassembly/local-reference.h>
#include <genie/util/exception.h>
#include <gtest/gtest.h>
#include <iostream>
#include <string>

typedef genie::read::localassembly::LocalReference LocalReference;

TEST(LocalReferenceTest, shiftedReads) {
    LocalReference localRef(1024);
    std::string reference = "ACCGTTCGAAATGCGTTACGCGCCATGCTA";
    std::string cigar = "5=";

    // add reads in overlapping pieces
    for (uint64_t pos = 0; pos <= reference.size() - 5; ++pos) {
        localRef.addSingleRead(reference.substr(pos, 5), cigar, pos);
    }

    EXPECT_EQ(reference, localRef.getReference(0, static_cast<uint32_t>(reference.size())));
}

TEST(LocalReferenceTest, majorityVoteNull) {
    LocalReference localRef(1024);

    // null should always loose majority-vote
    localRef.addSingleRead("0000000", "7=", 0);
    localRef.addSingleRead("0ACGTN0", "7=", 0);
    localRef.addSingleRead("0000000", "7=", 0);

    // to get string with null-byte
    std::string expected("\0ACGTN\0", 7);
    EXPECT_EQ(localRef.getReference(0, 7), expected);
}

TEST(LocalReferenceTest, majorityVoteDraw) {
    LocalReference localRef(1024);

    // A should win vs C, C vs G and so on
    localRef.addSingleRead("AGGNN", "5=", 0);
    localRef.addSingleRead("CCTT0", "5=", 0);
    EXPECT_EQ(localRef.getReference(0, 5), "ACGTN");
}

TEST(LocalReferenceTest, insertion) {
    LocalReference localRef(1024);

    // simple insertion
    localRef.addSingleRead("ACACCACCCA", "1=1+1=2+1=3+1=", 0);

    // to get string with null-byte
    std::string expected("AAAA\0", 5);
    EXPECT_EQ(localRef.getReference(0, 5), expected);
}

TEST(LocalReferenceTest, deletion) {
    LocalReference localRef(1024);

    // 1. AA  AA  A
    // 2.   C   T
    // 3.    G   G
    // 4. NNNNNNNNN
    localRef.addSingleRead("AAAAA", "2=2-2=2-1=", 0);
    localRef.addSingleRead("CT", "1=3-1=", 2);
    localRef.addSingleRead("GG", "1=3-1=", 3);
    localRef.addSingleRead("NNNNNNNNN", "9=", 0);

    EXPECT_EQ(localRef.getReference(0, 9), "AACGAATGA");
}

TEST(LocalReferenceTest, softClip) {
    LocalReference localRef(1024);

    localRef.addSingleRead("AGAGA", "(1)3=(1)", 0);
    EXPECT_EQ(localRef.getReference(0, 3), "GAG");

    // should not change ref
    localRef.addSingleRead("AAA", "(3)", 0);
    EXPECT_EQ(localRef.getReference(0, 3), "GAG");

    // should change ref
    localRef.addSingleRead("AAA", "(1)1=(1)", 0);
    EXPECT_EQ(localRef.getReference(0, 3), "AAG");
}

// TEST(LocalReferenceTest, hardClip) {
//
// }

TEST(LocalReferenceTest, bufferOverflow) {
    LocalReference localRef(8);

    localRef.addSingleRead("AAAA", "4=", 0);
    localRef.addSingleRead("GGGG", "4=", 0);
    localRef.addSingleRead("TTTT", "4=", 0);
    EXPECT_EQ(localRef.getReference(0, 4), "GGGG");
}

TEST(LocalReferenceTest, wrongUsage) {
    // too long read
    LocalReference localRef(2);
    EXPECT_THROW(localRef.addSingleRead("AAA", "3=", 0), genie::util::RuntimeException);

    // wrong cigar-length
    localRef = LocalReference(32);
    EXPECT_THROW(localRef.addSingleRead("AAA", "1+1=", 0), genie::util::RuntimeException);

    // wrong read order TODO: is the order important?
    localRef = LocalReference(32);
    localRef.addSingleRead("AAA", "3=", 2);
    EXPECT_THROW(localRef.addSingleRead("CCCCC", "5=", 0), genie::util::RuntimeException);
}