#include <gtest/gtest.h>

#include <utils/fastq-file-reader.h>
#include <utils/string-helpers.h>

std::string exec(const std::string &cmd) {
    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        return "<exec(" + cmd + ") failed>";
    }

    const int bufferSize = 256;
    char buffer[bufferSize];
    std::string result;

    while (!feof(pipe)) {
        if (fgets(buffer, bufferSize, pipe) != nullptr) {
            result += buffer;
        }
    }

    pclose(pipe);

    utils::rtrim(result);

    return result;
}

TEST(FastqFileReader, Simplest) { //NOLINT(cert-err-cpp)
    std::string gitRootDir = exec("git rev-parse --show-toplevel");
    utils::rtrim(gitRootDir);

    utils::FastqFileReader reader(gitRootDir + "/resources/test-files/fastq/simplest.fastq");

    std::vector<utils::FastqRecord> records;
    reader.readRecords(1, &records);

    EXPECT_EQ(records.size(), 1);
}