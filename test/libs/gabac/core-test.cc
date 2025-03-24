/*class CoreTest : public ::testing::Test {
   public:
    constexpr static unsigned int params[6] = {1, 1, 0, 0, 1, 1};
};

constexpr unsigned int CoreTest::params[];

TEST_F(CoreTest, encode) {
    std::vector<genie::util::DataBlock> symbols = {genie::util::DataBlock(0, 8),
genie::util::DataBlock(0, 8)}; symbols[1] = {1, 2, 3}; genie::util::DataBlock
bitstream; bitstream = {1, 3, 4};

    // Check parameter lengths
    for (auto& s : symbols) {
        for (unsigned int b = 0; b <
unsigned(genie::entropy::gabac::BinarizationId::STEG) + 1u; ++b) {
            std::vector<unsigned int> binpam;
            if (params[b] > 0) {
                binpam.resize(params[b] - 1u, 1);
                EXPECT_DEATH(genie::entropy::gabac::encode_cabac(
                                 genie::entropy::gabac::BinarizationId(b),
binpam, genie::entropy::gabac::ContextSelectionId::adaptive_coding_order_0, &s),
                             "");
            }
        }
    }
}

TEST_F(CoreTest, roundTrip) {
    std::vector<std::vector<unsigned int>> binarizationParameters = {{32}, {32},
{}, {}, {32}, {32}};

    std::vector<std::vector<int64_t>> intervals = {{0, 4294967295LL}, {0, 32},
{0, 32767},
                                                   {-16383, 16384},   {0, 1},
{0, 1}};

    std::vector<std::string> binNames = {"BI", "TU", "EG", "SEG", "TEG",
"STEG"};

    std::vector<std::string> ctxNames = {"bypass", "order0", "order1",
"order2"};

    genie::util::DataBlock bitstream;
    genie::util::DataBlock decodedSymbols(0, 8);

    // Roundtrips
    for (int c = 0; c < 4; ++c) {
        for (int b = 0; b < 6; ++b) {
            genie::util::DataBlock ran(1024, 8);
            gabac_tests::fillVectorRandomUniform(intervals[b][0],
intervals[b][1], &ran); genie::util::DataBlock sym(ran); std::cout << "--->
Testing binarization " + binNames[b] + " and context selection " + ctxNames[c] +
"..."
                      << std::endl;
            EXPECT_NO_THROW(genie::entropy::gabac::encode_cabac(genie::entropy::gabac::BinarizationId(b),
                                                                binarizationParameters[b],
                                                                genie::entropy::gabac::ContextSelectionId(c),
&sym)); EXPECT_NO_THROW(genie::entropy::gabac::decode_cabac(
                genie::entropy::gabac::BinarizationId(b),
binarizationParameters[b], genie::entropy::gabac::ContextSelectionId(c),
ran.getWordSize(), &sym)); EXPECT_EQ(sym, ran);
        }
    }
}*/
