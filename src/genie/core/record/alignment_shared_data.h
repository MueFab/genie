/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_SHARED_DATA_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_SHARED_DATA_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <memory>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::core::record {

/**
 * @brief
 */
class AlignmentSharedData final {
  uint16_t seq_id_;   //!< @brief
  uint8_t as_depth_;  //!< @brief

 public:
  /**
   * @brief
   */
  virtual ~AlignmentSharedData() = default;

  /**
   * @brief
   */
  AlignmentSharedData();

  /**
   * @brief
   * @param seq_id
   * @param as_depth
   */
  AlignmentSharedData(uint16_t seq_id, uint8_t as_depth);

  /**
   * @brief
   * @param reader
   */
  explicit AlignmentSharedData(util::BitReader& reader);

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint16_t GetSeqId() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetAsDepth() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_SHARED_DATA_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
