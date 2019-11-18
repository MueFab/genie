#include <gtest/gtest.h>
#include <util/bitwriter.h>

TEST(BitOutputStreamTest, write) {  // NOLINT(cert-err58-cpp)
    std::stringstream str;
    util::BitWriter bitOutputStream(&str);
    EXPECT_NO_THROW(bitOutputStream.write(0xFF, 8));
    EXPECT_NO_THROW(bitOutputStream.write(0xFF, 16));
    EXPECT_NO_THROW(bitOutputStream.write(0xFF, 32));
    EXPECT_NO_THROW(bitOutputStream.write(0xFF, 2));
}

TEST(BitOutputStreamTest, writeAlignZero) {  // NOLINT(cert-err58-cpp)
    std::stringstream str;
    util::BitWriter bitOutputStream(&str);
    bitOutputStream.write(0xFF, 2);
    EXPECT_NO_THROW(bitOutputStream.flush());
    EXPECT_NO_THROW(bitOutputStream.flush());
}
