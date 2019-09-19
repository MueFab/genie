#include "calq/file-reader.h"
#include <gtest/gtest.h>
#include "helpers.h"

TEST(FileReader, Constructor) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileReader fileReader(gitRootDir + "/resources/test-files/tests/0x041f5aac");
}

TEST(FileReader, ReadValue) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileReader fileReader(gitRootDir + "/resources/test-files/tests/0x041f5aac");

    // Read the first byte
    uint8_t byte = 0x00;
    size_t ret = fileReader.readValue(&byte);
    EXPECT_EQ(ret, sizeof(byte));
    EXPECT_EQ(byte, 0x04);

    // Read the next 2 bytes as uint16_t
    uint16_t word = 0x0000;
    ret = fileReader.readValue(&word);
    EXPECT_EQ(ret, sizeof(word));
    EXPECT_EQ(word, 0x5a1f);

    // Read the last byte as uint8_t
    byte = 0x00;
    ret = fileReader.readValue(&byte);
    EXPECT_EQ(ret, sizeof(byte));
    EXPECT_EQ(byte, 0xac);
}

TEST(FileReader, ReadValueAndSeekAndTellAndEof) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileReader fileReader(gitRootDir + "/resources/test-files/tests/0x041f5aac");

    // Check that the file position indicator points to the beginning of the file
    EXPECT_EQ(fileReader.tell(), 0);

    // Read the first byte
    uint8_t byte = 0x00;
    size_t ret = fileReader.readValue(&byte);
    EXPECT_EQ(ret, sizeof(byte));
    EXPECT_EQ(byte, 0x04);

    // Check the file position indicator
    EXPECT_EQ(fileReader.tell(), 1);

    // Seek past the next byte
    fileReader.seekFromCur(1);

    // Read the next (i.e., last) 2 bytes as uint16_t
    uint16_t word = 0x0000;
    ret = fileReader.readValue(&word);
    EXPECT_EQ(ret, sizeof(word));
    EXPECT_EQ(word, 0xac5a);

    // The EOF flag should not be set yet
    EXPECT_EQ(fileReader.eof(), false);

    // Try to read beyond the EOF
    byte = 0x00;
    ret = fileReader.readValue(&byte);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(byte, 0x00);

    // Now we expect eof() to yield true
    EXPECT_EQ(fileReader.eof(), true);
}

TEST(FileReader, Read) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileReader fileReader(gitRootDir + "/resources/test-files/tests/0x041f5aac");

    // Read all 4 bytes
    const size_t bufferSize = 4;
    auto *buffer = reinterpret_cast<uint8_t *>(malloc(sizeof(uint8_t) * bufferSize));
    EXPECT_NE(buffer, nullptr);
    size_t ret = fileReader.read(buffer, bufferSize);
    EXPECT_EQ(ret, bufferSize);
    EXPECT_EQ(buffer[0], 0x04);
    EXPECT_EQ(buffer[1], 0x1f);
    EXPECT_EQ(buffer[2], 0x5a);
    EXPECT_EQ(buffer[3], 0xac);
    free(buffer);
}

TEST(FileReader, ReadUint8) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileReader fileReader(gitRootDir + "/resources/test-files/tests/0x041f5aac");

    // Read a uint8_t
    uint8_t byte = 0x00;
    size_t ret = fileReader.readUint8(&byte);
    EXPECT_EQ(ret, sizeof(byte));
    EXPECT_EQ(byte, 0x04);
}

TEST(FileReader, ReadUint16) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileReader fileReader(gitRootDir + "/resources/test-files/tests/0x041f5aac");

    // Read a uint16_t
    uint16_t word = 0x0000;
    size_t ret = fileReader.readUint16(&word);
    EXPECT_EQ(ret, sizeof(word));
    EXPECT_EQ(word, 0x1f04);
}

TEST(FileReader, ReadUint32) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileReader fileReader(gitRootDir + "/resources/test-files/tests/0x041f5aac");

    // Read a uint32_t
    uint32_t dword = 0x00000000;
    size_t ret = fileReader.readUint32(&dword);
    EXPECT_EQ(ret, sizeof(dword));
    EXPECT_EQ(dword, 0xac5a1f04);
}

TEST(FileReader, ReadUint64) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileReader fileReader(gitRootDir + "/resources/test-files/tests/0x041f5aac041f5aac");

    // Read a uint64_t
    uint64_t qword = 0x0000000000000000;
    size_t ret = fileReader.readUint64(&qword);
    EXPECT_EQ(ret, sizeof(qword));
    EXPECT_EQ(qword, 0xac5a1f04ac5a1f04);
}
