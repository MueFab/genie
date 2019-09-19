#include "calq/file-line-reader.h"
#include <gtest/gtest.h>
#include "helpers.h"

TEST(FileLineReader, Constructor) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileLineReader fileLineReader(gitRootDir + "/resources/test-files/tests/0x041f5aac");
}

TEST(FileLineReader, ReadLine) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileLineReader fileLineReader(gitRootDir + "/resources/test-files/tests/test-text.txt");

    std::string line;

    fileLineReader.readLine(&line);
    EXPECT_EQ(line, "test-line");

    fileLineReader.readLine(&line);
    EXPECT_EQ(line, "  test-line-leading-ws");

    fileLineReader.readLine(&line);
    EXPECT_EQ(line, "test-line-trailing-ws");
}
