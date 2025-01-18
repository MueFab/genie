/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include <genie/util/string_helpers.h>
#include <gtest/gtest.h>

// -----------------------------------------------------------------------------
TEST(StringHelpersTest, trim) {
  const std::string str = "ACABBBACA";
  auto tmp = str;
  EXPECT_EQ(genie::util::Ltrim(tmp, "AC"), "BBBACA");
  tmp = str;
  EXPECT_EQ(genie::util::Rtrim(tmp, "AC"), "ACABBB");
  tmp = str;
  EXPECT_EQ(genie::util::Trim(tmp, "AC"), "BBB");
}

TEST(StringHelpersTest, Tokenizer) {
  const std::string str = "Eins    Uno One";
  EXPECT_EQ(genie::util::Tokenize(str, ' '),
            std::vector<std::string>({"Eins", "Uno", "One"}));
  EXPECT_EQ(genie::util::Tokenize(str, 'n'),
            std::vector<std::string>({"Ei", "s    U", "o O", "e"}));
}

// -----------------------------------------------------------------------------