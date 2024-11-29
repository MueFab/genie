/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/quality/calq/encoder.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/record/alignment_split/same_rec.h"
#include "genie/quality/calq/calq_coder.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------
namespace genie::quality::calq {

using ClassType = core::record::ClassType;

// -----------------------------------------------------------------------------
paramqv1::Codebook CodebookFromVector(const std::vector<unsigned char>& vec) {
  paramqv1::Codebook codebook(vec[0], vec[1]);
  for (size_t i = 2; i < vec.size(); ++i) {
    codebook.AddEntry(vec[i]);
  }
  return codebook;
}

// -----------------------------------------------------------------------------
core::GenSubIndex get_qv_steps(const size_t i) {
  UTILS_DIE_IF(i > 7, "QV_STEPS index out of range");
  return std::make_pair(core::GenDesc::kQv, static_cast<uint16_t>(i + 2));
}

// -----------------------------------------------------------------------------
void Encoder::FillCalqStructures(const core::record::Chunk& chunk,
                                 EncodingOptions& opt,
                                 SideInformation& side_information,
                                 EncodingBlock& input) {
  // fill calq objects
  for (auto& rec : chunk.GetData()) {
    auto& f_segment = rec.GetSegments().front();
    auto& f_alignment = rec.GetAlignments().front();

    std::vector<std::string> qvalues;
    std::vector<std::string> sequences;
    std::vector<std::string> cigars;
    std::vector<uint64_t> positions;

    positions.push_back(f_alignment.GetPosition());
    cigars.push_back(f_alignment.GetAlignment().GetECigar());
    sequences.push_back(f_segment.GetSequence());
    qvalues.push_back(f_segment.GetQualities().front());

    // add second read info
    if (rec.GetSegments().size() == 2) {
      auto& s_segment = rec.GetSegments()[1];

      UTILS_DIE_IF(
          rec.GetAlignments().front().GetAlignmentSplits().front()->GetType() !=
              core::record::AlignmentSplit::Type::kSameRec,
          "Wrong record type");

      sequences.push_back(s_segment.GetSequence());
      qvalues.push_back(s_segment.GetQualities().front());

      if (rec.GetClassId() == ClassType::kClassHm) {
        positions.push_back(positions.back());
        // create cigar like "x+"
        std::string cigar = std::to_string(qvalues.back().size());
        cigar.append("+");
      } else {
        auto& s_alignment = dynamic_cast<
            core::record::alignment_split::SameRec&>(
            *rec.GetAlignments().front().GetAlignmentSplits().front().get());

        positions.push_back(f_alignment.GetPosition() + s_alignment.GetDelta());
        cigars.push_back(s_alignment.GetAlignment().GetECigar());
      }
    }

    input.quality_values.emplace_back(std::move(qvalues));
    side_information.sequences.emplace_back(std::move(sequences));
    side_information.cigars.emplace_back(std::move(cigars));
    side_information.positions.emplace_back(std::move(positions));
  }

  // set offset and options
  side_information.pos_offset = side_information.positions.front().front();
  opt.quality_value_offset = 0;
  opt.quality_value_min = 33;   // ascii !
  opt.quality_value_max = 126;  // ascii ~

  //    auto classID = chunk.getData().front().getClassID();
  //    if(classID == genie::core::record::ClassType::CLASS_HM || classID ==
  //    genie::core::record::ClassType::CLASS_U
  //    || classID == genie::core::record::ClassType::CLASS_I){
  //        opt.hasUnaligned = true;
  //    }
}

// -----------------------------------------------------------------------------
void Encoder::EncodeAligned(const core::record::Chunk& chunk,
                            paramqv1::QualityValues1& param,
                            core::AccessUnit::Descriptor& desc) {
  // objects required for calq
  EncodingOptions encoding_options;
  SideInformation side_information;
  EncodingBlock input;
  DecodingBlock output;

  FillCalqStructures(chunk, encoding_options, side_information, input);

  encode(encoding_options, side_information, input, &output);

  // add codebooks from calq to param
  paramqv1::ParameterSet set;
  for (auto& code_vec : output.code_books) {
    auto codebook = CodebookFromVector(code_vec);
    set.AddCodeBook(std::move(codebook));
  }
  param.SetQualityParameters(std::move(set));

  // setup descriptor and fill
  desc.Add(core::AccessUnit::Subsequence(1, core::gen_sub::kQvPresent));

  // QV_CODEBOOK
  desc.Add(core::AccessUnit::Subsequence(
      util::DataBlock(&output.quantizer_indices), core::gen_sub::kQvCodebook));

  // fill QV_STEPS_0-7
  for (size_t i = 0; i < output.step_indices.size(); ++i) {
    desc.Add(core::AccessUnit::Subsequence(
        util::DataBlock(&output.step_indices[i]), get_qv_steps(i)));
  }
}

// -----------------------------------------------------------------------------
void Encoder::AddQualities(const core::record::Segment& s,
                           core::AccessUnit::Descriptor& desc,
                           const UniformMinMaxQuantizer& quantizer) {
  auto& subsequence = desc.Get(static_cast<uint16_t>(desc.GetSize()) - 1);

  for (const auto& q : s.GetQualities()) {
    for (const auto& c : q) {
      const auto index = static_cast<uint8_t>(quantizer.ValueToIndex(c));
      subsequence.Push(index);
    }
  }
}

// -----------------------------------------------------------------------------
void Encoder::EncodeUnaligned(const core::record::Chunk& chunk,
                              paramqv1::QualityValues1& param,
                              core::AccessUnit::Descriptor& desc) {
  // create quantizer
  const UniformMinMaxQuantizer quantizer(33, 126, 8);

  // set codebook
  std::vector<uint8_t> codebook_vec;
  for (const auto& [fst, snd] : quantizer.InverseLut()) {
    codebook_vec.push_back(static_cast<uint8_t>(snd));
  }
  auto codebook = CodebookFromVector(codebook_vec);
  paramqv1::ParameterSet set;
  set.AddCodeBook(std::move(codebook));
  param.SetQualityParameters(std::move(set));

  // setup descriptor
  desc.Add(core::AccessUnit::Subsequence(1, core::gen_sub::kQvPresent));
  desc.Add(core::AccessUnit::Subsequence(1, core::gen_sub::kQvCodebook));
  desc.Add(core::AccessUnit::Subsequence(1, core::gen_sub::kQvSteps0));

  // encode values
  for (const auto& rec : chunk.GetData()) {
    for (const auto& seg : rec.GetSegments()) {
      AddQualities(seg, desc, quantizer);
    }
  }
}

// -----------------------------------------------------------------------------
core::QvEncoder::qv_coded Encoder::Process(const core::record::Chunk& chunk) {
  const util::Watch watch;
  auto param = std::make_unique<paramqv1::QualityValues1>(
      paramqv1::QualityValues1::QualityParametersPresetId::ASCII, false);
  core::AccessUnit::Descriptor desc(core::GenDesc::kQv);

  if (const ClassType& class_type = chunk.GetData()[0].GetClassId();
      class_type == ClassType::kClassU) {
    EncodeUnaligned(chunk, *param, desc);
  } else {
    EncodeAligned(chunk, *param, desc);
  }

  core::stats::PerfStats stats;
  stats.AddDouble("time-qvcalq", watch.Check());

  return std::make_tuple(std::move(param), desc, stats);
}

// -----------------------------------------------------------------------------
}  // namespace genie::quality::calq

// -----------------------------------------------------------------------------
