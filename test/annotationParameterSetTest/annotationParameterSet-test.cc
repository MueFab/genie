#include <gtest/gtest.h>

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include "RandomRecordFillIn.h"
#include "genie/core/record/annotation_parameter_set/record.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

#define GENERATE_TEST_FILES false

class AnnotationParameterSetTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    AnnotationParameterSetTests() = default;

    ~AnnotationParameterSetTests() override = default;

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
        srand(static_cast<unsigned int>(time(NULL)));
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }
    // void sharedSubroutine() {
    //    // If needed, define subroutines for your tests to share
    // }
};


TEST_F(AnnotationParameterSetTests, annotationParameterSetZeros) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    genie::core::record::annotation_parameter_set::Record record;

    EXPECT_EQ(record.getParameterSetID(), (uint8_t)0);
    EXPECT_EQ(record.getATID(), 0);
    EXPECT_EQ(record.getATAlphbetID(), 0);
    EXPECT_EQ(record.getATCoordSize(), 0);
    EXPECT_FALSE(record.isATPos$0Bits());
    EXPECT_EQ(record.getNumberOfAuxAttributeGroups(), 0);
}

TEST_F(AnnotationParameterSetTests, AnnotationParameterSetRandom) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    RandomAnnotationEncodingParameters RandomContactMatrixParameters;
    genie::core::record::annotation_parameter_set::Record annotationParameterSet;
    genie::core::record::annotation_parameter_set::Record annotationParameterSetCheck;

    annotationParameterSet = RandomContactMatrixParameters.randomAnnotationParameterSet();

    std::stringstream InOut;
    genie::util::BitWriter strwriter(&InOut);
    genie::util::BitReader strreader(InOut);
    annotationParameterSet.write(strwriter);
    strwriter.flush();
    annotationParameterSetCheck.read(strreader);

#if GENERATE_TEST_FILES
    std::string name = "TestFiles/AnnotationParameterSet_seed_";
    name += std::to_string(rand() % 10);

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::util::BitWriter writer(&outputfile);
        annotationParameterSet.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        annotationParameterSet.write(txtfile);
        txtfile.close();
    }
#endif
}
