#include <gtest/gtest.h>

#include <iostream>
#include <fstream>
#include "genie/util/bitreader.h"
#include "genie/core/record/tile_configuration/record.h"

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
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }

    // void sharedSubroutine() {
    //    // If needed, define subroutines for your tests to share
    // }
};




TEST_F(TileConfigurationTests, ConstructAllZeros) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    genie::core::record::tile_configuration::Record tileConfigurationRecord;
    EXPECT_EQ(tileConfigurationRecord.getAttributeGroupClass(), 0);
    EXPECT_FALSE(tileConfigurationRecord.isAttributeContiguity());
    EXPECT_FALSE(tileConfigurationRecord.isTwoDimensional());
    EXPECT_FALSE(tileConfigurationRecord.isColumnMajorTileOrder());
    EXPECT_EQ(tileConfigurationRecord.getSymetryMode(), 0);
    EXPECT_FALSE(tileConfigurationRecord.isSymetryMinorDiagonal());

    EXPECT_FALSE(tileConfigurationRecord.isAttrributeDependentTiles() );

    EXPECT_FALSE(tileConfigurationRecord.getDefaultTileStructure().isVariableSizeTiles() );
    EXPECT_EQ(tileConfigurationRecord.getDefaultTileStructure().getNumberOfTiles(), 0);
    EXPECT_EQ(tileConfigurationRecord.getDefaultTileStructure().getAllStartIndices().size(), 0);
    EXPECT_EQ(tileConfigurationRecord.getDefaultTileStructure().getALLEndIndices().size(), 0);
    EXPECT_EQ(tileConfigurationRecord.getDefaultTileStructure().getAllTileSizes().size(), 0);

    EXPECT_EQ(tileConfigurationRecord.getNumberOfAttributes().size(), 0);
    EXPECT_EQ(tileConfigurationRecord.getAttributeIDs().size(), 0);
    EXPECT_EQ(tileConfigurationRecord.getNumberOfDescriptors().size(), 0);
    EXPECT_EQ(tileConfigurationRecord.getDescriptorIDs().size(), 0);
}

TEST_F(TileConfigurationTests, ConstructWithValues) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    uint8_t AG_class = 1;
    bool attribute_contiguity = true;
    bool two_dimensional = true;

    bool column_major_tile_order = true;
    uint8_t symmetry_mode = 1;
    bool symmetry_minor_diagonal = true;

    bool attribute_dependent_tiles = true;
    genie::core::record::tile_configuration::TileStructure default_tile_structure(true, 1, {{5, 6}, {7, 8}},
                                                                                  {{9,10}, {11, 12}}, {2, 3, 4});
    uint16_t n_add_tile_structures = 1;
    std::vector<uint16_t> n_attribute = {1, 2, 3};
    std::vector<std::vector<uint16_t>> attribute_ID = {{1}, {2}, {3}};
    std::vector<uint8_t> n_descriptors = {1, 2};
    std::vector<std::vector<uint8_t>> descriptor_ID = {{1,3}, {2,2}, {3,1}};
    
    std::vector<genie::core::record::tile_configuration::TileStructure> additional_tile_structure;

    genie::core::record::tile_configuration::Record tileConfigurationRecord(
        AG_class, attribute_contiguity, two_dimensional, column_major_tile_order, symmetry_mode,
        symmetry_minor_diagonal,
        attribute_dependent_tiles, default_tile_structure, n_add_tile_structures, n_attribute, attribute_ID,
        n_descriptors, descriptor_ID, additional_tile_structure);

    EXPECT_EQ(tileConfigurationRecord.getAttributeGroupClass(), AG_class);
    EXPECT_TRUE(tileConfigurationRecord.isAttributeContiguity());
    EXPECT_TRUE(tileConfigurationRecord.isTwoDimensional());
    EXPECT_TRUE(tileConfigurationRecord.isColumnMajorTileOrder());
    EXPECT_EQ(tileConfigurationRecord.getSymetryMode(), symmetry_mode);
    EXPECT_TRUE(tileConfigurationRecord.isSymetryMinorDiagonal());

    EXPECT_TRUE(tileConfigurationRecord.isAttrributeDependentTiles());
    EXPECT_TRUE(tileConfigurationRecord.getDefaultTileStructure().isVariableSizeTiles());

    //EXPECT_EQ(tileConfigurationRecord.getDefaultTileStructure(), default_tile_structure);

    EXPECT_EQ(tileConfigurationRecord.getNumberOfAttributes(), n_attribute);
    EXPECT_EQ(tileConfigurationRecord.getAttributeIDs(), attribute_ID);
    EXPECT_EQ(tileConfigurationRecord.getNumberOfDescriptors(), n_descriptors);
    EXPECT_EQ(tileConfigurationRecord.getDescriptorIDs(), descriptor_ID);



    EXPECT_EQ(0, 0);
    ASSERT_EQ(0, 0);
}
