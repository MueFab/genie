#include <gtest/gtest.h>
#include <fstream>
#include <iostream>

#include "random_record_fill_in.h"
#include "genie/core/parameter/annotation/tile_configuration.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
// ---------------------------------------------------------------------------------------------------------------------
#define GENERATE_TEST_FILES false

class TileConfigurationTests : public ::testing::Test {
 protected:
    // Do any necessary setup for your tests here
    TileConfigurationTests() = default;

    ~TileConfigurationTests() override = default;

    void SetUp() override {
        srand(static_cast<unsigned int>(time(NULL)));
        // Code here will be called immediately before each test
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }
};

TEST_F(TileConfigurationTests, TileConfigurationZeros) {  // NOLINT(cert-err58-cpp)
    genie::core::parameter::annotation::TileConfiguration tileConfiguration;

    EXPECT_EQ(tileConfiguration.getAttributeGroupClass(), 0);
    EXPECT_EQ(tileConfiguration.getAttributeIDs().size(), 0);
}

TEST_F(TileConfigurationTests, TileConfigurationRandom) {  // NOLINT(cert-err58-cpp)
    RandomAnnotationEncodingParameters randomTileConfiguration;
    uint8_t AT_coord_size = static_cast<uint8_t>(rand() % 4);
    genie::core::parameter::annotation::TileConfiguration tileConfiguration(AT_coord_size);
    genie::core::parameter::annotation::TileConfiguration tileConfigurationCheck(AT_coord_size);

    tileConfiguration = randomTileConfiguration.randomTileConfiguration(AT_coord_size);
    std::stringstream InOut;
    genie::util::BitWriter strwriter(InOut);
    genie::util::BitReader strreader(InOut);
    tileConfiguration.write(strwriter);
    strwriter.FlushBits();
    tileConfigurationCheck.read(strreader);
    std::stringstream CheckOut;
    genie::util::BitWriter checkWriter(CheckOut);
    tileConfigurationCheck.write(checkWriter);
    checkWriter.FlushBits();

    std::stringstream nullStream;
    genie::util::BitWriter writeSize(nullStream);
    auto size = tileConfiguration.getSize(writeSize);
    if (size % 8 != 0) size += (8 - size % 8);
    EXPECT_EQ(InOut.str().size(), size / 8);

    EXPECT_EQ(tileConfiguration.isAttributeContiguity(), tileConfigurationCheck.isAttributeContiguity());
    EXPECT_EQ(tileConfiguration.isTwoDimensional(), tileConfigurationCheck.isTwoDimensional());
    EXPECT_EQ(tileConfiguration.isColumnMajorTileOrder(), tileConfigurationCheck.isColumnMajorTileOrder());
    EXPECT_EQ(tileConfiguration.isSymmetryMinorDiagonal(), tileConfigurationCheck.isSymmetryMinorDiagonal());
    EXPECT_EQ(tileConfiguration.getAttributeGroupClass(), tileConfigurationCheck.getAttributeGroupClass());
    EXPECT_EQ(tileConfiguration.getSymmetryMode(), tileConfigurationCheck.getSymmetryMode());

    EXPECT_EQ(tileConfiguration.getAttributeIDs().size(), tileConfigurationCheck.getAttributeIDs().size());
    EXPECT_EQ(tileConfiguration.getNumberOfAttributes().size(), tileConfigurationCheck.getNumberOfAttributes().size());
    EXPECT_EQ(tileConfiguration.getDescriptorIDs().size(), tileConfigurationCheck.getDescriptorIDs().size());

    EXPECT_EQ(tileConfiguration.getAttributeIDs(), tileConfigurationCheck.getAttributeIDs());
    EXPECT_EQ(tileConfiguration.getNumberOfAttributes(), tileConfigurationCheck.getNumberOfAttributes());
    EXPECT_EQ(tileConfiguration.getDescriptorIDs(), tileConfigurationCheck.getDescriptorIDs());
}

TEST_F(TileConfigurationTests, TileConfigurationRandomSimpleStructure) {  // NOLINT(cert-err58-cpp)
    RandomAnnotationEncodingParameters randomTileConfiguration;
    genie::core::parameter::annotation::TileConfiguration tileConfiguration;
    uint8_t ATCoordSize = static_cast<uint8_t>(rand() % 4);
    tileConfiguration = randomTileConfiguration.randomTileConfiguration(ATCoordSize);
    genie::core::parameter::annotation::TileConfiguration tileConfigurationCheck;

    std::stringstream InOut;
    genie::util::BitWriter strwriter(InOut);
    genie::util::BitReader strreader(InOut);
    tileConfiguration.write(strwriter);
    strwriter.FlushBits();
    tileConfigurationCheck.read(strreader);

    std::stringstream nullStream;
    genie::util::BitWriter writeSize(nullStream);
    auto size = tileConfiguration.getSize(writeSize);
    if (size % 8 != 0) size += (8 - size % 8);
    EXPECT_EQ(InOut.str().size(), size / 8);

    EXPECT_EQ(tileConfiguration.getAttributeGroupClass(), tileConfigurationCheck.getAttributeGroupClass());
    EXPECT_EQ(tileConfiguration.isAttributeContiguity(), tileConfigurationCheck.isAttributeContiguity());
    EXPECT_EQ(tileConfiguration.isTwoDimensional(), tileConfigurationCheck.isTwoDimensional());
}
