/**
 * Copyright 2018-2024 The Genie Authors.
 * @file calq_coder.cc
 *
 * @brief Implements the core encoding and decoding functions for genomic
 * quality values in Calq.
 *
 * This file is part of the Genie project, designed to compress and reconstruct
 * quality values associated with genomic sequences. The `calq_coder.cpp` file
 * provides essential functionality for both encoding and decoding workflows,
 * supporting multiple quantization techniques.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/calq_coder.h"

#include <map>
#include <utility>

#include "genie/quality/calq/quality_decoder.h"
#include "genie/quality/calq/quality_encoder.h"
#include "genie/util/lloyd_max_quantizer.h"
#include "genie/util/runtime_exception.h"
#include "genie/util/uniform_min_max_quantizer.h"

// -----------------------------------------------------------------------------

namespace genie::quality::calq {

// -----------------------------------------------------------------------------

void encode(const EncodingOptions& opt, const SideInformation& side_information,
            const EncodingBlock& input, DecodingBlock* output) {
  util::ProbabilityDistribution pdf(opt.quality_value_min,
                                    opt.quality_value_max);

  // Check quality value range
  for (const auto& sam_record : input.quality_values) {
    for (const auto& read : sam_record) {
      for (const auto& q : read) {
        UTILS_DIE_IF(q < opt.quality_value_min || q > opt.quality_value_max,
                     "Quality value out of range");
        pdf.AddToPdf(static_cast<size_t>(q) - opt.quality_value_offset);
      }
    }
  }

  std::map<int, util::Quantizer> quantizers;

  for (auto i = static_cast<int>(opt.quantization_min);
       i <= static_cast<int>(opt.quantization_max); ++i) {
    if (opt.quantizer_type == QuantizerType::UNIFORM) {
      util::UniformMinMaxQuantizer quantizer(
          static_cast<const int&>(opt.quality_value_min),
          static_cast<const int&>(opt.quality_value_max), i);
      quantizers.insert(std::pair<int, util::Quantizer>(
          static_cast<const int&>(i - opt.quantization_min), quantizer));
    } else if (opt.quantizer_type == QuantizerType::LLOYD_MAX) {
      util::LloydMaxQuantizer quantizer(static_cast<size_t>(i));
      quantizer.build(pdf);
      quantizers.insert(std::pair<int, util::Quantizer>(
          static_cast<const int&>(i - opt.quantization_min), quantizer));
    } else {
      UTILS_DIE("Quantization Type not supported");
    }
  }

  // Encode the quality values
  QualityEncoder quality_encoder(opt, quantizers, output);

  for (size_t i = 0; i < side_information.positions.size(); ++i) {
    EncodingRecord record = {
        input.quality_values[i], side_information.sequences[i],
        side_information.cigars[i], side_information.positions[i]};
    quality_encoder.AddMappedRecordToBlock(record);
  }

  quality_encoder.FinishBlock();
}

// -----------------------------------------------------------------------------

void decode(const DecodingOptions&, const SideInformation& side_information,
            const DecodingBlock& input, EncodingBlock* output,
            core::AccessUnit::Subsequence& present) {
  // Decode the quality values
  QualityDecoder quality_decoder(input, side_information.positions[0][0],
                                 side_information.quality_offset, output);
  output->quality_values.clear();
  output->quality_values.emplace_back();
  for (size_t i = 0; i < side_information.positions[0].size(); ++i) {
    if (!present.end() && !present.Pull()) {
      quality_decoder.DecodeDummy();
      continue;
    }
    DecodingRead r = {side_information.positions[0][i],
                      side_information.cigars[0][i]};
    quality_decoder.DecodeMappedRecordFromBlock(r);
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
