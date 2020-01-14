#ifndef GENIE_DATE_H
#define GENIE_DATE_H

#include <cstdint>
#include <cstdlib>
#include <string>
#include "exceptions.h"
#include "tokenize.h"

namespace util {

class Date {
   private:
    uint16_t year;
    uint8_t month;
    uint8_t day;
    bool timePresent;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    bool checkValidity() const;

   public:
    static std::string getRegex();
    uint16_t getYear() const;
    uint8_t getMonth() const;
    uint8_t getDay() const;
    bool isTimePresent() const;
    uint8_t getHour() const;
    uint8_t getMinute() const;
    uint8_t getSecond() const;

    bool leapYear() const;

    static Date fromString(const std::string& string);

    Date(uint16_t _year, uint8_t _month, uint8_t _day);

    Date(uint16_t _year, uint8_t _month, uint8_t _day, uint8_t _hour, uint8_t _minute, uint8_t _second);
    std::string toString() const;
};

}  // namespace util

#endif  // GENIE_DATE_H
