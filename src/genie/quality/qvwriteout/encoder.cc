/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder.cc
 *
 * @brief Implementation of the Quality Value Writeout encoder for the Genie
 * framework.
 *
 * This file contains the implementation of the `Encoder` class within the
 * `qvwriteout` namespace. The encoder processes sequencing quality values (QV)
 * and writes them into Access Units (AUs) with customizable quality value
 * configurations.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/qvwriteout/encoder.h"

#include <memory>
#include <string>
#include <utility>

#include "genie/core/cigar_tokenizer.h"
#include "genie/core/record/alignment_split/same_rec.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::quality::qvwriteout {

// -----------------------------------------------------------------------------

void Encoder::SetUpParameters(const core::record::Chunk& rec,
                              paramqv1::QualityValues1& param,
                              core::AccessUnit::Descriptor& desc) {
  paramqv1::ParameterSet set;

  auto codebook = paramqv1::QualityValues1::GetPresetCodebook(
      paramqv1::QualityValues1::QualityParametersPresetId::ASCII);
  set.AddCodeBook(std::move(codebook));

  desc.Add(core::AccessUnit::Subsequence(1, core::gen_sub::kQvPresent));
  desc.Add(core::AccessUnit::Subsequence(1, core::gen_sub::kQvCodebook));
  desc.Add(core::AccessUnit::Subsequence(1, core::gen_sub::kQvSteps0));
  if (rec.GetData().front().GetClassId() == core::record::ClassType::kClassI ||
      rec.GetData().front().GetClassId() == core::record::ClassType::kClassHm) {
    desc.Add(core::AccessUnit::Subsequence(1, core::gen_sub::kQvSteps1));

    codebook = paramqv1::QualityValues1::GetPresetCodebook(
        paramqv1::QualityValues1::QualityParametersPresetId::ASCII);
    set.AddCodeBook(std::move(codebook));
  }

  param.SetQualityParameters(std::move(set));
}

// -----------------------------------------------------------------------------

void Encoder::EncodeAlignedSegment(const core::record::Segment& s,
                                   const std::string& e_cigar,
                                   core::AccessUnit::Descriptor& desc) {
  for (const auto& q : s.GetQualities()) {
    core::CigarTokenizer::Tokenize(
        e_cigar, core::GetECigarInfo(),
        [&desc, &q](const uint8_t cigar, const std::pair<size_t, size_t>& bs,
                    const std::pair<size_t, size_t>&) -> bool {
          const auto qvs = std::string_view(q).substr(bs.first, bs.second);
          const uint8_t codebook =
              core::GetECigarInfo().lut_step_ref[cigar] ||
                      GetAlphabetProperties(core::AlphabetId::kAcgtn)
                          .IsIncluded(static_cast<char>(cigar))
                  ? 2
                  : static_cast<uint8_t>(desc.GetSize()) - 1;
          for (const auto& c : qvs) {
            UTILS_DIE_IF(c < 33 || c > 126, "Invalid quality score");
            desc.Get(codebook).Push(c - 33);
          }
          return true;
        });
  }
}

// -----------------------------------------------------------------------------

void Encoder::EncodeUnalignedSegment(const core::record::Segment& s,
                                     core::AccessUnit::Descriptor& desc) {
  for (const auto& q : s.GetQualities()) {
    for (const auto& c : q) {
      UTILS_DIE_IF(c < 33 || c > 126, "Invalid quality score");
      desc.Get(static_cast<uint16_t>(desc.GetSize()) - 1).Push(c - 33);
    }
  }
}

// -----------------------------------------------------------------------------

core::QvEncoder::qv_coded Encoder::Process(const core::record::Chunk& rec) {
  const util::Watch watch;
  auto param = std::make_unique<paramqv1::QualityValues1>(
      paramqv1::QualityValues1::QualityParametersPresetId::ASCII, false);
  core::AccessUnit::Descriptor desc(core::GenDesc::kQv);

  SetUpParameters(rec, *param, desc);

  for (const auto& r : rec.GetData()) {
    auto& s_first = r.GetSegments()[0];

    if (r.GetAlignments().empty()) {
      EncodeUnalignedSegment(s_first, desc);
    } else {
      EncodeAlignedSegment(
          s_first, r.GetAlignments().front().GetAlignment().GetECigar(), desc);
    }

    if (r.GetSegments().size() == 1) {
      continue;
    }

    auto& s_second = r.GetSegments()[1];

    if (r.GetClassId() == core::record::ClassType::kClassHm ||
        r.GetClassId() == core::record::ClassType::kClassU) {
      EncodeUnalignedSegment(s_second, desc);
    } else {
      EncodeAlignedSegment(
          s_second,
          dynamic_cast<const core::record::alignment_split::SameRec*>(
              r.GetAlignments().front().GetAlignmentSplits().front().get())
              ->GetAlignment()
              .GetECigar(),
          desc);
    }
  }

  core::stats::PerfStats stats;
  stats.AddDouble("time-qv1write-out", watch.Check());
  return std::make_tuple(std::move(param), std::move(desc), stats);
}

// -----------------------------------------------------------------------------

}  // namespace genie::quality::qvwriteout

// -----------------------------------------------------------------------------
