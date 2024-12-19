/**
 * Copyright 2018-2024 The Genie Authors.
 * @file calq_coder.h
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 *
 * @brief Declaration of data structures and functions for quality value
 * encoding and decoding using CALQ.
 *
 * This file contains the declaration of data structures and functions used for
 * encoding and decoding quality values using CALQ algorithms. The CALQ coding
 * techniques are used to Compress and reconstruct quality values in genomic
 * sequences based on various parameters and configuration options.
 *
 * @details The CALQ coder provides two main functionalities: encoding and
 * decoding of quality values. It supports both aligned and unaligned sequences,
 * and various CALQ versions (e.g., CALQ V1 and V2) with different quantization
 * and filtering techniques.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_CALQ_CODER_H_
#define SRC_GENIE_QUALITY_CALQ_CALQ_CODER_H_

// -----------------------------------------------------------------------------

#include <cstdint>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------
namespace genie::quality::calq {

/**
 * @brief Side information used to encode and Decode quality values.
 *
 * The `SideInformation` struct contains alignment and sequence-related data
 * used during the encoding and decoding of quality values. It is not encoded
 * itself but is required to guide the compression process and ensure accurate
 * reconstruction.
 */
struct SideInformation {
  /// 1-based leftmost mapping position for each read.
  std::vector<std::vector<uint64_t>> positions;

  /// Base sequences for each read.
  std::vector<std::vector<std::string>> sequences;

  /// CIGAR strings for each read, used in encoding.
  std::vector<std::vector<std::string>> cigars;

  /// Reference sequence for CALQ V2. Can remain empty for CALQ V1.
  std::string reference;

  /// First position of the read (including unmapped).
  uint64_t pos_offset;

  /// Quality value format offset.
  uint8_t quality_offset;

  /**
   * @brief Default constructor for initializing empty side information.
   */
  SideInformation() : pos_offset(0), quality_offset(33) {}
};

/**
 * @brief Represents a block of quality values to be encoded.
 *
 * The `EncodingBlock` struct contains sequences of quality values for each
 * read. Each sequence corresponds to a read and holds the quality values
 * associated with the bases in the sequence.
 */
struct EncodingBlock {
  /// Sequences of quality values for each read.
  std::vector<std::vector<std::string>> quality_values;
};

/**
 * @brief Stores data for multiple reads in an encoding record.
 *
 * The `EncodingRecord` struct contains quality values, base sequences, CIGAR
 * strings, and positions for multiple reads, allowing them to be processed as a
 * single record.
 */
struct EncodingRecord {
  /// Quality values for each read.
  std::vector<std::string> quality_values;

  /// Base sequences for each read.
  std::vector<std::string> sequences;

  /// CIGAR strings for each read.
  std::vector<std::string> cigars;

  /// 1-based leftmost mapping position for each read.
  std::vector<uint64_t> positions;
};

/**
 * @brief Enumeration for quantizer types.
 *
 * Defines the types of quantizers used in the CALQ algorithm.
 */
enum struct QuantizerType {
  /// Static uniform quantizer
  UNIFORM,

  /// Adaptive Lloyd-Max quantizer.
  LLOYD_MAX
};

// -----------------------------------------------------------------------------
/**
 * @brief Enumeration for filter types used in CALQ V2.
 *
 * Specifies the type of filter applied to smooth activity values in CALQ V2.
 */
enum struct FilterType {
  /// Gaussian smoothing filter
  GAUSS,

  /// Rectangular smoothing filter
  RECTANGLE
};

// -----------------------------------------------------------------------------
/**
 * @brief Enumeration for CALQ versions.
 *
 * Specifies which CALQ version to use for encoding and decoding.
 */
enum struct Version {
  /// V1: No reference
  V1,

  /// V2: Reference-based.
  V2
};

// -----------------------------------------------------------------------------
/**
 * @brief Options for encoding quality values.
 *
 * The `EncodingOptions` struct holds configuration parameters used during
 * encoding. These options define the behavior of the encoder and the properties
 * of the encoded output.
 */
