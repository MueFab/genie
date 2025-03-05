/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_BOX_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_BOX_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>
#include <vector>
#include "alignment.h"
#include "alignment_split.h"
#include "class_type.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/make_unique.h"
#include "genie/util/runtime_exception.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

/**
 * @brief Class representing an external alignment box
 *
 * Contains alignment information and optional split alignment data.
 */
class AlignmentBox {
 private:
  uint64_t mapping_pos_{};                                             //!< @brief
  Alignment alignment_;                                                //!< @brief
  std::vector<std::unique_ptr<AlignmentSplit>> split_alignment_info_;  //!< @brief

 public:
  /**
   * @brief Default constructor
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief Constructs an alignment box by reading from a bit stream
   * @param type The class type
   * @param as_depth The alignment split depth
   * @param number_of_template_segments The number of template segments
   * @param reader The bit reader to read from
   */
  explicit AlignmentBox(ClassType type, uint8_t as_depth, uint8_t number_of_template_segments,
                        util::BitReader& reader);

  /**
   * @brief Constructs an alignment box with specified parameters
   * @param mapping_pos The mapping position
   * @param alignment The alignment information
   */
  AlignmentBox(uint64_t mapping_pos, Alignment&& alignment);

  /**
   * @brief Copy constructor
   * @param container The alignment box to copy from
   */
  AlignmentBox(const AlignmentBox& container);

  /**
   * @brief Move constructor
   * @param container The alignment box to move from
   */
  AlignmentBox(AlignmentBox&& container) noexcept;

  /**
   * @brief Default destructor
   */
  ~AlignmentBox() = default;

  /**
   * @brief Copy assignment operator
   * @param container The alignment box to copy from
   * @return Reference to this alignment box
   */
  AlignmentBox& operator=(const AlignmentBox& container);

  /**
   * @brief Move assignment operator
   * @param container The alignment box to move from
   * @return Reference to this alignment box
   */
  AlignmentBox& operator=(AlignmentBox&& container) noexcept;

  /**
   * @brief Adds a split alignment to this alignment box
   * @param alignment The split alignment to add
   */
  void AddAlignmentSplit(std::unique_ptr<AlignmentSplit> alignment);

  /**
   * @brief Gets the mapping position
   * @return The mapping position
   */
  AlignmentBox();

  /**
   * @brief Gets the mapping position
   * @return The mapping position
   */
  [[nodiscard]] uint64_t GetPosition() const;

  /**
   * @brief Gets the alignment information
   * @return Constant reference to the alignment
   */
  [[nodiscard]] const Alignment& GetAlignment() const;

  /**
   * @brief Gets the split alignment information
   * @return Constant reference to the split alignments
   */
  [[nodiscard]] const std::vector<std::unique_ptr<AlignmentSplit>>& GetAlignmentSplits() const;

  /**
   * @brief Gets the number of template segments
   * @return The number of template segments
   */
  [[nodiscard]] uint8_t GetNumberOfTemplateSegments() const;
};

// -------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// -------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_BOX_H_

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
