#include <gtest/gtest.h>
#include <format/fasta/fasta-file-reader.h>
#include "helpers.h"

// May need to add some requirements on what counts as a valid FASTA in the documentation

TEST(FastaFileReader, Minimal) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    util::FastaFileReader reader(gitRootDir + "/resources/test-files/fasta/minimal.fasta");

    std::vector<util::FastaRecord> records;
    reader.parse(&records);

    EXPECT_EQ(records.size(), 1);
    EXPECT_EQ(records.front().header, ">minimal");
    EXPECT_EQ(records.front().sequence, "GATTACA");
}

TEST(FastaFileReader, Empty) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    util::FastaFileReader reader(gitRootDir + "/resources/test-files/fasta/empty.fasta");

    std::vector<util::FastaRecord> records;
    reader.parse(&records);

    EXPECT_EQ(records.size(), 1);
    EXPECT_EQ(records.front().header, "");
    EXPECT_EQ(records.front().sequence, "");
}

TEST(FastaFileReader, OnlyHeaders) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    util::FastaFileReader reader(gitRootDir + "/resources/test-files/fasta/only-headers.fasta");

    std::vector<util::FastaRecord> records;
    reader.parse(&records);

    // This is the behavior the code offers - might make more sense if it skipped any blank
    // records and this ended up an empty record as previous
    EXPECT_EQ(records.size(), 1);
    EXPECT_EQ(records.front().header, ">header2");
    EXPECT_EQ(records.front().sequence, "");
}

TEST(FastaFileReader, SingleSequenceNoHeader) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    util::FastaFileReader reader(gitRootDir + "/resources/test-files/fasta/single-sequence-no-header.fasta");

    std::vector<util::FastaRecord> records;
    reader.parse(&records);

    // There isn't a definitive answer on whether a bare sequence is allowed,
    // but allowing a blank line could cause problems, as seen in the Blank Line Middle test below
    EXPECT_EQ(records.size(), 1);
    EXPECT_EQ(records.front().header, "");
    EXPECT_EQ(records.front().sequence, "GATTACA");
}

TEST(FastaFileReader, SupernumeraryNewlines) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    util::FastaFileReader reader(gitRootDir + "/resources/test-files/fasta/supernumerary-newlines.fasta");

    std::vector<util::FastaRecord> records;

    EXPECT_THROW(reader.parse(&records), std::runtime_error);
}

TEST(FastaFileReader, TwoTimesGattaca) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    util::FastaFileReader reader(gitRootDir + "/resources/test-files/fasta/two-times-gattaca.fasta");

    std::vector<util::FastaRecord> records;
    reader.parse(&records);

    EXPECT_EQ(records.size(), 2);
    EXPECT_EQ(records.front().header, ">header1");
    EXPECT_EQ(records.front().sequence, "GATTACA");
    EXPECT_EQ(records.back().header, ">header2");
    EXPECT_EQ(records.back().sequence, "GATTACA");
}

TEST(FastaFileReader, BlankLineMiddle) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    util::FastaFileReader reader(gitRootDir + "/resources/test-files/fasta/blank-line-middle.fasta");

    std::vector<util::FastaRecord> records;
    reader.parse(&records);

    // This seems like a problem since it inserts the blank line character into the sequence
    // NCBI requires no blank lines in the middle of a sequence, probably for this reason
    EXPECT_EQ(records.size(), 1);
    EXPECT_EQ(records.front().header, ">header1");
    EXPECT_EQ(records.front().sequence, "GATTACA\nACATTAG");
    //    EXPECT_THROW(reader.parse(&records), std::runtime_error);
}

TEST(FastaFileReader, BlankLineBetween) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    util::FastaFileReader reader(gitRootDir + "/resources/test-files/fasta/blank-line-between.fasta");

    std::vector<util::FastaRecord> records;
    reader.parse(&records);

    // This too allows a newline character to be part of the sequence, which is not desirable
    EXPECT_EQ(records.size(), 2);
    EXPECT_EQ(records.front().header, ">header1");
    EXPECT_EQ(records.front().sequence, "GATTACA\n");
    EXPECT_EQ(records.back().header, ">header2");
    EXPECT_EQ(records.back().sequence, "ACATTAG");
}
