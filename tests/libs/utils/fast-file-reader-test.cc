#include <gtest/gtest.h>

#include <utils/fasta-file-reader.h>
#include <utils/string-helpers.h>

TEST(FastaFileReader, Minimal) { //NOLINT(cert-err-cpp)
    std::string gitRootDir = exec("git rev-parse --show-toplevel");
    utils::rtrim(gitRootDir);

    utils::FastaFileReader reader(gitRootDir + "/resources/test-files/fasta/minimal.fasta");

    std::vector<utils::FastaRecord> records;
    reader.parse(&records);

    EXPECT_EQ(records.size(), 1);
    EXPECT_EQ(records.front().header, ">minimal");
    EXPECT_EQ(records.front().sequence, "GATTACA");
}

TEST(FastaFileReader, Empty) { //NOLINT(cert-err-cpp
    std::string gitRootDir = exec("git rev-parse --show-toplevel");
    utils::rtrim(gitRootDir);

    utils::FastaFileReader reader(gitRootDir + "/resources/test-files/fasta/empty.fasta");

    std::vector<utils::FastaRecord> records;
    reader.parse(&records);

    EXPECT_EQ(records.size(), 0);
    EXPECT_EQ(records.front().header, NULL);
    EXPECT_EQ(records.front().sequence, NULL);

     }