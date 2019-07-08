#include <vector>

#include "gabac/bit_output_stream.h"
#include "gabac/data_block.h"

#include "gtest/gtest.h"


class BitOutputStreamTest : public ::testing::Test
{
 protected:
    void SetUp() override{
    }

    void TearDown() override{
        // Code here will be called immediately after each test
    }
};


TEST_F(BitOutputStreamTest, write){
    gabac::DataBlock bitstream(0, 1);
    gabac::BitOutputStream bitOutputStream(&bitstream);
    EXPECT_NO_THROW(bitOutputStream.write(0xFF, 8));
    EXPECT_NO_THROW(bitOutputStream.write(0xFF, 16));
    EXPECT_NO_THROW(bitOutputStream.write(0xFF, 32));
    EXPECT_NO_THROW(bitOutputStream.write(0xFF, 2));
}

TEST_F(BitOutputStreamTest, writeAlignZero){
    gabac::DataBlock bitstream(0, 1);
    gabac::BitOutputStream bitOutputStream(&bitstream);
    bitOutputStream.write(0xFF, 2);
    EXPECT_NO_THROW(bitOutputStream.writeAlignZero());
    EXPECT_NO_THROW(bitOutputStream.writeAlignZero());
}
