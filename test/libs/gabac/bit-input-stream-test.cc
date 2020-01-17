#include <genie/entropy/gabac/bit-input-stream.h>
#include <gtest/gtest.h>
#include <genie/util/data-block.h>

TEST(BitInputStreamTest, readByte) {  // NOLINT(cert-err58-cpp)
    genie::util::DataBlock bitstream(0, 1);
    bitstream = {0xFF};
    genie::entropy::gabac::BitInputStream bitInputStream(&bitstream);
    EXPECT_EQ(0xFF, bitInputStream.readByte());
}
