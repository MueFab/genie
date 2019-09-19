#include <gtest/gtest.h>
#include "calq/file-reader.h"
#include "helpers.h"

TEST(File, Constructor) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileReader reader(gitRootDir + "/resources/test-files/tests/0x041f5aac");
}

TEST(File, AdvanceAndTell) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileReader reader(gitRootDir + "/resources/test-files/tests/0x041f5aac");

    // A fresh file reader should have a file position indicator at position 0.
    EXPECT_EQ(reader.tell(), 0);

    // Do not advance at all
    EXPECT_NO_THROW(reader.advance(0));
    EXPECT_EQ(reader.tell(), 0);

    // Advance by 1
    EXPECT_NO_THROW(reader.advance(1));
    EXPECT_EQ(reader.tell(), 1);

    // Go back by 1
    EXPECT_NO_THROW(reader.advance(-1));
    EXPECT_EQ(reader.tell(), 0);

    // Advance by 2
    EXPECT_NO_THROW(reader.advance(2));
    EXPECT_EQ(reader.tell(), 2);

    // Advance by 1
    EXPECT_NO_THROW(reader.advance(1));
    EXPECT_EQ(reader.tell(), 3);
}

TEST(File, Eof) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileReader fileReader(gitRootDir + "/resources/test-files/tests/0x041f5aac");

    // Without reading or writing from/to a file we do not really have the means to test the eof() function.
    EXPECT_EQ(fileReader.eof(), false);
}

TEST(File, SeekFromCurAndTell) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileReader fileReader(gitRootDir + "/resources/test-files/tests/0x041f5aac");

    EXPECT_NO_THROW(fileReader.seekFromCur(0));
    EXPECT_EQ(fileReader.tell(), 0);
}

TEST(File, SeekFromEndAndTell) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileReader fileReader(gitRootDir + "/resources/test-files/tests/0x041f5aac");

    EXPECT_NO_THROW(fileReader.seekFromEnd(0));
    EXPECT_EQ(fileReader.tell(), 4);
}

TEST(File, SeekFromSetAndTell) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileReader fileReader(gitRootDir + "/resources/test-files/tests/0x041f5aac");

    EXPECT_NO_THROW(fileReader.seekFromSet(0));
    EXPECT_EQ(fileReader.tell(), 0);
}

TEST(File, Size) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileReader fileReader(gitRootDir + "/resources/test-files/tests/0x041f5aac");

    EXPECT_EQ(fileReader.size(), 4);
}

TEST(File, Tell) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileReader fileReader(gitRootDir + "/resources/test-files/tests/0x041f5aac");

    EXPECT_EQ(fileReader.tell(), 0);
    fileReader.advance(1);
    EXPECT_EQ(fileReader.tell(), 1);
}
