/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_RAW_REFERENCE_SEQ_H_
#define SRC_GENIE_FORMAT_MGB_RAW_REFERENCE_SEQ_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>

#include "genie/util/bit_reader.h"
#include "genie/util/bit_writer.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

/**
 * @brief
 */
class RawReferenceSequence final {
  uint16_t sequence_id_;      //!< @brief
  uint64_t seq_start_;        //!< @brief
  uint64_t seq_end_;          //!< @brief
  std::string ref_sequence_;  //!< @brief

 public:
  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint16_t GetSeqId() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint64_t GetStart() const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint64_t GetEnd() const;

  /**
   * @brief
   * @return
   */
  std::string& GetSequence();

  /**
   * @brief
   * @return
   */
  [[nodiscard]] const std::string& GetSequence() const;

  /**
   * @brief
   * @param sequence_id
   * @param seq_start
   * @param ref_sequence
   */
  RawReferenceSequence(uint16_t sequence_id, uint64_t seq_start,
                       std::string&& ref_sequence);

  /**
   * @brief
   * @param reader
   * @param header_only
   */
  explicit RawReferenceSequence(util::BitReader& reader, bool header_only);

  /**
   * @brief
   * @param s
   * @return
   */
  [[nodiscard]] bool IsIdUnique(const RawReferenceSequence& s) const;

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] uint64_t GetTotalSize() const;
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_RAW_REFERENCE_SEQ_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
