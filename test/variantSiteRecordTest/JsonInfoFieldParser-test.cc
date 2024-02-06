#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include "genie/annotation/JsonAttributeParser.h"
#include "genie/util/bitreader.h"

class JsonInfoFieldParserTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    JsonInfoFieldParserTests() = default;

    ~JsonInfoFieldParserTests() override = default;

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

TEST_F(JsonInfoFieldParserTests, readJsonFile) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    std::string path = "./Testfiles/exampleMGrecs/";
    std::string filename = "ALL.chrX.phase1_release_v3.20101123.snps_indels_svs.genotypes.vcf.infotags.json";
    std::ifstream inputfile;
    std::stringstream infoFieldStream;
    inputfile.open(path + filename, std::ios::in);
    if (inputfile.is_open()) {
        infoFieldStream << inputfile.rdbuf();
        inputfile.close();

    genie::annotation::JsonAttributeParser jsonInfoFields(infoFieldStream);
    EXPECT_GT(jsonInfoFields.getInfoFields().size(), 10);
    EXPECT_EQ(jsonInfoFields.getInfoField(0).ID, "AA");
    EXPECT_EQ(jsonInfoFields.getInfoField(0).Number, 1);
    EXPECT_EQ(jsonInfoFields.getInfoField(0).Type, genie::core::DataType::STRING);
    }
}
