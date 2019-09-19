#include "calq/fasta-file-reader.h"
#include <gtest/gtest.h>
#include "calq/fasta-record.h"
#include "helpers.h"

TEST(FastaFileReader, Constructor) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FastaFileReader fastaFileReader(gitRootDir + "/resources/test-files/fasta/minimal.fasta");
}

TEST(FastaFileReader, Parse) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FastaFileReader fastaFileReader(gitRootDir + "/resources/test-files/fasta/minimal.fasta");

    std::vector<calq::FastaRecord> fastaRecords;
    fastaFileReader.parse(&fastaRecords);

    EXPECT_EQ(fastaRecords.size(), 1);
    EXPECT_EQ(fastaRecords.front().header, ">minimal");
    EXPECT_EQ(fastaRecords.front().sequence, "GATTACA");
}
