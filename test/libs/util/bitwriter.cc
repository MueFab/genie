/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/util/bit_writer.h"
#include <gtest/gtest.h>

// -----------------------------------------------------------------------------
TEST(BitWriter, write) {  // NOLINT(cert-err58-cpp)
  std::stringstream str;
  genie::util::BitWriter bit_output_stream(str);
  EXPECT_NO_THROW(bit_output_stream.WriteBits(0xFF, 8));
  EXPECT_NO_THROW(bit_output_stream.WriteBits(0xFF, 16));
  EXPECT_NO_THROW(bit_output_stream.WriteBits(0xFF, 32));
  EXPECT_NO_THROW(bit_output_stream.WriteBits(0xFF, 2));
}

// -----------------------------------------------------------------------------
TEST(BitWriter, writeAlignZero) {  // NOLINT(cert-err58-cpp)
  std::stringstream str;
  genie::util::BitWriter bit_output_stream(str);
  bit_output_stream.WriteBits(0xFF, 2);
  EXPECT_NO_THROW(bit_output_stream.FlushBits());
  EXPECT_NO_THROW(bit_output_stream.FlushBits());
}

// -----------------------------------------------------------------------------