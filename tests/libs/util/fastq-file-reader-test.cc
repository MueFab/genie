#include <gtest/gtest.h>
#include <util/fastq-file-reader.h>
#include "helpers.h"

TEST(FastqFileReader, Simplest) {  //NOLINT(cert-err-cpp)
    std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");

    util::FastqFileReader reader(gitRootDir + "/resources/test-files/fastq/simplest.fastq");

    std::vector<util::FastqRecord> records;
    reader.readRecords(1, &records);

    EXPECT_EQ(records.size(), 1);
}
