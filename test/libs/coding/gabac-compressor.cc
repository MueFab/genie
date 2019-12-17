#include <gtest/gtest.h>
#include <coding/gabac-compressor.h>
#include "helpers.h"

TEST(GabacCompressor, Minimal) {  // NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    util::FastaFileReader reader(gitRootDir + "/resources/test-files/fasta/minimal.fasta");

    std::vector<util::FastaRecord> records;
    reader.parse(&records);

    EXPECT_EQ(records.size(), 1);
    EXPECT_EQ(records.front().header, ">minimal");
    EXPECT_EQ(records.front().sequence, "GATTACA");
}
