/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_REF_CFG_H_
#define SRC_GENIE_FORMAT_MGB_REF_CFG_H_

// -----------------------------------------------------------------------------

#include <cstdint>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

/**
 * @brief
 */
class RefCfg final {
  uint16_t ref_sequence_id_;     //!< @brief
  uint64_t ref_start_position_;  //!< @brief
  uint64_t ref_end_position_;    //!< @brief

  uint8_t pos_size_;  //!< @brief internal

 public:
  /**
   * @brief
   * @param other
   * @return
   */
  bool operator==(const RefCfg& other) const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint16_t GetSeqId() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint64_t GetStart() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint64_t GetEnd() const;

  /**
   * @brief
   * @param ref_sequence_id
   * @param ref_start_position
   * @param ref_end_position
   * @param pos_size
   */
  RefCfg(uint16_t ref_sequence_id, uint64_t ref_start_position,
         uint64_t ref_end_position, uint8_t pos_size);

  /**
   * @brief
   * @param pos_size
   */
  explicit RefCfg(uint8_t pos_size);

  /**
   * @brief
   * @param pos_size
   * @param reader
   */
  RefCfg(uint8_t pos_size, util::BitReader& reader);

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_REF_CFG_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
