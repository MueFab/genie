/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_OTHER_REC_H_
#define SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_OTHER_REC_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstdint>
#include <memory>

#include "genie/core/record/alignment_external.h"
#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core::record::alignment_external {

/**
 * @brief
 */
class OtherRec final : public AlignmentExternal {
  uint64_t next_pos_;     //!< @brief
  uint16_t next_seq_id_;  //!< @brief

 public:
  /**
   * @brief
   * @param next_pos
   * @param next_seq_id
   */
  OtherRec(uint64_t _next_pos, uint16_t _next_seq_ID);

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
   */
  OtherRec();

  /**
   * @brief
   * @param reader
   */
  explicit OtherRec(util::BitReader &reader);

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

#endif  // SRC_GENIE_CORE_RECORD_ALIGNMENT_EXTERNAL_OTHER_REC_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
