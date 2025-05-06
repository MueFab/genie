/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.h
 *
 * @brief This file contains the definition of the `TagDecoder` class, which
 * is used to decode auxiliary tags in the Genie framework.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_DEMULTIPLEXTAG_DECODER_H_
#define SRC_GENIE_READ_DEMULTIPLEXTAG_DECODER_H_

// -----------------------------------------------------------------------------

#include "genie/core/record/tag_record.h"

// -----------------------------------------------------------------------------

namespace genie::demultiplex_tag {

class TagDecoder {};

/**
 * @brief Class responsible for decoding auxiliary tags in the Genie framework.
 *
 * This class provides functionality to decode tags from a given access unit
 * descriptor. It extracts the tag information and returns it as a TagRecord.
 */
class Decoder : public TagDecoder {
  /// @brief Access unit descriptor containing the tag information.
  core::AccessUnit::Descriptor desc_;

 public:
  /**
   * @brief Constructs a TagDecoder with the given access unit descriptor.
   * @param desc The access unit descriptor containing the tag information.
   */
  explicit Decoder(core::AccessUnit::Descriptor&& desc);

  /**
   * @brief Decodes the tags from the access unit descriptor.
   * @return A TagRecord containing the decoded tags.
   */
  core::record::TagRecord DecodeTags();
};

// -----------------------------------------------------------------------------

}  // namespace genie::demultiplex_tag

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_DEMULTIPLEXTAG_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
