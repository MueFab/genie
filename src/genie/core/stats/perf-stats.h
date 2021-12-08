/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_STATS_PERF_STATS_H_
#define SRC_GENIE_CORE_STATS_PERF_STATS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace stats {

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
        double avg() const;
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
    std::map<std::string, Stat>::const_iterator begin() const;

    /**
     * @brief
     * @return
     */
    std::map<std::string, Stat>::const_iterator end() const;

    /**
     * @brief
     * @param _active
     */
    void setActive(bool _active);

    /**
     * @brief
     * @return
     */
    bool isActive() const;

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

}  // namespace stats
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_STATS_PERF_STATS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
