#include <gtest/gtest.h>
#include <coding/gabac-compressor.h>
#include "helpers.h"

class GabacCompressorTest : public ::testing::Test {
protected:
    // Do any necessary setup for your tests here
    GabacCompressorTest() = default;

    ~GabacCompressorTest() override = default;

    void SetUp() override {
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }

    // void sharedSubroutine() {
    //    // If needed, define subroutines for your tests to share
    // }
};

TEST(GabacCompressorTest, NonEmpty) {  // NOLINT(cert-err58-cpp)
// The rule of thumb is to use EXPECT_* when you want the test to continue
// to reveal more errors after the assertion failure, and use ASSERT_*
// when continuing after failure doesn't make sense.
EXPECT_EQ(0, 0);
ASSERT_EQ(0, 0);
}