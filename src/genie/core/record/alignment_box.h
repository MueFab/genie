/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_BOX_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_BOX_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <vector>

#include "genie/core/record/alignment.h"
#include "genie/core/record/alignment_split.h"
#include "genie/core/record/class_type.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::core::record {

/**
 * @brief ExternalAlignment
 */
class AlignmentBox {
  uint64_t mapping_pos_{};  //!< @brief
  Alignment alignment_;     //!< @brief
  std::vector<std::unique_ptr<AlignmentSplit>>
      split_alignment_info_;  //!< @brief

 public:
  [[nodiscard]] bool IsExtendedAlignment() const {
    return alignment_.GetFlags().has_value();
  }

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief
   * @param type
   * @param as_depth
   * @param extended_alignment
   * @param number_of_template_segments
   * @param reader
   */
  explicit AlignmentBox(ClassType type, uint8_t as_depth,
                        bool extended_alignment,
                        uint8_t number_of_template_segments,
                        util::BitReader& reader);

  /**
   * @brief
   * @param mapping_pos
   * @param alignment
   */
  AlignmentBox(uint64_t mapping_pos, Alignment&& alignment);

  /**
   * @brief
   * @param container
   */
  AlignmentBox(const AlignmentBox& container);

  /**
   * @brief
   * @param container
   */
  AlignmentBox(AlignmentBox&& container) noexcept;

  /**
   * @brief
   */
  ~AlignmentBox() = default;

  /**
   * @brief
   * @param container
   * @return
   */
  AlignmentBox& operator=(const AlignmentBox& container);

  /**
   * @brief
   * @param container
   * @return
   */
  AlignmentBox& operator=(AlignmentBox&& container) noexcept;

  /**
   * @brief
   * @param alignment
   */
  void AddAlignmentSplit(std::unique_ptr<AlignmentSplit> alignment);

  /**
   * @brief
   */
  AlignmentBox();

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint64_t GetPosition() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const Alignment& GetAlignment() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::vector<std::unique_ptr<AlignmentSplit>>&
  GetAlignmentSplits() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint8_t GetNumberOfTemplateSegments() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_BOX_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
