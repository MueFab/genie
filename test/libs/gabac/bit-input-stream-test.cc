#include <gabac/bit-input-stream.h>
#include <gtest/gtest.h>
#include <util/data-block.h>

TEST(BitInputStreamTest, readByte) {  // NOLINT(cert-err58-cpp)
    util::DataBlock bitstream(0, 1);
    bitstream = {0xFF};
    gabac::BitInputStream bitInputStream(&bitstream);
    EXPECT_EQ(0xFF, bitInputStream.readByte());
}
