/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
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
#include "genie/util/quantizer.h"
#include "genie/quality/calq/record_pileup.h"

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

struct EncodingOptions;
struct DecodingBlock;
enum struct Version;
// -----------------------------------------------------------------------------

struct EncodingRead {
  uint32_t pos_min;
  uint32_t pos_max;
  std::string qvalues;
  std::string cigar;
  std::string sequence;
  std::string reference;
};

// -----------------------------------------------------------------------------

class QualityEncoder {
 public:
  explicit QualityEncoder(const EncodingOptions& options,
                          const std::map<int, util::Quantizer>& quant,
                          DecodingBlock* output);
  ~QualityEncoder();
  void AddMappedRecordToBlock(EncodingRecord& record);
  void FinishBlock();
  [[nodiscard]] size_t NrMappedRecords() const;

 private:
  void QuantizeUntil(uint64_t pos);
  void EncodeRecords(std::vector<EncodingRecord> records) const;
  void EncodeMappedQuality(const std::string& quality_values,
                           const std::string& cigar, uint64_t pos) const;

  // Sizes & counters
  size_t nr_mapped_records_;
  size_t min_pos_unencoded_;

  int nr_quantizers_;

  // Quality value offset for this block
  uint8_t quality_value_offset_;

  // 0-based position offset of this block
  uint64_t pos_offset_;

  // Pileup
  RecordPileup record_pileup_;

  Haplotyper haplotyper_;
  Genotyper genotyper_;
  DecodingBlock* out_;

  // Quantizers
  std::map<int, util::Quantizer> quantizers_;
};

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_QUALITY_CALQ_QUALITY_ENCODER_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
