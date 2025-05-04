/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_UNPAIRED_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_UNPAIRED_H_

// -----------------------------------------------------------------------------

#include <memory>

#include "genie/core/record/alignment_split.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::core::record::alignment_split {

/**
 * @brief
 */
class Unpaired final : public AlignmentSplit {
 public:
  /**
   * @brief
   */
  Unpaired();

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const override;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::optional<bool> IsExtendedAlignment() const override;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::unique_ptr<AlignmentSplit> clone() const override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_split

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_UNPAIRED_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
