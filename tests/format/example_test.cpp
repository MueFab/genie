#include <iostream>

#include "gtest/gtest.h"


class example_test : public ::testing::Test
{
 protected:
    virtual void SetUp()
    {
        // Code here will be called immediately before each test
    }

    virtual void TearDown()
    {
        // Code here will be called immediately after each test
    }
};


TEST_F(example_test, functionalityUnderTest)
{
    EXPECT_NO_THROW(std::cout << "");
    EXPECT_EQ(0, 0);
}
