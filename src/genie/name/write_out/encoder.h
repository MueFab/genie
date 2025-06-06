/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the Encoder class for handling name encoding in the
 * GENIE framework.
 *
 * This file contains the declaration of the `Encoder` class, which is a
 * specialized implementation of the `NameEncoder` interface within the GENIE
 * framework. The `Encoder` class is responsible for processing genomic record
 * chunks and encoding their names according to the GENIE format specifications.
 *
 * @details The `Encoder` class inherits from the `core::NameEncoder` base class
 * and overrides the `process` method to handle name encoding tasks. It uses
 * various utilities and tokenizers provided by the GENIE library to efficiently
 * process record chunks and produce encoded output.
 */

#ifndef SRC_GENIE_NAME_WRITE_OUT_ENCODER_H_
#define SRC_GENIE_NAME_WRITE_OUT_ENCODER_H_

// -----------------------------------------------------------------------------

#include <string>
#include <tuple>

#include "genie/core/name_encoder.h"
#include "genie/name/tokenizer/tokenizer.h"

// -----------------------------------------------------------------------------

namespace genie::name::write_out {

/**
 * @brief Implements the name encoding functionality for Genie
 *
 * The `Encoder` class is responsible for encoding the names of genomic records
 * and preparing them for write out operations. It processes input record
 * chunks, extracts the necessary metadata, and encodes the record names using
 * the specified GENIE format. The class is designed to work within the
 * `write_out` module of the GENIE library.
 */
class Encoder final : public core::NameEncoder {
 public:
  /**
   * @brief Processes a chunk of genomic records and encodes their names.
   *
   * This method takes a chunk of genomic records as input, performs the name
   * encoding operations, and returns the resulting descriptor along with
   * performance statistics. The encoded data is structured according to the
   * GENIE format specifications.
   *
   * @param recs The chunk of genomic records to be processed.
   * @return A tuple containing:
   *  - `core::AccessUnit::Descriptor`: The descriptor for the encoded access
   * unit.
   *  - `core::stats::PerfStats`: The performance statistics for the encoding
   * process.
   */
  std::tuple<core::AccessUnit::Descriptor, core::stats::PerfStats> Process(
      const core::record::Chunk& recs) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::name::write_out

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_NAME_WRITE_OUT_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
