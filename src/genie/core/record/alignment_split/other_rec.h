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

 public:
  /**
   * @brief
   * @param split_pos
   * @param split_seq_id
   */
  OtherRec(uint64_t split_pos, uint16_t split_seq_id);

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
   * @param reader
   */
  explicit OtherRec(util::BitReader& reader);

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
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_split

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_OTHER_REC_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
