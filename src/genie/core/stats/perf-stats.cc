/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/core/stats/perf-stats.h"
#include <algorithm>
#include <iomanip>
#include <ostream>
#include "genie/util/runtime-exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {
namespace stats {

// ---------------------------------------------------------------------------------------------------------------------

double PerfStats::Stat::avg() const {
    if (isInteger) {
        return static_cast<double>(sum.iData) / ctr;
    } else {
        return sum.fData / ctr;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void PerfStats::addDouble(const std::string& name, double dat) {
    Stat s;
    s.isInteger = false;
    s.min.fData = dat;
    s.sum.fData = dat;
    s.max.fData = dat;
    s.ctr = 1;
    add(name, s);
}

// ---------------------------------------------------------------------------------------------------------------------

void PerfStats::addInteger(const std::string& name, int64_t dat) {
    Stat s;
    s.isInteger = true;
    s.min.iData = dat;
    s.sum.iData = dat;
    s.max.iData = dat;
    s.ctr = 1;
    add(name, s);
}

// ---------------------------------------------------------------------------------------------------------------------

void PerfStats::add(const std::string& name, const Stat& s) {
    if (!active) {
        return;
    }
    auto it = data.find(name);
    if (it == data.end()) {
        data[name] = s;
    } else {
        it->second.ctr += s.ctr;
        UTILS_DIE_IF(it->second.isInteger != s.isInteger,
                     "Tried to combine integer and floating point numbers in statistics");
        if (it->second.isInteger) {
            it->second.min.iData = std::min(it->second.min.iData, s.min.iData);
            it->second.max.iData = std::max(it->second.max.iData, s.max.iData);
            it->second.sum.iData += s.sum.iData;
        } else {
            it->second.min.fData = std::min(it->second.min.fData, s.min.fData);
            it->second.max.fData = std::max(it->second.max.fData, s.max.fData);
            it->second.sum.fData += s.sum.fData;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void PerfStats::add(const PerfStats& stats) {
    for (const auto& p : stats.data) {
        add(p.first, p.second);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::map<std::string, PerfStats::Stat>::const_iterator PerfStats::begin() const { return data.begin(); }

// ---------------------------------------------------------------------------------------------------------------------

std::map<std::string, PerfStats::Stat>::const_iterator PerfStats::end() const { return data.end(); }

// ---------------------------------------------------------------------------------------------------------------------

void PerfStats::setActive(bool _active) { active = _active; }

// ---------------------------------------------------------------------------------------------------------------------

bool PerfStats::isActive() const { return active; }

// ---------------------------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const PerfStats& stats) {
    for (const auto& s : stats) {
        stream << std::setw(40) << std::left << s.first;
        if (s.second.isInteger) {
            //   stream << "min: " << std::setw(16) << std::left << std::fixed << s.second.min.iData;
            //  stream << "avg: " << std::setw(16) << std::setfill(' ') << std::setprecision(2) <<  std::left <<
            //  std::fixed << s.second.avg();
            //   stream << "max: " << std::setw(16) << std::left << std::fixed << s.second.max.iData << std::endl;
            stream << "sum: " << std::setw(16) << std::left << std::fixed << s.second.sum.iData << std::endl;
        } else {
            //    stream << "min: " << std::setw(16) << std::left << std::fixed << s.second.min.fData;
            //   stream << "avg: " << std::setw(16) << std::left << std::fixed << s.second.avg();
            //    stream << "max: " << std::setw(16) << std::left << std::fixed << s.second.max.fData << std::endl;
            stream << "sum: " << std::setw(16) << std::left << std::fixed << s.second.sum.fData << std::endl;
        }
    }
    return stream;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace stats
}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
