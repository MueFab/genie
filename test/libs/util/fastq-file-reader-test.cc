#include <gtest/gtest.h>
#include <util/fastq-file-reader.h>
#include "helpers.h"

TEST(FastqFileReader, Simplest) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    util::FastqFileReader reader(gitRootDir + "/resources/test-files/fastq/simplest.fastq");

    std::vector<util::FastqRecord> records;
    reader.readRecords(1, &records);

    EXPECT_EQ(records.size(), 1);
    EXPECT_EQ(records.front().title, "@title1");
    EXPECT_EQ(records.front().sequence, "AAAA");
    EXPECT_EQ(records.front().optional, "+");
    EXPECT_EQ(records.front().qualityScores, "QUAL");
}

TEST(FastqFileReader, FourteenRecords) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    util::FastqFileReader reader(gitRootDir + "/resources/test-files/fastq/fourteen-gattaca-records.fastq");

    std::vector<util::FastqRecord> records;
    reader.readRecords(14, &records);

    EXPECT_EQ(records.size(), 14);
    EXPECT_EQ(records.front().title, "@title1");
    EXPECT_EQ(records.front().sequence, "GATTACA");
    EXPECT_EQ(records.front().optional, "+");
    EXPECT_EQ(records.front().qualityScores, "QUALITY");

    EXPECT_EQ(records.back().title, "@title14");
    EXPECT_EQ(records.back().sequence, "ACATTAG");
    EXPECT_EQ(records.back().optional, "+");
    EXPECT_EQ(records.back().qualityScores, "YTILAUQ");
}

TEST(FastqFileReader, BlankLine) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    util::FastqFileReader reader(gitRootDir + "/resources/test-files/fastq/blank-line.fastq");

    std::vector<util::FastqRecord> records;
    reader.readRecords(2, &records);

    EXPECT_EQ(records.size(), 2);
    EXPECT_EQ(records.front().title, "@title1");
    EXPECT_EQ(records.front().sequence, "GATTACA");
    EXPECT_EQ(records.front().optional, "+");
    EXPECT_EQ(records.front().qualityScores, "QUALITY");

    EXPECT_EQ(records.back().title, "\n");
    EXPECT_EQ(records.back().sequence, "GATTACA");
    EXPECT_EQ(records.back().optional, "+");
    EXPECT_EQ(records.back().qualityScores, "QUALITY");
}

TEST(FastqFileReader, Truncated) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    util::FastqFileReader reader(gitRootDir + "/resources/test-files/fastq/fourteen-gattaca-records-truncated.fastq");

    std::vector<util::FastqRecord> records;
    EXPECT_THROW(reader.readRecords(14, &records), std::runtime_error);
}