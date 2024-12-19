/**
 * Copyright 2018-2024 The Genie Authors.
 * @file quality_encoder.h
 *
 * @brief Defines the QualityEncoder class and related structures for quality
 * value encoding.
 *
 * This file is part of the Genie project, which provides tools for efficient
 * genomic data compression. The `QualityEncoder` class is responsible for
 * encoding the quality values of genomic data while applying advanced
 * quantization, haplotyping, and genotyping techniques. The encoding process is
 * optimized for reduced storage while maintaining fidelity to the original
 * data.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_QUALITY_CALQ_QUALITY_ENCODER_H_
#define SRC_GENIE_QUALITY_CALQ_QUALITY_ENCODER_H_

// -----------------------------------------------------------------------------

#include <map>
#include <string>
#include <vector>

#include "genie/quality/calq/genotyper.h"
#include "genie/quality/calq/haplotyper.h"
#include "genie/quality/calq/record_pileup.h"
#include "genie/util/quantizer.h"

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

/**
 * @brief Represents a single mapped record for quality encoding.
 */
struct EncodingRead {
  /// First mapped position
  uint32_t pos_min;

  /// Last mapped position
  uint32_t pos_max;

  /// Quality values
  std::string qvalues;

  /// CIGAR string
  std::string cigar;

  /// Nucleotide sequence
  std::string sequence;

  /// Nucleotides on the reference
  std::string reference;
};

// -----------------------------------------------------------------------------

class QualityEncoder {
 public:
  /**
   * @brief Constructs a QualityEncoder object for encoding quality values.
   *
   * Initializes the QualityEncoder with the given encoding options, quantizers,
   * and output block. This includes setting up necessary configurations for
   * quality value encoding, haplotyper, and genotyper functionalities.
   *
   * @param options Encoding options that include quality encoding parameters,
   *        such as quantization range and other related configurations.
   * @param quant A map of integer keys to quantizer objects used for quality
   *        value quantization during the encoding process.
   * @param output Pointer to the DecodingBlock that stores the output of the
   *        quality encoding procedure.
   */
  explicit QualityEncoder(const EncodingOptions& options,
                          const std::map<int, util::Quantizer>& quant,
                          DecodingBlock* output);

  /**
   * @brief Adds a mapped record to the current encoding block for processing.
   *
   * This method includes the specified mapped record in the encoding block,
   * updates internal quantizer settings, and processes quality values as
   * necessary. It ensures that the necessary quantization and encoding steps
   * are performed for the added record.
   *
   * @param record The mapped record containing positional, sequence,
   *        quality, and cigar information to be added for encoding.
   */
  void AddMappedRecordToBlock(EncodingRecord& record);

  /**
   * @brief Finalizes the current block of quality encoding.
   *
   * This method processes any remaining data in the record pileup, computes
   * all remaining quantizers, and encodes any unprocessed records. It ensures
   * that all buffered data is properly quantized and encoded before completing
   * the current encoding block.
   */
  void FinishBlock();

  /**
   * @brief Retrieves the number of mapped records that have been processed.
   * @return The number of mapped records stored in the encoder.
   */
  [[nodiscard]] size_t NrMappedRecords() const;

 private:
  /**
   * @brief Quantizes quality values for positions up to the specified position.
   * @param pos Position up to which the quality values should be quantized.
   */
  void QuantizeUntil(uint64_t pos);

  /**
   * @brief Encodes quality values for a list of encoding records.
   * @param records Vector of EncodingRecord objects containing quality values,
   * CIGAR strings, and positions to encode.
   */
  void EncodeRecords(std::vector<EncodingRecord> records) const;

  /**
   * @brief Encode quality values for a mapped record.
   * @param quality_values Quality values to encode.
   * @param cigar CIGAR string for the record.
   * @param pos Position of the record.
   */
  void EncodeMappedQuality(const std::string& quality_values,
                           const std::string& cigar, uint64_t pos) const;

  /// Current number of mapped records
  size_t nr_mapped_records_;

  /// First position with unencoded quality values
  size_t min_pos_unencoded_;

  /// Number of quantizers
  int nr_quantizers_;

  /// Quality value offset
  uint8_t quality_value_offset_;

  /// Position offset for this block
  uint64_t pos_offset_;

  /// Record pileup
  RecordPileup record_pileup_;

  /// Haplotyper
  Haplotyper haplotyper_;

  /// Genotyper
  Genotyper genotyper_;

  /// Output block
  DecodingBlock* out_;

  /// Quantizers for quality values
  std::map<int, util::Quantizer> quantizers_;
};

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_QUALITY_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
