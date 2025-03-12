/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MGB_RAW_REFERENCE_H_
#define SRC_GENIE_FORMAT_MGB_RAW_REFERENCE_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <vector>

#include "genie/core/parameter/data_unit.h"
#include "genie/format/mgb/raw_reference_seq.h"
#include "genie/util/bit_writer.h"

// -----------------------------------------------------------------------------

namespace genie::format::mgb {

/**
 * @brief
 */
class RawReference final : public core::parameter::DataUnit {
  std::vector<RawReferenceSequence> seqs_;  //!< @brief

  bool headerless_;

 public:
  /**
   * @brief
   */
  explicit RawReference(bool header_less = false);

  /**
   * @brief
   * @param reader
   * @param header_only
   * @param header_less
   */
  explicit RawReference(util::BitReader& reader, bool header_only = false,
                        bool header_less = false);

  /**
   * @brief
   */
  ~RawReference() override = default;

  /**
   * @brief
   * @param ref
   */
  void AddSequence(RawReferenceSequence&& ref);

  /**
   * @brief
   * @param writer
   */
  void Write(util::BitWriter& writer) const override;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] bool IsEmpty() const;

  /**
   * @brief
   * @param index
   * @return
   */
  RawReferenceSequence& GetSequence(size_t index);

  /**
   * @brief
   * @return
   */
  std::vector<RawReferenceSequence>::iterator begin();

  /**
   * @brief
   * @return
   */
  std::vector<RawReferenceSequence>::iterator end();

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::vector<RawReferenceSequence>::const_iterator begin()
      const {
    return seqs_.begin();
  }

  /**
   * @brief
   * @return
   */
  [[nodiscard]] std::vector<RawReferenceSequence>::const_iterator end() const {
    return seqs_.end();
  }

  /**
   * @brief
   * @param state
   */
  void SetHeaderLess(bool state);
};

// -----------------------------------------------------------------------------

}  // namespace genie::format::mgb

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MGB_RAW_REFERENCE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