struct EncodingOptions {
  /// Output additional pileup information.
  bool debug_pileup = false;

  /// Squash activity values to [0.0, 1.0] in CALQ V2.
  bool squash = true;

  /// Filter Size (sigma for Gauss, radius for Rect).
  size_t filter_size = 17;

  /// Filter cutoff value (kernel radius).
  size_t filter_cut_off = 50;

  /// High-quality soft-clip threshold.
  uint8_t hq_soft_clip_threshold = 29;

  /// Soft-clip propagation length.
  size_t hq_soft_clip_propagation = 50;

  /// Number of soft-clips needed to trigger propagation.
  size_t hq_soft_clip_streak = 7;

  /// Minimum quantization step number.
  uint8_t quantization_min = 2;

  /// Maximum quantization step number.
  uint8_t quantization_max = 8;

  /// Polyploidy of the biological data source.
  uint8_t polyploidy = 2;

  /// Maximum quality value for the format.
  uint8_t quality_value_max = 41;

  /// Minimum quality value for the format.
  uint8_t quality_value_min = 0;

  /// Quality value format offset.
  uint8_t quality_value_offset = 33;

  /// Filter type used for smoothing.
  FilterType filter_type = FilterType::GAUSS;

  /// Quantizer type used.
  QuantizerType quantizer_type = QuantizerType::UNIFORM;

  /// CALQ version used for encoding.
  Version version = Version::V1;

  /// Flag indicating the presence of unaligned reads.
  bool has_unaligned = false;
};

// -----------------------------------------------------------------------------
/**
 * @brief Options for decoding quality values.
 *
 * The `DecodingOptions` struct is a placeholder for parameters used during the
 * decoding process. Additional fields can be added in the future to control
 * decoding behavior.
 */
struct DecodingOptions {};

// -----------------------------------------------------------------------------
/**
 * @brief Encoded streams for decoding by CALQ.
 *
 * The `DecodingBlock` struct contains encoded quality value streams that can be
 * decoded using the CALQ algorithm.
 */
struct DecodingBlock {
  /// Quantizer selection indices.
  std::vector<uint8_t> quantizer_indices;

  /// Step indices for each quantizer.
  std::vector<std::vector<uint8_t>> step_indices;

  /// Quantizer representative values.
  std::vector<std::vector<uint8_t>> code_books;
};

// -----------------------------------------------------------------------------
/**
 * @brief Decodes quality values using the provided decoding options and side
 * information.
 *
 * @param opt The decoding options to use.
 * @param side_information The alignment and reference data needed for decoding.
 * @param input The encoded quality values to be decoded.
 * @param output The output structure to store the decoded quality values.
 */
void decode(const DecodingOptions& opt, const SideInformation& side_information,
            const DecodingBlock& input, EncodingBlock* output);

// -----------------------------------------------------------------------------
/**
 * @brief Encodes quality values using the provided encoding options and side
 * information.
 *
 * @param opt The encoding options to use.
 * @param side_information The alignment and reference data needed for encoding.
 * @param input The quality values to be encoded.
 * @param output The output structure to store the encoded quality values.
 */
void encode(const EncodingOptions& opt, const SideInformation& side_information,
            const EncodingBlock& input, DecodingBlock* output);

// -----------------------------------------------------------------------------
/**
 * @brief Encodes unaligned quality values using the provided encoding options.
 *
 * This function handles the encoding of quality values for reads that are not
 * aligned to a reference.
 *
 * @param opt The encoding options to use.
 * @param input The unaligned quality values to be encoded.
 * @param output The output structure to store the encoded quality values.
 */
void EncodeUnaligned(const EncodingOptions& opt, const EncodingBlock& input,
                     DecodingBlock* output);

// -----------------------------------------------------------------------------
}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
#endif  // SRC_GENIE_QUALITY_CALQ_CALQ_CODER_H_
