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
  std::vector<std::vector<uint64_t>>
      positions;  //!< @brief 1-based leftmost mapping position for each read.
  std::vector<std::vector<std::string>>
      sequences;  //!< @brief Base sequences for each read.
  std::vector<std::vector<std::string>>
      cigars;  //!< @brief CIGAR strings for each read, used in encoding.
  std::string reference;  //!< @brief Reference sequence for CALQ V2. Can remain
                          //!< empty for CALQ V1.
  uint64_t
      pos_offset;  //!< @brief First position of the read (including unmapped).
  uint8_t quality_offset;  //!< @brief Quality value format offset.

  /**
   * @brief Default constructor for initializing empty side information.
   */
  SideInformation() : pos_offset(0), quality_offset(33) {}
};

// -----------------------------------------------------------------------------
/**
 * @brief Represents a block of quality values to be encoded.
 *
 * The `EncodingBlock` struct contains sequences of quality values for each
 * read. Each sequence corresponds to a read and holds the quality values
 * associated with the bases in the sequence.
 */
struct EncodingBlock {
  std::vector<std::vector<std::string>>
      quality_values;  //!< @brief Sequences of quality values for each read.
};

// -----------------------------------------------------------------------------
/**
 * @brief Stores data for multiple reads in an encoding record.
 *
 * The `EncodingRecord` struct contains quality values, base sequences, CIGAR
 * strings, and positions for multiple reads, allowing them to be processed as a
 * single record.
 */
struct EncodingRecord {
  std::vector<std::string>
      quality_values;                  //!< @brief Quality values for each read.
  std::vector<std::string> sequences;  //!< @brief Base sequences for each read.
  std::vector<std::string> cigars;     //!< @brief CIGAR strings for each read.
  std::vector<uint64_t>
      positions;  //!< @brief 1-based leftmost mapping position for each read.
};

// -----------------------------------------------------------------------------
/**
 * @brief Enumeration for quantizer types.
 *
 * Defines the types of quantizers used in the CALQ algorithm.
 */
enum struct QuantizerType {
  UNIFORM,
  LLOYD_MAX
};  //!< @brief Static uniform quantizer or adaptive Lloyd-Max quantizer.

// -----------------------------------------------------------------------------
/**
 * @brief Enumeration for filter types used in CALQ V2.
 *
 * Specifies the type of filter applied to smooth activity values in CALQ V2.
 */
enum struct FilterType {
  GAUSS,
  RECTANGLE
};  //!< @brief Gaussian smoothing filter or rectangular smoothing filter.

// -----------------------------------------------------------------------------
/**
 * @brief Enumeration for CALQ versions.
 *
 * Specifies which CALQ version to use for encoding and decoding.
 */
enum struct Version {
  V1,
  V2
};  //!< @brief V1: No reference, V2: Reference-based.

// -----------------------------------------------------------------------------
/**
 * @brief Options for encoding quality values.
 *
 * The `EncodingOptions` struct holds configuration parameters used during
 * encoding. These options define the behavior of the encoder and the properties
 * of the encoded output.
 */
struct EncodingOptions {
  bool debug_pileup = false;  //!< @brief Output additional pileup information.
  bool squash =
      true;  //!< @brief Squash activity values to [0.0, 1.0] in CALQ V2.
  size_t filter_size =
      17;  //!< @brief Filter Size (sigma for Gauss, radius for Rect).
  size_t filter_cut_off = 50;  //!< @brief Filter cutoff value (kernel radius).
  uint8_t hq_soft_clip_threshold =
      29;  //!< @brief High-quality soft-clip threshold.
  size_t hq_soft_clip_propagation =
      50;  //!< @brief Soft-clip propagation length.
  size_t hq_soft_clip_streak =
      7;  //!< @brief Number of soft-clips needed to trigger propagation.
  uint8_t quantization_min = 2;  //!< @brief Minimum quantization step number.
  uint8_t quantization_max = 8;  //!< @brief Maximum quantization step number.
  uint8_t polyploidy = 2;  //!< @brief Polyploidy of the biological data source.
  uint8_t quality_value_max =
      41;  //!< @brief Maximum quality value for the format.
  uint8_t quality_value_min =
      0;  //!< @brief Minimum quality value for the format.
  uint8_t quality_value_offset = 33;  //!< @brief Quality value format offset.
  FilterType filter_type =
      FilterType::GAUSS;  //!< @brief Filter type used for smoothing.
  QuantizerType quantizer_type =
      QuantizerType::UNIFORM;     //!< @brief Quantizer type used.
  Version version = Version::V1;  //!< @brief CALQ version used for encoding.
  bool has_unaligned =
      false;  //!< @brief Flag indicating the presence of unaligned reads.
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
  std::vector<uint8_t>
      quantizer_indices;  //!< @brief Quantizer selection indices.
  std::vector<std::vector<uint8_t>>
      step_indices;  //!< @brief Step indices for each quantizer.
  std::vector<std::vector<uint8_t>>
      code_books;  //!< @brief Quantizer representative values.
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
