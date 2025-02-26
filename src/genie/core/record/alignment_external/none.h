/**
* Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_NONE_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_NONE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>

#include "genie/core/record/alignment_external.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::alignment_external {

/**
 * @brief
 */
class None final : public AlignmentExternal {
 public:
  /**
   * @brief
   */
  None();

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter &writer) const override;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::unique_ptr<AlignmentExternal> Clone() const override;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record::alignment_external

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_NONE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
