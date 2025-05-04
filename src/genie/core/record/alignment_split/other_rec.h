/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_OTHER_REC_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_OTHER_REC_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <optional>

#include "genie/core/record/alignment.h"
#include "genie/core/record/alignment_split.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::core::record::alignment_split {

/**
 * @brief
 */
class OtherRec final : public AlignmentSplit {
  uint64_t split_pos_;     //!< @brief
  uint16_t split_seq_id_;  //!< @brief
  std::optional<Alignment> extended_alignment_;  //!< @brief

 public:
  /**
   * @brief
   * @param split_pos
   * @param split_seq_id
   * @param extended_alignment
   */
  OtherRec(uint64_t split_pos, uint16_t split_seq_id,
           const std::optional<Alignment>& extended_alignment);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint64_t GetNextPos() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint16_t GetNextSeq() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::optional<Alignment> GetExtendedAlignment() const;

  /**
   * @brief
   * @param extended_alignment
   * @param reader
   */
  explicit OtherRec(bool extended_alignment, util::BitReader& reader);

  /**
   * @brief
   */
  OtherRec();

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const override;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::unique_ptr<AlignmentSplit> clone() const override;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::optional<bool> IsExtendedAlignment() const override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_split

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_OTHER_REC_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
