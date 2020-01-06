#include <gtest/gtest.h>
#include <coding/gabac-compressor.h>
#include "helpers.h"

class GabacCompressorTest : public ::testing::Test {
protected:
    // Do any necessary setup for your tests here
    GabacCompressorTest() = default;

    ~GabacCompressorTest() override = default;

    void SetUp() override {
        std::string gitRootDir = util_tests::exec("git rev-parse --show-toplevel");
        const size_t RECORDS_PER_BLOCK = 10000;
        std::ifstream myFastqInput(gitRootDir + "/resources/test-files/fastq/minimal.fastq")
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }

    // void sharedSubroutine() {
    //    // If needed, define subroutines for your tests to share
    // }
};

TEST_F(GabacCompressorTest, NonEmpty) {  // NOLINT(cert-err58-cpp)
// The rule of thumb is to use EXPECT_* when you want the test to continue
// to reveal more errors after the assertion failure, and use ASSERT_*
// when continuing after failure doesn't make sense.
EXPECT_EQ(0, 0);
ASSERT_EQ(0, 0);
}