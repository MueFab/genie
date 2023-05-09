
/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */
#include <gtest/gtest.h>

#include "RandomRecordFillIn.h"
#include "genie/core/record/annotation_parameter_set/TileStructure.h"
// ---------------------------------------------------------------------------------------------------------------------

class TileStructureTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    TileStructureTests() = default;

    ~TileStructureTests() override = default;

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

TEST_F(TileStructureTests, TileStructureZeros) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    genie::core::record::annotation_parameter_set::TileStructure tileStructure;

    EXPECT_FALSE(tileStructure.isVariableSizeTiles());
    EXPECT_EQ(tileStructure.getALLEndIndices().size(), 0);
    EXPECT_EQ(tileStructure.getAllStartIndices().size(), 0);
    EXPECT_EQ(tileStructure.getAllTileSizes().size(), 1);
    EXPECT_EQ(tileStructure.getNumberOfTiles(), 0);
}

TEST_F(TileStructureTests, TileStructurevalues) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.
    uint8_t ATCoordSize = 3;
    bool two_dimensional = false;
    bool variable_size_tiles = true;
    uint64_t n_tiles = 4;
    std::vector<std::vector<uint64_t>> start_index{{1}, {2}, {3}, {4}};
    std::vector<std::vector<uint64_t>> end_index{{5}, {6}, {7}, {8}};
    std::vector<uint64_t> tile_size = {8, 16, 25, 64};
    genie::core::record::annotation_parameter_set::TileStructure tileStructure(
        two_dimensional, ATCoordSize, variable_size_tiles, n_tiles, start_index, end_index, tile_size);
    EXPECT_TRUE(tileStructure.isVariableSizeTiles());
    EXPECT_EQ(tileStructure.getALLEndIndices().size(), n_tiles);
    EXPECT_EQ(tileStructure.getAllStartIndices()[3], start_index[3]);
    EXPECT_EQ(tileStructure.getAllTileSizes()[1], tile_size[1]);
    EXPECT_EQ(tileStructure.getNumberOfTiles(), n_tiles);
}

TEST_F(TileStructureTests, TileStructureRandom) {  // NOLINT(cert-err58-cpp)
    // The rule of thumb is to use EXPECT_* when you want the test to continue
    // to reveal more errors after the assertion failure, and use ASSERT_*
    // when continuing after failure doesn't make sense.

    RandomAnnotationEncodingParameters RandomTileStructure;
    genie::core::record::annotation_parameter_set::TileStructure tileStructure;
    uint8_t ATCoordSize = static_cast<uint8_t>(rand() % 4);
    bool two_dimensional = static_cast<bool>(rand() % 2);
    tileStructure = RandomTileStructure.randomTileStructure(ATCoordSize, two_dimensional);

    std::string name = "TestFiles/TileStructure_seed_";
    name += std::to_string(rand() % 10);

#if GENERATE_TEST_FILES

    std::ofstream outputfile;
    outputfile.open(name + ".bin", std::ios::binary | std::ios::out);
    if (outputfile.is_open()) {
        genie::util::BitWriter writer(&outputfile);
        tileStructure.write(writer);
        writer.flush();
        outputfile.close();
    }
    std::ofstream txtfile;
    txtfile.open(name + ".txt", std::ios::out);
    if (txtfile.is_open()) {
        txtfile << std::to_string(two_dimensional) << ",";
        txtfile << std::to_string(ATCoordSize) << ",";
        tileStructure.write(txtfile);
        txtfile.close();
    }
#endif
}
