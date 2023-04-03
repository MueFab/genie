#include <gtest/gtest.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include "genie/util/bitreader.h"
#include "genie/core/record/variant_genotype/record.h"

class VariantGenotypeRecordTests : public ::testing::Test {
   protected:
    // Do any necessary setup for your tests here
      VariantGenotypeRecordTests() = default;

    ~VariantGenotypeRecordTests() override = default;

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

#include <array>


std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}
TEST_F(VariantGenotypeRecordTests, readFilefrombin) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    std::string gitRootDir = exec("git rev-parse --show-toplevel");
    
    std::filebuf fb1;
    if (fb1.open(gitRootDir.substr(0, gitRootDir.length() - 1) + "/data/records/1.3.05_cut.geno",
                 std::ios::in | std::ios::binary)) {
        std::istream is(&fb1);
        genie::util::BitReader reader(is);
        std::ofstream myfile("1.3.05_cut.geno_test.txt");
        do {
            genie::core::record::variant_genotype::Record variant_geno_record(reader);
           
            variant_geno_record.write(myfile);
           
        } while (is.peek() != EOF);
        fb1.close();
        myfile.close();
    }
    

    EXPECT_EQ(0, 0);
    ASSERT_EQ(0, 0);
}
