/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include <genie/util/bit-writer.h>
#include <gtest/gtest.h>

// ---------------------------------------------------------------------------------------------------------------------

TEST(BitWriter, write) {  // NOLINT(cert-err58-cpp)
    std::stringstream str;
    genie::util::BitWriter bitOutputStream(str);
    EXPECT_NO_THROW(bitOutputStream.writeBits(0xFF, 8));
    EXPECT_NO_THROW(bitOutputStream.writeBits(0xFF, 16));
    EXPECT_NO_THROW(bitOutputStream.writeBits(0xFF, 32));
    EXPECT_NO_THROW(bitOutputStream.writeBits(0xFF, 2));
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(BitWriter, writeAlignZero) {  // NOLINT(cert-err58-cpp)
    std::stringstream str;
    genie::util::BitWriter bitOutputStream(str);
    bitOutputStream.writeBits(0xFF, 2);
    EXPECT_NO_THROW(bitOutputStream.flushBits());
    EXPECT_NO_THROW(bitOutputStream.flushBits());
}

// ---------------------------------------------------------------------------------------------------------------------