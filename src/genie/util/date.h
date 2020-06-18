/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DATE_H
#define GENIE_DATE_H

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 *
 */
class Date {
   private:
    uint16_t year; //!<
    uint8_t month; //!<
    uint8_t day; //!<
    bool timePresent; //!<
    uint8_t hour; //!<
    uint8_t minute; //!<
    uint8_t second; //!<

    /**
     *
     * @return
     */
    bool checkValidity() const;

   public:

    /**
     *
     * @return
     */
    static std::string getRegex();

    /**
     *
     * @return
     */
    uint16_t getYear() const;

    /**
     *
     * @return
     */
    uint8_t getMonth() const;

    /**
     *
     * @return
     */
    uint8_t getDay() const;

    /**
     *
     */
    bool isTimePresent() const;

    /**
     *
     * @return
     */
    uint8_t getHour() const;

    /**
     *
     * @return
     */
    uint8_t getMinute() const;

    /**
     *
     * @return
     */
    uint8_t getSecond() const;

    /**
     *
     * @return
     */
    bool leapYear() const;

    /**
     *
     * @param string
     * @return
     */
    static Date fromString(const std::string& string);

    /**
     *
     * @param _year
     * @param _month
     * @param _day
     */
    Date(uint16_t _year, uint8_t _month, uint8_t _day);

    /**
     *
     * @param _year
     * @param _month
     * @param _day
     * @param _hour
     * @param _minute
     * @param _second
     */
    Date(uint16_t _year, uint8_t _month, uint8_t _day, uint8_t _hour, uint8_t _minute, uint8_t _second);

    /**
     *
     * @return
     */
    std::string toString() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DATE_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------