/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/util/date.h"
#include <regex>
#include "genie/util/runtime-exception.h"
#include "genie/util/string-helpers.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::util {

// ---------------------------------------------------------------------------------------------------------------------

bool Date::checkValidity() const {
    size_t days_per_month[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (year < 1900 || year >= 2100) {
        return false;
    }
    if (month < 1 || month > 12) {
        return false;
    }
    if (day < 1 || day > days_per_month[month - 1]) {
        return false;
    }
    if (month == 2 && day == 29 && !leapYear()) {
        return false;
    }
    if (!timePresent) return true;
    if (hour > 23) {
        return false;
    }
    if (minute > 59) {
        return false;
    }
    return second <= 59;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string Date::getRegex() {
    return "^(1|2)(9|0)[0-9]{2}-(0|1)[0-9]-[0-3][0-9](T[0-2][0-9]:[0-5][0-9]:[0-5][0-9])*$";
}

// ---------------------------------------------------------------------------------------------------------------------

uint16_t Date::getYear() const { return year; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Date::getMonth() const { return month; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Date::getDay() const { return day; }

// ---------------------------------------------------------------------------------------------------------------------

bool Date::isTimePresent() const { return timePresent; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Date::getHour() const { return hour; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Date::getMinute() const { return minute; }

// ---------------------------------------------------------------------------------------------------------------------

uint8_t Date::getSecond() const { return second; }

// ---------------------------------------------------------------------------------------------------------------------

bool Date::leapYear() const {
    if (!(year % 400)) {
        return true;
    }
    if (!(year % 100)) {
        return false;
    }
    return !(year % 4);
}

// ---------------------------------------------------------------------------------------------------------------------

Date Date::fromString(const std::string& string) {
    UTILS_DIE_IF(!std::regex_match(string, std::regex(getRegex())), "Invalid date");

    auto date_time = tokenize(string, 'T');
    auto date = tokenize(date_time.front(), '-');
    if (date_time.size() == 1) {
        return {uint16_t(std::stoi(date[0])), uint8_t(std::stoi(date[1])), uint8_t(std::stoi(date[2]))};
    } else if (date_time.size() == 2) {
        auto time = tokenize(date_time.back(), ':');
        return {uint16_t(std::stoi(date[0])), uint8_t(std::stoi(date[1])), uint8_t(std::stoi(date[2])),
                uint8_t(std::stoi(time[0])),  uint8_t(std::stoi(time[1])), uint8_t(std::stoi(time[2]))};
    } else {
        UTILS_DIE("Invalid date");
    }
}

// ---------------------------------------------------------------------------------------------------------------------

Date::Date(uint16_t _year, uint8_t _month, uint8_t _day)
    : year(_year), month(_month), day(_day), timePresent(false), hour(0), minute(0), second(0) {
    UTILS_DIE_IF(!checkValidity(), "Invalid date");
}

// ---------------------------------------------------------------------------------------------------------------------

Date::Date(uint16_t _year, uint8_t _month, uint8_t _day, uint8_t _hour, uint8_t _minute, uint8_t _second)
    : year(_year), month(_month), day(_day), timePresent(true), hour(_hour), minute(_minute), second(_second) {
    UTILS_DIE_IF(!checkValidity(), "Invalid date");
}

// ---------------------------------------------------------------------------------------------------------------------

std::string Date::toString() const {
    std::string ret;
    ret += std::to_string(year);
    ret += '-';
    if (month < 10) {
        ret += '0';
    }
    ret += std::to_string(month);
    ret += '-';
    if (day < 10) {
        ret += '0';
    }
    ret += std::to_string(day);
    if (!timePresent) {
        return ret;
    }
    ret += 'T';
    if (hour < 10) {
        ret += '0';
    }
    ret += std::to_string(hour);
    ret += ':';
    if (minute < 10) {
        ret += '0';
    }
    ret += std::to_string(minute);
    ret += ':';
    if (second < 10) {
        ret += '0';
    }
    ret += std::to_string(second);
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::util

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
