/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_STATS_PERF_STATS_H_
#define SRC_GENIE_CORE_STATS_PERF_STATS_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <map>
#include <string>

// -----------------------------------------------------------------------------

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
      double f_data;   //!< @brief
      int64_t i_data;  //!< @brief
    };
  };

  /**
   * @brief
   */
  struct Stat {
    bool is_integer{};  //!< @brief
    Data min{};         //!< @brief
    Data sum{};         //!< @brief
    Data max{};         //!< @brief
    uint64_t ctr{};     //!< @brief

    /**
     * @brief
     * @return
     */
    [[nodiscard]] double Avg() const;
  };

  /**
   * @brief
   * @param name
   * @param dat
   */
  void AddDouble(const std::string& name, double dat);

  /**
   * @brief
   * @param name
   * @param dat
   */
  void AddInteger(const std::string& name, int64_t dat);

  /**
   * @brief
   * @param name
   * @param s
   */
  void Add(const std::string& name, const Stat& s);

  /**
   * @brief
   * @param stats
   */
  void Add(const PerfStats& stats);

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
   * @param active
   */
  void SetActive(bool active);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsActive() const;

  void print() const;

 private:
  bool active_{true};                 //!< @brief
  std::map<std::string, Stat> data_;  //!< @brief
};

/**
 * @brief
 * @param stream
 * @param stats
 * @return
 */
std::ostream& operator<<(std::ostream& stream, const PerfStats& stats);

// -----------------------------------------------------------------------------

}  // namespace genie::core::stats

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_STATS_PERF_STATS_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
