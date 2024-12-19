/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.cc
 *
 * @brief Implements the Decoder class for reconstructing quality values from
 * compressed genomic data.
 *
 * This file is part of the Genie project, which specializes in efficient
 * genomic data compression and decompression workflows. The `Decoder` class is
 * responsible for reversing the compression process by reconstructing quality
 * values from encoded representations, supporting both aligned and unaligned
 * genomic data.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/decoder.h"

#include <limits>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "genie/core/cigar_tokenizer.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------
namespace genie::quality::calq {

// -----------------------------------------------------------------------------
bool Decoder::IsAligned(const core::AccessUnit::Descriptor& desc) {
  return desc.GetSize() != 3;
}

// -----------------------------------------------------------------------------
std::vector<std::string> Decoder::DecodeAligned(
    const paramqv1::QualityValues1& param,
    const std::vector<std::string>& e_cigar_vec,
    const std::vector<uint64_t>& positions,
    core::AccessUnit::Descriptor& desc) {
  // calq variables
  constexpr DecodingOptions options;  // default options
  SideInformation side_information;
  EncodingBlock output;
  DecodingBlock input;

  side_information.pos_offset = positions[0];
  side_information.quality_offset = 0;
  FillInput(input, desc, param);

  // fill sideInformation
  side_information.positions.emplace_back(positions);
  side_information.cigars.emplace_back(e_cigar_vec);

  decode(options, side_information, input, &output);

  return output.quality_values.front();
}

// -----------------------------------------------------------------------------
std::vector<std::string> Decoder::DecodeUnaligned(
    const paramqv1::QualityValues1& param_casted,
    const std::vector<std::string>& e_cigar_vec,
    core::AccessUnit::Descriptor& desc) {
  std::vector<std::string> qv;

  for (const auto& e_cigar : e_cigar_vec) {
    qv.emplace_back();
    core::CigarTokenizer::Tokenize(
        e_cigar, core::GetECigarInfo(),
        [&qv, &desc, &param_casted](const uint8_t cigar,
                                    const std::pair<size_t, size_t>& bs,
                                    const std::pair<size_t, size_t>&) -> bool {
          (void)cigar;
          for (size_t i = 0; i < bs.second; ++i) {
            const auto index = static_cast<uint8_t>(desc.Get(2).Pull());
            qv.back().push_back(static_cast<char>(
                param_casted.GetCodebook(0).GetEntries()[index]));
          }
          return true;
        });
  }
  return qv;
}

// -----------------------------------------------------------------------------
void Decoder::FillInput(DecodingBlock& input,
                        core::AccessUnit::Descriptor& desc,
                        const paramqv1::QualityValues1& param) {
  UTILS_DIE_IF(desc.GetSize() > std::numeric_limits<uint16_t>::max(),
               "Too many descriptors");
  for (uint16_t i = 1; i < static_cast<uint16_t>(desc.GetSize()); ++i) {
    auto data = static_cast<uint8_t*>(desc.Get(i).GetData().GetData());
    const auto size = desc.Get(i).GetData().Size();

    if (i == 1) {
      input.quantizer_indices = std::vector(data, data + size);
    } else {
      input.step_indices.emplace_back(data, data + size);
    }
  }

  // codebooks
  for (size_t i = 0; i < param.GetNumberCodeBooks(); ++i) {
    input.code_books.push_back(param.GetCodebook(i).GetEntries());
  }
}

// -----------------------------------------------------------------------------
std::tuple<std::vector<std::string>, core::stats::PerfStats> Decoder::Process(
    const core::parameter::QualityValues& param,
    const std::vector<std::string>& e_cigar_vec,
    const std::vector<uint64_t>& positions,
    core::AccessUnit::Descriptor& desc) {
  std::vector<std::string> result_qv;
  core::stats::PerfStats stats;
  const util::Watch watch;

  if (desc.IsEmpty()) {
    return std::make_tuple(result_qv, stats);
  }

  const auto& param_casted =
      dynamic_cast<const paramqv1::QualityValues1&>(param);

  if (IsAligned(desc)) {
    result_qv = DecodeAligned(param_casted, e_cigar_vec, positions, desc);
  } else {
    result_qv = DecodeUnaligned(param_casted, e_cigar_vec, desc);
  }

  stats.AddDouble("time-qv-calq", watch.Check());
  return std::make_tuple(result_qv, stats);
}

// -----------------------------------------------------------------------------
}  // namespace genie::quality::calq
