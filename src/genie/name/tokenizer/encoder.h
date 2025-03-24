/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the Encoder class for handling token-based name
 * encoding in the GENIE framework.
 *
 * This file contains the declaration of the `Encoder` class, which is a
 * specialized implementation of the `NameEncoder` interface. The `Encoder`
 * class is responsible for processing genomic record chunks and encoding their
 * names into a tokenized format using the GENIE framework's tokenizer.
 *
 * @details The `Encoder` class provides a token-based approach to name
 * encoding, breaking down genomic record names into discrete tokens for
 * efficient encoding and storage. It is designed to work within the `tokenizer`
 * module of the GENIE library.
 */

#ifndef SRC_GENIE_NAME_TOKENIZER_ENCODER_H_
#define SRC_GENIE_NAME_TOKENIZER_ENCODER_H_

// -----------------------------------------------------------------------------

#include <tuple>

#include "genie/core/name_encoder.h"
#include "genie/name/tokenizer/tokenizer.h"

// -----------------------------------------------------------------------------

namespace genie::name::tokenizer {

/**
 * @brief Implements the token-based name encoding functionality for Genie
 *
 * The `Encoder` class is responsible for encoding the names of genomic records
 * using a token-based approach. It processes input record chunks, generates
 * tokens for the record names, and organizes the encoded data into an
 * `AccessUnit::Descriptor` object. The class is designed to work within the
 * `tokenizer` module of the GENIE library.
 */
class Encoder final : public core::NameEncoder {
 public:
  /**
   * @brief Processes a chunk of genomic records and encodes their names into
   * tokens.
   *
   * This method takes a chunk of genomic records as input, performs
   * tokenization of the record names, and encodes them using the GENIE
   * tokenizer. It returns the resulting descriptor along with performance
   * statistics, which can be used to evaluate the efficiency of the encoding
   * process.
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

}  // namespace genie::name::tokenizer

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_NAME_TOKENIZER_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
