#include "calq/file-writer.h"
#include <gtest/gtest.h>
#include "calq/file-reader.h"
#include "helpers.h"

TEST(FileWriter, Constructor) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileWriter fileWriter(gitRootDir + "/FileWriterConstructorTestFile");
}

TEST(FileWriter, WriteValue) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileWriter fileWriter(gitRootDir + "/FileWriterWriteValueTestFile");

    // Write a byte
    uint8_t byte = 0x42;
    size_t ret = fileWriter.writeValue(&byte);
    EXPECT_EQ(ret, sizeof(byte));

    // Close the file in the file writer and open it with a file reader
    EXPECT_NO_THROW(fileWriter.close());
    calq::FileReader fileReader(gitRootDir + "/FileWriterWriteValueTestFile");

    // Read the byte back in
    ret = fileReader.readValue(&byte);
    EXPECT_EQ(ret, sizeof(byte));
    EXPECT_EQ(byte, 0x42);
}

TEST(FileWriter, Write) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileWriter fileWriter(gitRootDir + "/FileWriterWriteTestFile");

    // Write 4 bytes
    const size_t writeBufferSize = 4;
    auto *writeBuffer = reinterpret_cast<uint8_t *>(malloc(sizeof(uint8_t) * writeBufferSize));
    writeBuffer[0] = 0x04;
    writeBuffer[1] = 0x1f;
    writeBuffer[2] = 0x5a;
    writeBuffer[3] = 0xac;
    size_t ret = fileWriter.write(writeBuffer, writeBufferSize);
    EXPECT_EQ(ret, writeBufferSize);
    free(writeBuffer);

    // Close the file in the file writer and open it with a file reader
    EXPECT_NO_THROW(fileWriter.close());
    calq::FileReader fileReader(gitRootDir + "/FileWriterWriteTestFile");

    // Read the bytes back in
    const size_t readBufferSize = 4;
    auto *readBuffer = reinterpret_cast<uint8_t *>(malloc(sizeof(uint8_t) * readBufferSize));
    EXPECT_NE(readBuffer, nullptr);
    ret = fileReader.read(readBuffer, readBufferSize);
    EXPECT_EQ(ret, readBufferSize);
    EXPECT_EQ(readBuffer[0], 0x04);
    EXPECT_EQ(readBuffer[1], 0x1f);
    EXPECT_EQ(readBuffer[2], 0x5a);
    EXPECT_EQ(readBuffer[3], 0xac);
    free(readBuffer);
}

TEST(FileWriter, WriteUintX) {  // NOLINT(cert-err58-cpp)
    std::string gitRootDir = calq_tests::exec("git rev-parse --show-toplevel");
    calq::FileWriter fileWriter(gitRootDir + "/FileWriterWriteUintXTestFile");

    uint8_t byte = 0x42;
    uint16_t word = 0x4242;
    uint32_t dword = 0x42424242;
    uint64_t qword = 0x4242424242424242;

    EXPECT_EQ(fileWriter.writeUint8(byte), sizeof(byte));
    EXPECT_EQ(fileWriter.writeUint16(word), sizeof(word));
    EXPECT_EQ(fileWriter.writeUint32(dword), sizeof(dword));
    EXPECT_EQ(fileWriter.writeUint64(qword), sizeof(qword));
}
