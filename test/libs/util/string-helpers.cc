/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include <genie/util/string-helpers.h>
#include <gtest/gtest.h>

// ---------------------------------------------------------------------------------------------------------------------

TEST(StringHelpersTest, trim) {
    std::string str = "ACABBBACA";
    auto tmp = str;
    EXPECT_EQ(genie::util::ltrim(tmp, "AC"), "BBBACA");
    tmp = str;
    EXPECT_EQ(genie::util::rtrim(tmp, "AC"), "ACABBB");
    tmp = str;
    EXPECT_EQ(genie::util::trim(tmp, "AC"), "BBB");
}

TEST(StringHelpersTest, Tokenizer) {
    std::string str = "Eins    Uno One";
    EXPECT_EQ(genie::util::tokenize(str, ' '), std::vector<std::string>({"Eins", "Uno", "One"}));
    EXPECT_EQ(genie::util::tokenize(str, 'n'), std::vector<std::string>({"Ei", "s    U", "o O", "e"}));
}

// ---------------------------------------------------------------------------------------------------------------------