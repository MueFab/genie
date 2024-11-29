/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the Decoder class for handling name decoding in the
 * GENIE framework.
 *
 * This file contains the declaration of the `Decoder` class, which is a
 * specialized implementation of the `NameDecoder` interface within the GENIE
 * framework. The `Decoder` class is responsible for processing encoded
 * descriptors and decoding the names of genomic records according to the GENIE
 * format specifications.
 *
 * @details The `Decoder` class inherits from the `core::NameDecoder` base class
 * and overrides the `process` method to handle name decoding tasks. It utilizes
 * token structures and other utilities from the GENIE library to reconstruct
 * record names from encoded data.
 */

#ifndef SRC_GENIE_NAME_WRITE_OUT_DECODER_H_
#define SRC_GENIE_NAME_WRITE_OUT_DECODER_H_

// -----------------------------------------------------------------------------

#include <string>
#include <tuple>
#include <vector>

#include "genie/core/name_decoder.h"
#include "genie/name/tokenizer/token.h"
#include "genie/util/data_block.h"

// -----------------------------------------------------------------------------

namespace genie::name::write_out {

/**
 * @brief Implements the name decoding functionality for Genie
 *
 * The `Decoder` class is responsible for decoding the names of genomic records
 * from encoded descriptors. It processes input `core::AccessUnit::Descriptor`
 * objects, reconstructs the original names of the records, and returns them in
 * a readable format. The class is designed to work within the `write_out`
 * module of the GENIE library.
 */
class Decoder final : public core::NameDecoder {
 public:
  /**
   * @brief Processes an encoded descriptor and decodes the names of the
   * records.
   *
   * This method takes an encoded `core::AccessUnit::Descriptor` object as
   * input, performs the necessary decoding operations, and returns the decoded
   * names along with performance statistics. The decoding is performed
   * according to the GENIE format specifications.
   *
   * @param desc The descriptor of the encoded access unit to be processed.
   * @return A tuple containing:
   *  - `std::vector<std::string>`: A vector of decoded record names.
   *  - `core::stats::PerfStats`: The performance statistics for the decoding
   * process.
   */
  std::tuple<std::vector<std::string>, core::stats::PerfStats> Process(
      core::AccessUnit::Descriptor& desc) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::name::write_out

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_NAME_WRITE_OUT_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
