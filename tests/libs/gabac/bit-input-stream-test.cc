#include <gabac/bit-input-stream.h>
#include <gabac/data-block.h>
#include <gtest/gtest.h>

TEST(BitInputStreamTest, readByte) {  // NOLINT(cert-err58-cpp)
    gabac::DataBlock bitstream(0, 1);
    bitstream = {0xFF};
    gabac::BitInputStream bitInputStream(&bitstream);
    EXPECT_EQ(0xFF, bitInputStream.readByte());
}
