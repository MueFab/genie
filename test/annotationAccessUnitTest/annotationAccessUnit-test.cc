#include <gtest/gtest.h>

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include "genie/core/constants.h"
#include "genie/core/arrayType.h"
#include "genie/core/record/annotation_access_unit/record.h"
#include "genie/core/writer.h"
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/core/record/variant_site/VariantSiteParser.h"

#define GENERATE_TEST_FILES false

class AnnotationAccessUnitTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    AnnotationAccessUnitTests() = default;

    ~AnnotationAccessUnitTests() override = default;

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

TEST_F(AnnotationAccessUnitTests, annotationAccessUnitHeader) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    bool attributeContiguity = false;
    bool twoDimensional = false;
    bool columnMajorTileOrder = false;
    bool variable_size_tiles = false;

    uint8_t ATCoordSize = 3;
    bool is_attribute = false;
    uint16_t attribute_ID = 0;
  
    genie::core::AnnotDesc descriptor_ID =
        genie::core::AnnotDesc ::DESCRIPTION;
    uint64_t n_tiles_per_col = 0;
    uint64_t n_tiles_per_row = 0;
    uint64_t n_blocks = 1;
    uint64_t tile_index_1 = 0;
    bool tile_index_2_exists = false;
    uint64_t tile_index_2 = 0;

    genie::core::record::annotation_access_unit::AnnotationAccessUnitHeader unitHeader(
        attributeContiguity, twoDimensional, columnMajorTileOrder, variable_size_tiles, ATCoordSize, is_attribute,
        attribute_ID, descriptor_ID, n_tiles_per_col, n_tiles_per_row, n_blocks, tile_index_1, tile_index_2_exists,
        tile_index_2);
    EXPECT_EQ(unitHeader.getNumberOfBlocks(), n_blocks);
    EXPECT_EQ(unitHeader.getDescriptorID(), descriptor_ID);
}
TEST_F(AnnotationAccessUnitTests, annotationAccessUnitHeader2) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
 //   genie::core::record::variant_site::VaritanSiteParser parser;


}