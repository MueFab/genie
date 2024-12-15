/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of the Encoder class for encoding quality values using
 * CALQ.
 *
 * This file contains the declaration of the `Encoder` class, which provides
 * functionalities for encoding quality values using the CALQ algorithm. It uses
 * various configurations and encoding techniques to Compress quality values in
 * genomic sequences, either aligned or unaligned.
 *
 * @details The `Encoder` class extends the base `core::QVEncoder` class and
 * implements the process method for handling different types of sequences. It
 * uses various helper methods to structure the input data and manage quality
 * values, and relies on the `UniformMinMaxQuantizer` for efficient quality
 * value quantization.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_ENCODER_H_
#define SRC_GENIE_QUALITY_CALQ_ENCODER_H_

// -----------------------------------------------------------------------------

#include "genie/core/qv_encoder.h"
#include "genie/quality/calq/calq_coder.h"
#include "genie/util/uniform_min_max_quantizer.h"
#include "genie/quality/paramqv1/qv_coding_config_1.h"

// -----------------------------------------------------------------------------
namespace genie::quality::calq {

/**
 * @brief Encoder for quality values using the CALQ algorithm.
 *
 * The `Encoder` class is responsible for encoding quality values in genomic
 * sequences using the CALQ algorithm. It supports both aligned and unaligned
 * sequences and employs specialized techniques such as quantization to achieve
 * high compression ratios while preserving the quality value information.
 */
class Encoder final : public core::QvEncoder {
  /**
   * @brief Fills the internal CALQ structures with data from the given chunk.
   *
   * This method processes the input `Chunk` and extracts relevant side
   * information, including positions, sequences, and CIGAR strings. It also
   * configures the CALQ encoding options and prepares the `EncodingBlock`
   * structure for encoding.
   *
   * @param chunk The input chunk containing genomic records to encode.
   * @param opt The CALQ encoding options to use.
   * @param side_information The side information extracted from the input
   * chunk.
   * @param input The encoding block structure to be filled with quality values.
   */
  static void FillCalqStructures(const core::record::Chunk& chunk,
                                 EncodingOptions& opt,
                                 SideInformation& side_information,
                                 EncodingBlock& input);

  /**
   * @brief Encodes quality values for aligned sequences.
   *
   * This method handles the encoding of quality values for sequences that are
   * aligned to a reference. It uses the CALQ algorithm to Compress the quality
   * values based on alignment positions and CIGAR strings.
   *
   * @param chunk The input chunk containing aligned genomic records.
   * @param param The quality value coding configuration.
   * @param desc The access unit descriptor to store the encoded quality values.
   */
  static void EncodeAligned(const core::record::Chunk& chunk,
                            paramqv1::QualityValues1& param,
                            core::AccessUnit::Descriptor& desc);

  /**
   * @brief Adds the quality values of a segment to the access unit descriptor.
   *
   * This method extracts the quality values of a given segment and uses the
   * provided quantizer to Compress and add them to the access unit descriptor.
   *
   * @param s The segment whose quality values are to be added.
   * @param desc The access unit descriptor to store the encoded values.
   * @param quantizer The quantizer used to Compress the quality values.
   */
  static void AddQualities(const core::record::Segment& s,
                           core::AccessUnit::Descriptor& desc,
                           const util::UniformMinMaxQuantizer& quantizer);

  /**
   * @brief Encodes quality values for unaligned sequences.
   *
   * This method handles the encoding of quality values for unaligned sequences,
   * where no reference or alignment positions are available. It uses internal
   * structures and configurations to Compress the quality values and stores
   * them in the access unit descriptor.
   *
   * @param chunk The input chunk containing unaligned genomic records.
   * @param param The quality value coding configuration.
   * @param desc The access unit descriptor to store the encoded quality values.
   */
  static void EncodeUnaligned(const core::record::Chunk& chunk,
                              paramqv1::QualityValues1& param,
                              core::AccessUnit::Descriptor& desc);

 public:
  /**
   * @brief Processes the input chunk and encodes the quality values.
   *
   * This method implements the core functionality for encoding quality values.
   * It determines whether the sequences in the chunk are aligned or unaligned,
   * and applies the appropriate encoding strategy. The method returns the
   * encoded quality values along with metadata for the access unit descriptor.
   *
   * @param chunk The input chunk containing genomic records to encode.
   * @return A structure containing the encoded quality values and metadata.
   */
  qv_coded Process(const core::record::Chunk& chunk) override;
};

// -----------------------------------------------------------------------------
}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
#endif  // SRC_GENIE_QUALITY_CALQ_ENCODER_H_
