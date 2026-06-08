/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_MM_CFG_H_
#define SRC_GENIE_FORMAT_MGB_MM_CFG_H_

// -----------------------------------------------------------------------------

#include <cstdint>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

/**
 * @brief ISO 23092-2 Section 3.4.1.1 table 19 lines 7 to 10
 */
class MmCfg final {
  uint16_t mm_threshold_;  //!< Line 8
  uint32_t mm_count_;      //!< Line 9

 public:
  /**
   * @brief
   * @param other
   * @return
   */
  bool operator==(const MmCfg& other) const;

  /**
   * @brief
   * @param mm_threshold
   * @param mm_count
   */
  MmCfg(uint16_t mm_threshold, uint32_t mm_count);

  /**
   * @brief
   */
  MmCfg();

  /**
   * @brief
   * @param reader
   */
  explicit MmCfg(util::BitReader& reader);

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_MM_CFG_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
