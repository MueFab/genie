/**
 * Copyright 2018-2024 The Genie Authors.
 * @file quality_decoder.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the QualityDecoder class and supporting structures for
 * decoding quality values.
 *
 * This file contains the declaration of the `QualityDecoder` class, which
 * provides methods for decoding mapped and unmapped genomic quality values
 * using quantizers and encoded quality blocks. It handles the conversion of
 * encoded quality values back to their original form using various quantization
 * strategies and maintains state information for each read.
 *
 * @details The `QualityDecoder` class is used to Decode encoded quality blocks
 * into a format that can be used for further processing or storage. It relies
 * on input structures like `DecodingRead`, `EncodingBlock`, and `DecodingBlock`
 * to maintain and Decode quality values. The decoder uses an internal set of
 * quantizers to determine how the quality values should be reconstructed.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_QUALITY_DECODER_H_
#define SRC_GENIE_QUALITY_CALQ_QUALITY_DECODER_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <string>
#include <vector>

#include "genie/quality/calq/calq_coder.h"
#include "genie/util/quantizer.h"

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

/**
 * @brief Stores metadata for decoding a specific read.
 *
 * The `DecodingRead` structure stores the necessary metadata for decoding a
 * single read, including the minimum mapping position (`posMin`) and the
 * corresponding CIGAR string that describes the alignment of the read.
 */
struct DecodingRead {
  /// The 1-based leftmost mapping position of the read.
  uint64_t pos_min;

  /// The CIGAR string for the read, describing alignment operations.
  std::string cigar;
};

/**
 * @brief A decoder class for reconstructing quality values from encoded blocks.
 *
 * The `QualityDecoder` class is responsible for taking encoded quality values
 * and reconstructing them into their original quality score representations. It
 * uses quantizers stored in the `DecodingBlock` to Decode the values according
 * to the specified quantization strategy.
 */
class QualityDecoder {
  /// Offset for read positions to handle global positions correctly.
  uint64_t pos_offset_;

  /// The offset applied to quality values (e.g., 33 for ASCII format).
  int quality_value_offset_;

  /// Indexes of the quantizer values used for decoding.
  std::vector<size_t> qvi_idx_;

  /// The quantizers used to Decode quality values for different segments.
  std::vector<util::Quantizer> quantizers_;

  /// Pointer to the output structure where decoded quality values are stored.
  EncodingBlock* out_;

  /// The input encoded block containing compressed quality values.
  const DecodingBlock& in_;

 public:
  /**
   * @brief Constructs a `QualityDecoder` with the specified input and output
   * blocks.
   *
   * This constructor initializes a `QualityDecoder` object using a reference to
   * the encoded `DecodingBlock` and sets up internal states for decoding. The
   * decoded values are stored in the `EncodingBlock` passed in the `out`
   * parameter.
   *
   * @param input The input encoded block to be decoded.
   * @param position_offset The starting position offset for read positions.
   * @param quality_offset The quality value offset to be applied during
   * decoding.
   * @param output Pointer to the output structure where decoded quality values
   * will be stored.
   */
  QualityDecoder(const DecodingBlock& input, uint64_t position_offset,
                 uint8_t quality_offset, EncodingBlock* output);

  /**
   * @brief Destructor for the `QualityDecoder` class.
   *
   * Cleans up the internal states and pointers used by the `QualityDecoder`.
   */
  ~QualityDecoder();

  /**
   * @brief Decodes a mapped record using information from the encoded block.
   *
   * This function takes a `DecodingRead` structure, extracts relevant metadata,
   * and uses it to Decode quality values for a single mapped record. The
   * decoded values are stored in the `EncodingBlock` that was provided during
   * construction.
   *
   * @param sam_record The `DecodingRead` structure containing metadata for the
   * read.
   */
  void DecodeMappedRecordFromBlock(const DecodingRead& sam_record);
};

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_QUALITY_DECODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
