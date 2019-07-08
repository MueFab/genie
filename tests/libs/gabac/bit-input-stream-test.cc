#include <gtest/gtest.h>

#include <gabac/bit_input_stream.h>
#include <gabac/data_block.h>

TEST(BitInputStreamTest, readByte) { // NOLINT(cert-err58-cpp)
    gabac::DataBlock bitstream(0, 1);
    bitstream = {0xFF};
    gabac::BitInputStream bitInputStream(&bitstream);
    EXPECT_EQ(0xFF, bitInputStream.readByte());
}
