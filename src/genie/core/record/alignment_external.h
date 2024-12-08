/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <memory>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::core::record {

/**
 * @brief
 */
class AlignmentExternal {
 public:
  /**
   * @brief
   */
  enum class Type : uint8_t { kNone, kOtherRec };

  /**
   * @brief
   * @param more_alignment_info_type
   */
  explicit AlignmentExternal(Type more_alignment_info_type);

  /**
   * @brief
   */
  virtual ~AlignmentExternal() = default;

  /**
   * @brief
   * @param writer
   */
  virtual void Write(util::BitWriter& writer) const;

  /**
   * @brief
   * @param reader
   * @return
   */
  static std::unique_ptr<AlignmentExternal> Factory(util::BitReader& reader);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] virtual std::unique_ptr<AlignmentExternal> Clone() const = 0;

 protected:
  Type more_alignment_info_type_;  //!< @brief
};

// -----------------------------------------------------------------------------

}  // namespace genie::core::record

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
