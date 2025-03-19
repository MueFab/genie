/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_PARAMETER_DATA_UNIT_H_
#define SRC_GENIE_CORE_PARAMETER_DATA_UNIT_H_

// -----------------------------------------------------------------------------

#include <cstdint>

#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------

namespace stats {
class PerfStats;
}

// -----------------------------------------------------------------------------

namespace parameter {

/**
 * @brief
 */
class DataUnit {
 public:
  /**
   * @brief
   */
  enum class DataUnitType : uint8_t {
    kRawReference = 0,
    kParameterSet = 1,
    kAccessUnit = 2
  };

  /**
   * @brief
   */
  enum class DatasetType : uint8_t {
    kNonAligned = 0,
    kAligned = 1,
    kReference = 2,
    kCount = 3
  };

 private:
  DataUnitType data_unit_type_;  //!< @brief

 public:
  /**
   * @brief
   * @param t
   */
  explicit DataUnit(const DataUnitType& t);

  /**
   * @brief
   */
  virtual ~DataUnit() = default;

  /**
   * @brief
   * @param writer
   */
  virtual void Write(util::BitWriter& writer) const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] DataUnitType GetDataUnitType() const;

  /**
   * @brief
   * @param output
   */
  virtual void PrintDebug(std::ostream& output, uint8_t, uint8_t) const;
};

// -----------------------------------------------------------------------------

}  // namespace parameter
}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_PARAMETER_DATA_UNIT_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------