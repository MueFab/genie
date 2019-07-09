#include <gtest/gtest.h>

#include <gabac/bit_output_stream.h>
#include <gabac/data_block.h>

TEST(BitOutputStreamTest, write) { // NOLINT(cert-err58-cpp)
    gabac::DataBlock bitstream(0, 1);
    gabac::BitOutputStream bitOutputStream(&bitstream);
    EXPECT_NO_THROW(bitOutputStream.write(0xFF, 8));
    EXPECT_NO_THROW(bitOutputStream.write(0xFF, 16));
    EXPECT_NO_THROW(bitOutputStream.write(0xFF, 32));
    EXPECT_NO_THROW(bitOutputStream.write(0xFF, 2));
}

TEST(BitOutputStreamTest, writeAlignZero) { // NOLINT(cert-err58-cpp)
    gabac::DataBlock bitstream(0, 1);
    gabac::BitOutputStream bitOutputStream(&bitstream);
    bitOutputStream.write(0xFF, 2);
    EXPECT_NO_THROW(bitOutputStream.writeAlignZero());
    EXPECT_NO_THROW(bitOutputStream.writeAlignZero());
}
