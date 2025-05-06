/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder.h
 *
 * @brief This file contains the definition of the `TagEncoder` class, which
 * is used to encode auxiliary tags in the Genie framework.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_DEMULTIPLEXTAG_ENCODER_H_
#define SRC_GENIE_READ_DEMULTIPLEXTAG_ENCODER_H_

// -----------------------------------------------------------------------------

#include "genie/core/access_unit.h"
#include "genie/core/record/tag_record.h"

// -----------------------------------------------------------------------------

namespace genie::demultiplex_tag {

class TagEncoder {};

/**
 * @brief Class responsible for encoding auxiliary tags in the Genie framework.
 *
 * This class provides functionality to encode tags into a given access unit
 * descriptor. It converts the tag information into a format suitable for
 * storage or transmission.
 */
class Encoder : public TagEncoder {
  /// @brief Access unit descriptor to store the encoded tag information.
  core::AccessUnit::Descriptor desc_;

  /**
   * @brief Encodes the given key into a 16-bit integer.
   * @param key The key of the tag to encode.
   */
  static uint16_t EncodeTagKey(const std::array<char, 2>& key);

 public:
  /**
   * @brief Constructs a TagEncoder.
   */
  Encoder();

  /**
   * @brief Encodes the given tags into the access unit descriptor.
   * @param tags The tag record containing the tags to encode.
   */
  void EncodeTags(const core::record::TagRecord& tags);

  /**
   * @brief Moves the encoded access unit descriptor out of the encoder.
   * @return The encoded access unit descriptor.
   */
  core::AccessUnit::Descriptor&& MoveDescriptor();
};

// -----------------------------------------------------------------------------

}  // namespace genie::demultiplex_tag

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_DEMULTIPLEXTAG_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
