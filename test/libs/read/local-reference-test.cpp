#include "genie/read/localassembly/local_reference.h"
#include "genie/util/runtime_exception.h"
#include <gtest/gtest.h>

#include <string>

using LocalReference = genie::read::localassembly::LocalReference;

TEST(LocalReferenceTest, shiftedReads) {
  LocalReference local_ref(1024);
  const std::string reference = "ACCGTTCGAAATGCGTTACGCGCCATGCTA";
  const std::string cigar = "5=";

  // add reads in overlapping pieces
  for (uint64_t pos = 0; pos <= reference.size() - 5; ++pos) {
    local_ref.AddSingleRead(reference.substr(pos, 5), cigar, pos);
  }

  EXPECT_EQ(reference,
            local_ref.GetReference(0, static_cast<uint32_t>(reference.size())));
}

TEST(LocalReferenceTest, majorityVoteNull) {
  LocalReference local_ref(1024);

  // null should always loose majority-vote
  local_ref.AddSingleRead("0000000", "7=", 0);
  local_ref.AddSingleRead("0ACGTN0", "7=", 0);
  local_ref.AddSingleRead("0000000", "7=", 0);

  // to get string with null-byte
  const std::string expected("\0ACGTN\0", 7);
  EXPECT_EQ(local_ref.GetReference(0, 7), expected);
}

TEST(LocalReferenceTest, majorityVoteDraw) {
  LocalReference local_ref(1024);

  // A should win vs C, C vs G and so on
  local_ref.AddSingleRead("AGGNN", "5=", 0);
  local_ref.AddSingleRead("CCTT0", "5=", 0);
  EXPECT_EQ(local_ref.GetReference(0, 5), "ACGTN");
}

TEST(LocalReferenceTest, insertion) {
  LocalReference local_ref(1024);

  // simple insertion
  local_ref.AddSingleRead("ACACCACCCA", "1=1+1=2+1=3+1=", 0);

  // to get string with null-byte
  const std::string expected("AAAA\0", 5);
  EXPECT_EQ(local_ref.GetReference(0, 5), expected);
}

TEST(LocalReferenceTest, deletion) {
  LocalReference local_ref(1024);

  // 1. AA  AA  A
  // 2.   C   T
  // 3.    G   G
  // 4. NNNNNNNNN
  local_ref.AddSingleRead("AAAAA", "2=2-2=2-1=", 0);
  local_ref.AddSingleRead("CT", "1=3-1=", 2);
  local_ref.AddSingleRead("GG", "1=3-1=", 3);
  local_ref.AddSingleRead("NNNNNNNNN", "9=", 0);

  EXPECT_EQ(local_ref.GetReference(0, 9), "AACGAATGA");
}

TEST(LocalReferenceTest, softClip) {
  LocalReference local_ref(1024);

  local_ref.AddSingleRead("AGAGA", "(1)3=(1)", 0);
  EXPECT_EQ(local_ref.GetReference(0, 3), "GAG");

  // should not change ref
  local_ref.AddSingleRead("AAA", "(3)", 0);
  EXPECT_EQ(local_ref.GetReference(0, 3), "GAG");

  // should change ref
  local_ref.AddSingleRead("AAA", "(1)1=(1)", 0);
  EXPECT_EQ(local_ref.GetReference(0, 3), "AAG");
}

TEST(LocalReferenceTest, mixedCigar) {
  LocalReference local_ref(1024);

  // mix of deletion and insertions
  local_ref.AddSingleRead("AAAA", "4-4+", 0);
  local_ref.AddSingleRead("AAAA", "2+4-2+", 0);
  local_ref.AddSingleRead("AAAA", "4+4-", 0);
  EXPECT_EQ(local_ref.GetReference(0, 4), std::string("\0\0\0\0", 4));

  // mixed all
  local_ref = LocalReference(1024);

  local_ref.AddSingleRead("AGTCGT", "[4](2)2=2-2+[2]", 0);
  EXPECT_EQ(local_ref.GetReference(0, 4), std::string("TC\0\0", 4));

  local_ref.AddSingleRead("AGTCGT", "(2)2=(2)", 3);
  EXPECT_EQ(local_ref.GetReference(0, 5), std::string("TC\0TC", 5));
  local_ref.AddSingleRead("AAAAA", "2+1=(2)", 2);
  EXPECT_EQ(local_ref.GetReference(0, 5), std::string("TCATC", 5));
}

TEST(LocalReferenceTest, bufferOverflow) {
  LocalReference local_ref(8);

  local_ref.AddSingleRead("AAAA", "4=", 0);
  local_ref.AddSingleRead("GGGG", "4=", 0);
  local_ref.AddSingleRead("TTTT", "4=", 0);
  EXPECT_EQ(local_ref.GetReference(0, 4), "GGGG");
}

TEST(LocalReferenceTest, wrongUsage) {
  // too long read
  LocalReference local_ref(2);
  EXPECT_THROW(local_ref.AddSingleRead("AAA", "3=", 0),
               genie::util::RuntimeException);

  // wrong cigar-length
  local_ref = LocalReference(32);
  EXPECT_THROW(local_ref.AddSingleRead("AAA", "1+1=", 0),
               genie::util::RuntimeException);
}