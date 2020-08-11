/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <genie/util/stringview.h>
#include <gtest/gtest.h>

// ---------------------------------------------------------------------------------------------------------------------

TEST(StringViewTest, stringview) {
    std::string str = "0123456789";
    genie::util::StringView view(1, 4);
    EXPECT_EQ(view.length(), 3);
    view = view.deploy(str.c_str());
    EXPECT_EQ(view.length(), 3);
    std::string output;
    for(const auto &c : view) {
        output += c;
    }
    EXPECT_EQ(output, "123");
}

// ---------------------------------------------------------------------------------------------------------------------