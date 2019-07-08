#include <vector>

#include <gtest/gtest.h>

#include <gabac/binary_arithmetic_decoder.cpp>
#include <gabac/context_tables.h>
#include <gabac/context_selector.h>
#include <gabac/data_block.h>

class BinaryArithmeticDecoderTest : public ::testing::Test {
protected:
    BinaryArithmeticDecoderTest() = default;

    ~BinaryArithmeticDecoderTest() override = default;

    void SetUp() override {
        // Code here will be called immediately before each test
        bitStream_ = {'0', '1', '2', '3', '4', '5', '6', '7'};
        contextModels_ = gabac::contexttables::buildContextTable();
    }

    void TearDown() override {
        // Code here will be called immediately after each test
    }

    gabac::DataBlock bitStream_;
    std::vector<gabac::ContextModel> contextModels_;
};


TEST_F(BinaryArithmeticDecoderTest, decodeBinsEP) { // NOLINT(cert-err58-cpp)
    gabac::DataBlock tmp = bitStream_;
    gabac::BitInputStream bitInputStream(&tmp);
    gabac::BinaryArithmeticDecoder bad(bitInputStream);

    EXPECT_EQ(48, bad.decodeBinsEP(8));
    EXPECT_EQ(97, bad.decodeBinsEP(8));
    EXPECT_EQ(2479291184, bad.decodeBinsEP(32));
    EXPECT_ANY_THROW(bad.decodeBinsEP(8));
}

TEST_F(BinaryArithmeticDecoderTest, decodeBin) { // NOLINT(cert-err58-cpp)
    gabac::DataBlock tmp = bitStream_;
    gabac::BitInputStream bitInputStream(&tmp);
    gabac::BinaryArithmeticDecoder bad(bitInputStream);

    unsigned int cm = gabac::ContextSelector::getContextForBi(0, 0);

    // Byte 1
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));

    // Byte 2
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));

    // Byte 3
    EXPECT_EQ(1, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));

    // Byte 4
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));

    // Byte 5
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(1, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));

    // Byte 6
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(1, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));

    // Byte 7
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(1, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));

    // Byte 8
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(1, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
    EXPECT_EQ(0, bad.decodeBin(&contextModels_[cm++]));
}

TEST_F(BinaryArithmeticDecoderTest, decodeBinTrm) { // NOLINT(cert-err58-cpp)
    gabac::DataBlock tmp = bitStream_;
    gabac::BitInputStream bitInputStream(&tmp);
    gabac::BinaryArithmeticDecoder bad(bitInputStream);

    EXPECT_NO_THROW(bad.decodeBinTrm());
}
