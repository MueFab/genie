/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_AU_TYPE_CFG_H_
#define SRC_GENIE_FORMAT_MGB_AU_TYPE_CFG_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <optional>

#include "genie/format/mgb/extended_au.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

/**
 * @brief
 */
class AuTypeCfg {
 private:
  uint16_t sequence_ID;                   //!< @brief
  uint64_t AU_start_position;             //!< @brief
  uint64_t AU_end_position;               //!< @brief
  std::optional<ExtendedAu> extended_AU;  //!< @brief

  uint8_t posSize;  //!< @brief internal

 public:
  /**
   * @brief
   * @param other
   * @return
   */
  bool operator==(const AuTypeCfg& other) const;

  /**
   * @brief
   * @param sequence_id
   * @param au_start_position
   * @param au_end_position
   * @param pos_size
   */
  AuTypeCfg(uint16_t sequence_id, uint64_t au_start_position,
            uint64_t au_end_position, uint8_t pos_size);

  /**
   * @brief
   * @param pos_size
   */
  explicit AuTypeCfg(uint8_t pos_size);

  /**
   * @brief
   * @param pos_size
   * @param multiple_alignments
   * @param reader
   */
  AuTypeCfg(uint8_t pos_size, bool multiple_alignments,
            util::BitReader& reader);

  /**
   * @brief
   */
  virtual ~AuTypeCfg() = default;

  /**
   * @brief
   * @param extended_au
   */
  void SetExtendedAu(ExtendedAu&& extended_au);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint16_t GetRefId() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint64_t GetStartPos() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint64_t GetEndPos() const;

  /**
   * @brief
   * @param writer
   */
  virtual void Write(util::BitWriter& writer) const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_AU_TYPE_CFG_H_
