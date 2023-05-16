
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>

#include "RandomRecordFillIn.h"
#include "genie/core/record/annotation_parameter_set/TileConfiguration.h"
// ---------------------------------------------------------------------------------------------------------------------
#define GENERATE_TEST_FILES false

class TileConfigurationTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    TileConfigurationTests() = default;

    ~TileConfigurationTests() override = default;

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

TEST_F(TileConfigurationTests, TileConfigurationZeros) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    genie::core::record::annotation_parameter_set::TileConfiguration tileConfiguration;

    EXPECT_EQ(tileConfiguration.getAttributeGroupClass(), 0);
    EXPECT_EQ(tileConfiguration.getAttributeIDs().size(), 0);
}

TEST_F(TileConfigurationTests, TileConfigurationRandom) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    RandomAnnotationEncodingParameters randomTileConfiguration;
    genie::core::record::annotation_parameter_set::TileConfiguration tileConfiguration;

    tileConfiguration = randomTileConfiguration.randomTileConfiguration();

#if GENERATE_TEST_FILES
    uint8_t randomOrder = static_cast<uint8_t>(rand() % 10);
    std::string name = "TestFiles/TileConfugration_seed_";
    name += std::to_string(randomOrder);

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::util::BitWriter writer(&outputfile);
        tileConfiguration.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        tileConfiguration.write(txtfile);
        txtfile.close();
    }
#endif
}

TEST_F(TileConfigurationTests, TileConfigurationRandomSimpleStructure) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    RandomAnnotationEncodingParameters randomTileConfiguration;
    genie::core::record::annotation_parameter_set::TileConfiguration tileConfiguration;
    uint8_t ATCoordSize = static_cast<uint8_t>(rand() % 4);
    tileConfiguration = randomTileConfiguration.randomTileConfiguration(ATCoordSize);
    genie::core::record::annotation_parameter_set::TileConfiguration tileConfigurationCheck;

    std::stringstream InOut;
    //(std::stringstream::in | std::stringstream::out | std::stringstream::binary);
    genie::util::BitWriter strwriter(&InOut);
    genie::util::BitReader strreader(InOut);
    tileConfiguration.write(strwriter);
    strwriter.flush();
    tileConfigurationCheck.read(strreader);

    EXPECT_EQ(tileConfiguration.getAttributeGroupClass(), tileConfigurationCheck.getAttributeGroupClass());
    EXPECT_EQ(tileConfiguration.isAttributeContiguity(), tileConfigurationCheck.isAttributeContiguity());
    EXPECT_EQ(tileConfiguration.isTwoDimensional(), tileConfigurationCheck.isTwoDimensional());

#if GENERATE_TEST_FILES
    uint8_t randomOrder = static_cast<uint8_t>(rand() % 10);
    std::string name = "TestFiles/TileConfugrationSimple_seed_";
    name += std::to_string(randomOrder);

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::util::BitWriter writer(&outputfile);
        tileConfiguration.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        txtfile << std::to_string(ATCoordSize) << ",";
        tileConfiguration.write(txtfile);
        txtfile.close();
    }
#endif
}
