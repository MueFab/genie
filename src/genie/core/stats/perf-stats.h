/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_STATS_PERF_STATS_H_
#define SRC_GENIE_CORE_STATS_PERF_STATS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <map>
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::stats {

/**
 * @brief
 */
class PerfStats {
 public:
    /**
     * @brief
     */
    struct Data {
        union {
            double fData;   //!< @brief
            int64_t iData;  //!< @brief
        };
    };

    /**
     * @brief
     */
    struct Stat {
        bool isInteger{};  //!< @brief
        Data min{};        //!< @brief
        Data sum{};        //!< @brief
        Data max{};        //!< @brief
        uint64_t ctr{};    //!< @brief

        /**
         * @brief
         * @return
         */
        [[nodiscard]] double avg() const;
    };

    /**
     * @brief
     * @param name
     * @param dat
     */
    void addDouble(const std::string& name, double dat);

    /**
     * @brief
     * @param name
     * @param dat
     */
    void addInteger(const std::string& name, int64_t dat);

    /**
     * @brief
     * @param name
     * @param s
     */
    void add(const std::string& name, const Stat& s);

    /**
     * @brief
     * @param stats
     */
    void add(const PerfStats& stats);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] std::map<std::string, Stat>::const_iterator begin() const;

    /**
     * @brief
     * @return
     */
    [[nodiscard]] std::map<std::string, Stat>::const_iterator end() const;

    /**
     * @brief
     * @param _active
     */
    void setActive(bool _active);

    /**
     * @brief
     * @return
     */
    [[nodiscard]] bool isActive() const;

 private:
    bool active{true};                 //!< @brief
    std::map<std::string, Stat> data;  //!< @brief
};

/**
 * @brief
 * @param stream
 * @param stats
 * @return
 */
std::ostream& operator<<(std::ostream& stream, const PerfStats& stats);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::stats

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_STATS_PERF_STATS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
