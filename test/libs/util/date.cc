/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include <genie/util/date.h>
#include <gtest/gtest.h>

// ---------------------------------------------------------------------------------------------------------------------

TEST(DateTest, LowestDate) {
    std::string date_string("1900-01-01T00:00:00");
    genie::util::Date date = genie::util::Date::fromString(date_string);
    EXPECT_EQ(date.getYear(), 1900);
    EXPECT_EQ(date.getMonth(), 1);
    EXPECT_EQ(date.getDay(), 1);
    EXPECT_EQ(date.getHour(), 0);
    EXPECT_EQ(date.getMinute(), 0);
    EXPECT_EQ(date.getSecond(), 0);
    EXPECT_TRUE(date.isTimePresent());
    EXPECT_EQ(date_string, date.toString());
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(DateTest, HighestDate) {
    std::string date_string("2099-12-31T23:59:59");
    genie::util::Date date = genie::util::Date::fromString(date_string);
    EXPECT_EQ(date.getYear(), 2099);
    EXPECT_EQ(date.getMonth(), 12);
    EXPECT_EQ(date.getDay(), 31);
    EXPECT_EQ(date.getHour(), 23);
    EXPECT_EQ(date.getMinute(), 59);
    EXPECT_EQ(date.getSecond(), 59);
    EXPECT_TRUE(date.isTimePresent());
    EXPECT_EQ(date_string, date.toString());
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(DateTest, AnyDate) {
    std::string date_string("1997-08-12");
    genie::util::Date date = genie::util::Date::fromString(date_string);
    EXPECT_EQ(date.getYear(), 1997);
    EXPECT_EQ(date.getMonth(), 8);
    EXPECT_EQ(date.getDay(), 12);
    EXPECT_FALSE(date.isTimePresent());
    EXPECT_EQ(date_string, date.toString());
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(DateTest, AnyDateTime) {
    std::string date_string("2017-04-12T10:29:39");
    genie::util::Date date = genie::util::Date::fromString(date_string);
    EXPECT_EQ(date.getYear(), 2017);
    EXPECT_EQ(date.getMonth(), 04);
    EXPECT_EQ(date.getDay(), 12);
    EXPECT_EQ(date.getHour(), 10);
    EXPECT_EQ(date.getMinute(), 29);
    EXPECT_EQ(date.getSecond(), 39);
    EXPECT_TRUE(date.isTimePresent());
    EXPECT_EQ(date_string, date.toString());
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(DateTest, InvalidDateFormat) {
    EXPECT_ANY_THROW(genie::util::Date::fromString("1997-0812"));
    EXPECT_ANY_THROW(genie::util::Date::fromString("199A-08-12"));
    EXPECT_ANY_THROW(genie::util::Date::fromString("099-12-31T23:59:59"));
    EXPECT_ANY_THROW(genie::util::Date::fromString("2099-2-31T23:59:59"));
    EXPECT_ANY_THROW(genie::util::Date::fromString("2099-12-1T23:59:59"));
    EXPECT_ANY_THROW(genie::util::Date::fromString("2099-12-31T3:59:59"));
    EXPECT_ANY_THROW(genie::util::Date::fromString("2099-12-31T23:9:59"));
    EXPECT_ANY_THROW(genie::util::Date::fromString("2099-12-31T23:59:9"));
    EXPECT_ANY_THROW(genie::util::Date::fromString("2099-12-31T23:59: 9"));
    EXPECT_ANY_THROW(genie::util::Date::fromString("2017-04-12T10:29:39T10:29:39"));
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(DateTest, InvalidDateSematic) {
    // Field value too high
    EXPECT_ANY_THROW(genie::util::Date::fromString("2099-12-31T23:59:60"));
    EXPECT_ANY_THROW(genie::util::Date::fromString("2099-12-31T23:60:59"));
    EXPECT_ANY_THROW(genie::util::Date::fromString("2099-12-31T24:59:59"));
    EXPECT_ANY_THROW(genie::util::Date::fromString("2099-12-32T23:60:59"));
    EXPECT_ANY_THROW(genie::util::Date::fromString("2099-13-31T23:60:59"));
    EXPECT_ANY_THROW(genie::util::Date::fromString("2100-12-31T23:60:59"));

    // Field value too low
    EXPECT_ANY_THROW(genie::util::Date::fromString("1899-12-31T23:60:59"));
    EXPECT_ANY_THROW(genie::util::Date::fromString("2099-00-31T23:60:59"));
    EXPECT_ANY_THROW(genie::util::Date::fromString("2099-12-00T23:60:59"));
}

// ---------------------------------------------------------------------------------------------------------------------

TEST(DateTest, LeapYears) {
    EXPECT_TRUE(genie::util::Date::fromString("2020-01-01").leapYear());
    EXPECT_FALSE(genie::util::Date::fromString("2023-01-01").leapYear());
    EXPECT_FALSE(genie::util::Date::fromString("1900-01-01").leapYear());
    EXPECT_TRUE(genie::util::Date::fromString("2000-01-01").leapYear());
}

// ---------------------------------------------------------------------------------------------------------------------