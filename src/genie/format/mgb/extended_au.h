/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_EXTENDED_AU_H_
#define SRC_GENIE_FORMAT_MGB_EXTENDED_AU_H_

// -----------------------------------------------------------------------------

#include <cstdint>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

/**
 * @brief
 */
class ExtendedAu {
 private:
  uint64_t extended_AU_start_position;  //!< @brief
  uint64_t extended_AU_end_position;    //!< @brief

  uint8_t posSize;  //!< @brief Internal

 public:
  /**
   * @brief
   * @param other
   * @return
   */
  bool operator==(const ExtendedAu& other) const;

  /**
   * @brief
   * @param extended_au_start_position
   * @param extended_au_end_position
   * @param pos_size
   */
  ExtendedAu(uint64_t extended_au_start_position,
             uint64_t extended_au_end_position, uint8_t pos_size);

  /**
   * @brief
   * @param pos_size
   * @param reader
   */
  ExtendedAu(uint8_t pos_size, util::BitReader& reader);

  /**
   * @brief
   */
  virtual ~ExtendedAu() = default;

  /**
   * @brief
   * @param writer
   */
  virtual void Write(util::BitWriter& writer) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_EXTENDED_AU_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
