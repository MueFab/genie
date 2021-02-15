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
 *
 */
class PerfStats {
 public:
    /**
     *
     */
    struct Data {
        union {
            double fData;   //!<
            int64_t iData;  //!<
        };
    };

    /**
     *
     */
    struct Stat {
        bool isInteger{};  //!<
        Data min{};        //!<
        Data sum{};        //!<
        Data max{};        //!<
        uint64_t ctr{};    //!<

        /**
         *
         * @return
         */
        double avg() const;
    };

    /**
     *
     * @param name
     * @param dat
     */
    void addDouble(const std::string& name, double dat);

    /**
     *
     * @param name
     * @param dat
     */
    void addInteger(const std::string& name, int64_t dat);

    /**
     *
     * @param name
     * @param s
     */
    void add(const std::string& name, const Stat& s);

    /**
     *
     * @param stats
     */
    void add(const PerfStats& stats);

    /**
     *
     * @return
     */
    std::map<std::string, Stat>::const_iterator begin() const;

    /**
     *
     * @return
     */
    std::map<std::string, Stat>::const_iterator end() const;

    /**
     *
     * @param _active
     */
    void setActive(bool _active);

    /**
     *
     * @return
     */
    bool isActive() const;

 private:
    bool active{true};                 //!<
    std::map<std::string, Stat> data;  //!<
};

/**
 *
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
