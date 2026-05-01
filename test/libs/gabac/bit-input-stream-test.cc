#include <gtest/gtest.h>

#include "genie/entropy/gabac/bit_input_stream.h"
#include "genie/util/data_block.h"

TEST(BitInputStreamTest, readByte) {  // NOLINT(cert-err58-cpp)
  genie::util::DataBlock bitstream(0, 1);
  bitstream = {0xFF};
  genie::entropy::gabac::BitInputStream bit_input_stream(&bitstream);
  EXPECT_EQ(0xFF, bit_input_stream.ReadByte());
}
