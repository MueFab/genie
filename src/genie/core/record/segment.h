/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_SEGMENT_H_
#define SRC_GENIE_CORE_RECORD_SEGMENT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

// ---------------------------------------------------------------------------------------------------------------------

/**
 * @brief
 */
class Segment {
  std::string sequence_;                     //!< @brief
  std::vector<std::string> quality_values_;  //!< @brief

 public:
  /**
   * @brief
   */
  Segment();

  /**
   * @brief
   * @param _sequence
   */
  explicit Segment(std::string&& _sequence);

  /**
   * @brief
   * @param length
   * @param qv_depth
   * @param reader
   */
  Segment(uint32_t length, uint8_t qv_depth, util::BitReader& reader);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::string& GetSequence() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::vector<std::string>& GetQualities() const;

  /**
   * @brief
   * @param qv
   */
  void AddQualities(std::string&& qv);

  /**
   * @brief
   * @param write
   */
  void Write(util::BitWriter& write) const;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_SEGMENT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
