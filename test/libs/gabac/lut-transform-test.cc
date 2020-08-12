/*#include <genie/entropy/gabac/lut-transform.h>
#include <genie/util/data-block.h>
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include "common.h"

TEST(LutTransformTest, roundTripCoding0) {
    // Void input
    genie::util::DataBlock symbols(0, 1);
    symbols.resize(1024 * 1024);
    gabac_tests::fillVectorRandomUniform(0, 64, &symbols);
    genie::util::DataBlock transsymbols = symbols;
    genie::util::DataBlock decodedSymbols(0, 1);
    genie::util::DataBlock inverseLut0(0, 1);
    genie::util::DataBlock inverseLut1(0, 1);

    EXPECT_NO_THROW(genie::entropy::gabac::transformLutTransform(0, &transsymbols, &inverseLut0, &inverseLut1));
    decodedSymbols = transsymbols;
    EXPECT_NO_THROW(
        genie::entropy::gabac::inverseTransformLutTransform(0, &decodedSymbols, &inverseLut0, &inverseLut1));
    EXPECT_EQ(decodedSymbols.size(), symbols.size());
    EXPECT_EQ(decodedSymbols, symbols);

    EXPECT_NO_THROW(genie::entropy::gabac::transformLutTransform(1, &transsymbols, &inverseLut0, &inverseLut1));
    EXPECT_NO_THROW(genie::entropy::gabac::inverseTransformLutTransform(1, &transsymbols, &inverseLut0, &inverseLut1));
    EXPECT_EQ(decodedSymbols.size(), symbols.size());
    EXPECT_EQ(decodedSymbols, symbols);

    EXPECT_NO_THROW(genie::entropy::gabac::transformLutTransform(2, &transsymbols, &inverseLut0, &inverseLut1));
    EXPECT_NO_THROW(genie::entropy::gabac::inverseTransformLutTransform(2, &transsymbols, &inverseLut0, &inverseLut1));
    EXPECT_EQ(decodedSymbols.size(), symbols.size());
    EXPECT_EQ(decodedSymbols, symbols);
}
*/