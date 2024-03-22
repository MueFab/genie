#include <gtest/gtest.h>

class ExampleTestCase : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    ExampleTestCase() = default;

    ~ExampleTestCase() override = default;

    // Use SetUp instead of the constructor in the following cases:
    // - In the body of a constructor (or destructor), it's not possible to
    //   use the ASSERT_xx macros. Therefore, if the set-up operation could
    //   cause a fatal test failure that should prevent the test from running,
    //   it's necessary to use a CHECK macro or to use SetUp() instead of a
    //   constructor.
    // - If the tear-down operation could throw an exception, you must use
    //   TearDown() as opposed to the destructor, as throwing in a destructor
    //   leads to undefined behavior and usually will kill your program right
    //   away. Note that many standard libraries (like STL) may throw when
    //   exceptions are enabled in the compiler. Therefore you should prefer
    //   TearDown() if you want to write portable tests that work with or
    //   without exceptions.
    // - The googletest team is considering making the assertion macros throw
    //   on platforms where exceptions are enabled (e.g. Windows, Mac OS, and
    //   Linux client-side), which will eliminate the need for the user to
    //   propagate failures from a subroutine to its caller. Therefore, you
    //   shouldn't use googletest assertions in a destructor if your code
    //   could run on such a platform.
    // - In a constructor or destructor, you cannot make a virtual function
    //   call on this object. (You can call a method declared as virtual, but
    //   it will be statically bound.) Therefore, if you need to call a method
    //   that will be overridden in a derived class, you have to use
    //   SetUp()/TearDown().

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


struct info {
    std::string str;
    uint8_t type;
    std::vector<std::vector<uint8_t>> values;
};
struct MoreInfo {
    std::vector<info> allInfo;
    void add(info Info) { allInfo.push_back(Info); }
};

TEST_F(ExampleTestCase, ExampleTest) {
    info info1{ "AC", 8, {{9,0,0,0}} };
    info info2{ "AF", 11, {{75,89,134,59}} };
    info info3{ "AFR_AF", 11, {{10,215,35,60}} };
    info info4{ "CIEND" , 5, {{185,225},{82,0}} };
    MoreInfo infos;

    {
        infos.add(info1);
        infos.add(info2);
        infos.add(info3);
        infos.add(info2);
        infos.add(info4);
    }

    EXPECT_EQ(infos.allInfo.size(), 5);
}