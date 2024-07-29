/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_UTIL_DATE_H_
#define SRC_GENIE_UTIL_DATE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <cstdint>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace util {

/**
 * @brief Represents an ISO compliant date and optionally time.
 */
class Date {
 private:
    uint16_t year;     //!< @brief Full year.
    uint8_t month;     //!< @brief ID of month.
    uint8_t day;       //!< @brief Day inside of month. @attention Not all months have the same number of days.
    bool timePresent;  //!< @brief If false, only the date fields are valid.
    uint8_t hour;      //!< @brief 24h hour.
    uint8_t minute;    //!< @brief Minute.
    uint8_t second;    //!< @brief Second.

    /**
     * @brief
     * @return True if the date actually exists (e.g. 30th of february would return false).
     */
    bool checkValidity() const;

 public:
    /**
     * @brief
     * @return A regex which can be used to check for valid ISO strings.
     */
    static std::string getRegex();

    /**
     * @brief
     * @return Saved year.
     */
    uint16_t getYear() const;

    /**
     * @brief
     * @return Saved month.
     */
    uint8_t getMonth() const;

    /**
     * @brief
     * @return Saved day.
     */
    uint8_t getDay() const;

    /**
     * @brief
     * @return True if the time variables are valid.
     */
    bool isTimePresent() const;

    /**
     * @brief
     * @return Hour.
     */
    uint8_t getHour() const;

    /**
     * @brief
     * @return Minute.
     */
    uint8_t getMinute() const;

    /**
     * @brief
     * @return Second.
     */
    uint8_t getSecond() const;

    /**
     * @brief
     * @return True if the year in this date is a leap year and february has 29 days instead of 28.
     */
    bool leapYear() const;

    /**
     * @brief Parse a date from string.
     * @param string String with date information.
     * @return Constructed date object.
     */
    static Date fromString(const std::string& string);

    /**
     * @brief Construct date only.
     * @param _year Year.
     * @param _month Month.
     * @param _day Day.
     */
    Date(uint16_t _year, uint8_t _month, uint8_t _day);

    /**
     * @brief Construct date and time.
     * @param _year Year.
     * @param _month Month.
     * @param _day Day.
     * @param _hour Hour.
     * @param _minute Minute.
     * @param _second Second.
     */
    Date(uint16_t _year, uint8_t _month, uint8_t _day, uint8_t _hour, uint8_t _minute, uint8_t _second);

    /**
     * @brief Converts the date object into a printable string.
     * @return Date string.
     */
    std::string toString() const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace util
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_UTIL_DATE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
