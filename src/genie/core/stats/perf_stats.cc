/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/stats/perf_stats.h"

#include <algorithm>
#include <iomanip>
#include <map>
#include <ostream>
#include <string>

#include "genie/util/log.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "Stats";

namespace genie::core::stats {

// -----------------------------------------------------------------------------
double PerfStats::Stat::Avg() const {
  if (is_integer) {
    return static_cast<double>(sum.i_data) / static_cast<double>(ctr);
  }
  return sum.f_data / static_cast<double>(ctr);
}

// -----------------------------------------------------------------------------
void PerfStats::AddDouble(const std::string& name, const double dat) {
  Stat s;
  s.is_integer = false;
  s.min.f_data = dat;
  s.sum.f_data = dat;
  s.max.f_data = dat;
  s.ctr = 1;
  Add(name, s);
}

// -----------------------------------------------------------------------------
void PerfStats::AddInteger(const std::string& name, const int64_t dat) {
  Stat s;
  s.is_integer = true;
  s.min.i_data = dat;
  s.sum.i_data = dat;
  s.max.i_data = dat;
  s.ctr = 1;
  Add(name, s);
}

// -----------------------------------------------------------------------------
void PerfStats::Add(const std::string& name, const Stat& s) {
  if (!active_) {
    return;
  }
  if (const auto it = data_.find(name); it == data_.end()) {
    data_[name] = s;
  } else {
    it->second.ctr += s.ctr;
    UTILS_DIE_IF(it->second.is_integer != s.is_integer,
                 "Tried to combine integer and floating point numbers in "
                 "statistics");
    if (it->second.is_integer) {
      it->second.min.i_data = std::min(it->second.min.i_data, s.min.i_data);
      it->second.max.i_data = std::max(it->second.max.i_data, s.max.i_data);
      it->second.sum.i_data += s.sum.i_data;
    } else {
      it->second.min.f_data = std::min(it->second.min.f_data, s.min.f_data);
      it->second.max.f_data = std::max(it->second.max.f_data, s.max.f_data);
      it->second.sum.f_data += s.sum.f_data;
    }
  }
}

// -----------------------------------------------------------------------------
void PerfStats::Add(const PerfStats& stats) {
  for (const auto& [fst, snd] : stats.data_) {
    Add(fst, snd);
  }
}

// -----------------------------------------------------------------------------
std::map<std::string, PerfStats::Stat>::const_iterator PerfStats::begin()
    const {
  return data_.begin();
}

// -----------------------------------------------------------------------------
std::map<std::string, PerfStats::Stat>::const_iterator PerfStats::end() const {
  return data_.end();
}

// -----------------------------------------------------------------------------
void PerfStats::SetActive(const bool active) { active_ = active; }

// -----------------------------------------------------------------------------
bool PerfStats::IsActive() const { return active_; }

void PerfStats::print() const {
  for (const auto& [fst, snd] : *this) {
    std::stringstream stream;
    stream << std::setw(40) << std::left << fst;
    if (snd.is_integer) {
      stream << "sum: " << std::setw(16) << std::left << std::fixed
             << snd.sum.i_data;
    } else {
      stream << "sum: " << std::setw(16) << std::left << std::fixed
             << snd.sum.f_data;
    }
    UTILS_LOG(genie::util::Logger::Severity::INFO, stream.str());
  }
}


// -----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& stream, const PerfStats& stats) {
  for (const auto& [fst, snd] : stats) {
    stream << std::setw(40) << std::left << fst;
    if (snd.is_integer) {
      stream << "sum: " << std::setw(16) << std::left << std::fixed
             << snd.sum.i_data << std::endl;
    } else {
      stream << "sum: " << std::setw(16) << std::left << std::fixed
             << snd.sum.f_data << std::endl;
    }
  }
  return stream;
}

// -----------------------------------------------------------------------------

}  // namespace genie::core::stats

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
