/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record {

/**
 * @brief
 */
class AlignmentSplit {
 public:
  /**
   * @brief
   */
  enum class Type : uint8_t { kSameRec = 0, kOtherRec = 1, kUnpaired = 2 };

  /**
   * @brief
   * @param split_alignment
   */
  explicit AlignmentSplit(Type split_alignment);

  /**
   * @brief
   */
  virtual ~AlignmentSplit() = default;

  /**
   * @brief
   * @param writer
   */
  virtual void Write(util::BitWriter& writer) const;

  /**
   * @brief
   * @param as_depth
   * @param reader
   * @return
   */
  static std::unique_ptr<AlignmentSplit> Factory(uint8_t as_depth, util::BitReader& reader);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] virtual std::unique_ptr<AlignmentSplit> clone() const = 0;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] Type GetType() const;

 protected:
  Type split_alignment_;  //!< @brief
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core::record

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_SPLIT_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
