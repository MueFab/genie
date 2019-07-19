#include <algorithm>
#include <functional>
#include <iterator>
#include <iostream>
#include <limits>
#include <vector>
#include <random>

#include "gabac/constants.h"
#include "gabac/data-block.h"
#include "gabac/decode-cabac.h"
#include "gabac/encode-cabac.h"

#include "./test_common.h"

#include "gtest/gtest.h"


class coreTest : public ::testing::Test
{
 protected:
    void SetUp() override{
    }

    void TearDown() override{
        // Code here will be called immediately after each test
    }

 public:
    constexpr static unsigned int params[6] = {1, 1, 0, 0, 1, 1};
};

constexpr unsigned int coreTest::params[];

TEST_F(coreTest, encode){
    std::vector<gabac::DataBlock> symbols = {gabac::DataBlock(0, 8), gabac::DataBlock(0, 8)};
    symbols[1] = {1, 2, 3};
    gabac::DataBlock bitstream;
    bitstream = {1, 3, 4};

    // Check parameter lengths
    for (auto& s : symbols) {
        for (unsigned int b = 0; b < unsigned(gabac::BinarizationId::STEG) + 1u; ++b) {
            std::vector<unsigned int> binpam;
            if (params[b] > 0) {
                binpam.resize(params[b] - 1u, 1);
                EXPECT_DEATH(gabac::encode_cabac(
                        gabac::BinarizationId(b),
                        binpam,
                        gabac::ContextSelectionId::adaptive_coding_order_0,
                        &s
                ), "");
            }
        }
    }
}


TEST_F(coreTest, roundTrip){
    std::vector<std::vector<unsigned int>> binarizationParameters = {
            {32},
            {32},
            {},
            {},
            {32},
            {32}
    };

    std::vector<std::vector<int64_t>> intervals = {{0,      4294967295LL},
                                                   {0,      32},
                                                   {0,      32767},
                                                   {-16383, 16384},
                                                   {0,      1},
                                                   {0,      1}};

    std::vector<std::string> binNames = {"BI",
                                         "TU",
                                         "EG",
                                         "SEG",
                                         "TEG",
                                         "STEG"};

    std::vector<std::string> ctxNames = {"bypass",
                                         "order0",
                                         "order1",
                                         "order2"};

    gabac::DataBlock bitstream;
    gabac::DataBlock decodedSymbols(0, 8);

    // Roundtrips
    for (int c = 0; c < 4; ++c) {
        for (int b = 0; b < 6; ++b) {
            gabac::DataBlock ran(1024, 8);
            fillVectorRandomUniform(intervals[b][0], intervals[b][1], &ran);
            gabac::DataBlock sym(ran);
            std::cout
                    << "---> Testing binarization " + binNames[b] + " and context selection " + ctxNames[c] + "..."
                    << std::endl;
            EXPECT_NO_THROW(gabac::encode_cabac(
                    gabac::BinarizationId(b),
                    binarizationParameters[b],
                    gabac::ContextSelectionId(c),
                    &sym
            ));
            EXPECT_NO_THROW(gabac::decode_cabac(
                    gabac::BinarizationId(b),
                    binarizationParameters[b],
                    gabac::ContextSelectionId(c),
                    ran.getWordSize(),
                    &sym
            ));
            EXPECT_EQ(sym, ran);
        }
    }
}
