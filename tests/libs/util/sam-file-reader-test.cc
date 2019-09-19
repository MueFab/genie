#include "calq/sam-file-reader.h"
#include <gtest/gtest.h>
#include "calq/sam-record.h"
#include "helpers.h"

TEST(SamFileReader, Constructoor) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::SamFileReader samFileReader(gitRootDir + "/resources/test-files/sam/samspec.sam");
}

TEST(SamFileReader, Header) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::SamFileReader samFileReader(gitRootDir + "/resources/test-files/sam/samspec.sam");

    std::string samspecHeader = "@HD\tVN:1.5\tSO:coordinate\n";
    samspecHeader += "@SQ\tSN:ref\tLN:45\n";

    EXPECT_EQ(samFileReader.header(), samspecHeader);
}

TEST(SamFileReader, ReadRecords1) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::SamFileReader samFileReader(gitRootDir + "/resources/test-files/sam/samspec.sam");

    std::list<calq::SamRecord> samRecords;

    // Try to read all 6 records from the file
    EXPECT_EQ(samFileReader.readRecords(6, &samRecords), 6);

    // Check the first record
    EXPECT_EQ(samRecords.front().str(),
              "r001\t163\tref\t7\t30\t8M2I4M1D3M\t=\t37\t39\tTTAGATAAAGGATACTG\tQQQQQQQQQQQQQQQQQ\t*");

    // Check the last record
    EXPECT_EQ(samRecords.back().str(), "r001\t83\tref\t37\t30\t9M\t=\t7\t-39\tCAGCGGCAT\tQQQQQQQQQ\t*\tNM:i:1");
}

TEST(SamFileReader, ReadRecords2) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::SamFileReader samFileReader(gitRootDir + "/resources/test-files/sam/samspec.sam");

    std::list<calq::SamRecord> samRecords;

    // We request 7 records, although the file only has 6.
    EXPECT_EQ(samFileReader.readRecords(7, &samRecords), 6);
}
