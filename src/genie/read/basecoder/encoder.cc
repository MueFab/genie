/**
 * Copyright 2018-2024 The Genie Authors.
 * @file encoder.cc
 * @brief Implementation of the Encoder class for encoding aligned genomic
 * reads.
 *
 * This file contains the implementation of the Encoder class, which is
 * responsible for encoding aligned genomic reads into a specific data format
 * using the GENIE framework. It handles various operations such as encoding
 * CIGAR strings, soft and hard clips, and different alignment manipulations.
 *
 * @details The Encoder class encapsulates the functionality for transforming
 * read data into a compressed format. It operates on records defined in the
 * GENIE core and utilizes the core::AccessUnit structure for managing encoded
 * data streams. The class includes methods for encoding different segments of a
 * record, handling paired and unpaired reads, and managing clipping
 * information.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#include "genie/read/basecoder/encoder.h"

#include <algorithm>
#include <array>
#include <string>
#include <utility>

#include "genie/core/parameter/parameter_set.h"
#include "genie/core/record/alignment_box.h"
#include "genie/core/record/alignment_split/other_rec.h"
#include "genie/core/record/alignment_split/same_rec.h"

// -----------------------------------------------------------------------------

namespace genie::read::basecoder {

// -----------------------------------------------------------------------------

Encoder::CodingState::CodingState(const std::string& read_seq,
                                  const std::string& ref_name,
                                  const core::record::ClassType c_type)
    : count(0),
      read_pos(0),
      ref_offset(0),
      last_mismatch(0),
      is_right_clip(false),
      read(read_seq),
      ref(ref_name),
      type(c_type) {}

// -----------------------------------------------------------------------------

Encoder::Encoder(const uint64_t starting_mapping_pos)
    : container_(core::parameter::EncodingSet(), 0),
      pos_(starting_mapping_pos),
      read_counter_(0) {}

// -----------------------------------------------------------------------------

void Encoder::EncodeFirstSegment(const core::record::Record& rec) {
  // TODO(Fabian): Splices
  const auto& alignment =
      rec.GetAlignments().front();  // TODO(Fabian): Multiple alignments.
                                    // Currently only 1 supported

  if (rec.GetClassId() == core::record::ClassType::kClassHm) {
    container_.Push(core::gen_sub::kRtype, 6);
  } else if (rec.GetClassId() == core::record::ClassType::kClassU) {
    container_.Push(core::gen_sub::kRtype, 5);
  } else {
    container_.Push(core::gen_sub::kRtype,
                    static_cast<uint8_t>(rec.GetClassId()));
  }

  const auto position = alignment.GetPosition() - pos_;
  pos_ = alignment.GetPosition();
  container_.Push(core::gen_sub::kPositionFirst, position);

  const auto reverse_comp = alignment.GetAlignment().GetRComp();
  container_.Push(core::gen_sub::kReverseComplement, reverse_comp);

  const auto flag_pcr =
      (rec.GetFlags() & core::gen_const::kFlagsPcrDuplicateMask) >>
      core::gen_const::kFlagsPcrDuplicatePos;
  container_.Push(core::gen_sub::kFlagsPcrDuplicate, flag_pcr);
  const auto flag_quality =
      (rec.GetFlags() & core::gen_const::kFlagsQualityFailMask) >>
      core::gen_const::kFlagsQualityFailPos;
  container_.Push(core::gen_sub::kFlagsQualityFail, flag_quality);
  const auto flag_pair =
      (rec.GetFlags() & core::gen_const::kFlagsProperPairMask) >>
      core::gen_const::kFlagsProperPairPos;
  container_.Push(core::gen_sub::kFlagsProperPair, flag_pair);
  const auto flag_not_primary =
      (rec.GetFlags() & core::gen_const::kFlagsNotPrimaryMask) >>
      core::gen_const::kFlagsNotPrimaryPos;
  container_.Push(core::gen_sub::kFlagsNotPrimary, flag_not_primary);
  const auto flag_supplementary =
      (rec.GetFlags() & core::gen_const::kFlagsSupplementaryMask) >>
      core::gen_const::kFlagsNotPrimaryPos;
  container_.Push(core::gen_sub::kFlagsSupplementary, flag_supplementary);

  const auto mapping_score =
      alignment.GetAlignment().GetMappingScores().empty()
          ? 255
          : alignment.GetAlignment()
                .GetMappingScores()
                .front();  // TODO(Fabian): Multiple mapping scores
  container_.Push(core::gen_sub::kMappingScore, mapping_score);
}

// -----------------------------------------------------------------------------

const core::record::alignment_split::SameRec& Encoder::ExtractPairedAlignment(
    const core::record::Record& rec) {
  // TODO(Fabian): More than 2 split alignments (even supported by standard?)
  // TODO(Fabian): Multi alignments
  constexpr auto same_rec = core::record::AlignmentSplit::Type::kSameRec;
  UTILS_DIE_IF(
      rec.GetAlignments().front().GetAlignmentSplits().front()->GetType() !=
          same_rec,
      "Only same record split alignments supported");
  const auto alignment =
      rec.GetAlignments().front().GetAlignmentSplits().front().get();
  return *reinterpret_cast<const core::record::alignment_split::SameRec*>(
      alignment);
}

// -----------------------------------------------------------------------------

void Encoder::EncodeAdditionalSegment(
    const core::record::alignment_split::SameRec& split_rec,
    const bool first1) {

  const auto reverse_comp = split_rec.GetAlignment().GetRComp();
  container_.Push(core::gen_sub::kReverseComplement, reverse_comp);

  // TODO(Fabian): MappingScore depth != 1
  const auto mapping_score =
      split_rec.GetAlignment().GetMappingScores().front();
  container_.Push(core::gen_sub::kMappingScore, mapping_score);

  container_.Push(core::gen_sub::kPairDecodingCase,
                  core::gen_const::kPairSameRecord);

  const auto delta = split_rec.GetDelta();
  const auto same_rec_data =
      delta << 1u |
      static_cast<uint32_t>(
          !first1);  // FIRST1 is encoded in the least significant bit
  container_.Push(core::gen_sub::kPairSameRec, same_rec_data);
}

// -----------------------------------------------------------------------------

void Encoder::Add(const core::record::Record& rec, const std::string& ref1,
                  const std::string& ref2) {
  std::pair<ClipInformation, ClipInformation> clips;

  EncodeFirstSegment(rec);

  const auto& sequence = rec.GetSegments()[0].GetSequence();
  const auto& cigar = rec.GetAlignments()
                          .front()
                          .GetAlignment()
                          .GetECigar();  // TODO(Fabian): Multi-alignments
  clips.first = EncodeCigar(sequence, cigar, ref1, rec.GetClassId());

  const auto length_var_1 = rec.GetSegments()[0].GetSequence().length() - 1;
  const auto length_const_1 =
      length_var_1 + 1 + clips.first.hard_clips[0] + clips.first.hard_clips[1];
  UpdateLength(length_const_1);
  container_.Push(core::gen_sub::kReadLength, length_var_1);

  // Check if record is paired
  if (rec.GetSegments().size() > 1) {
    if (rec.GetClassId() == core::record::ClassType::kClassHm) {
      container_.Push(core::gen_sub::kMismatchPosTerminator,
                    core::gen_const::kMismatchPosTerminate);
      const auto& sequence2 = rec.GetSegments()[1].GetSequence();

      const auto length_var_2 = rec.GetSegments()[1].GetSequence().length() - 1;
      const auto length_const_2 = length_var_2 + 1;
      UpdateLength(length_const_2);
      container_.Push(core::gen_sub::kReadLength, length_var_2);
      container_.Push(core::gen_sub::kPairSameRec, !rec.IsRead1First());
      const std::string cigar2 = std::to_string(length_const_2) + "=";
      EncodeCigar(sequence2, cigar2, ref2, rec.GetClassId());
    } else {
      // Same record
      const core::record::alignment_split::SameRec& split_rec =
          ExtractPairedAlignment(rec);
      const auto& sequence2 = rec.GetSegments()[1].GetSequence();
      EncodeAdditionalSegment(split_rec, rec.IsRead1First());

      const auto& cigar2 = split_rec.GetAlignment().GetECigar();
      clips.second = EncodeCigar(sequence2, cigar2, ref2, rec.GetClassId());

      const auto length_var_2 = rec.GetSegments()[1].GetSequence().length() - 1;
      const auto length_const_2 = length_var_2 + 1 +
                                  clips.second.hard_clips[0] +
                                  clips.second.hard_clips[1];
      UpdateLength(length_const_2);
      container_.Push(core::gen_sub::kReadLength, length_var_2);

      const auto length2 = sequence2.length() + clips.second.hard_clips[0] +
                           clips.second.hard_clips[1];
      container_.Push(core::gen_sub::kReadLength, length2);
    }
  } else if (rec.GetNumberOfTemplateSegments() > 1) {
    // Unpaired
    if (rec.GetAlignments().front().GetAlignmentSplits().front()->GetType() ==
        core::record::AlignmentSplit::Type::kUnpaired) {
      if (rec.IsRead1First()) {
        container_.Push(core::gen_sub::kPairDecodingCase,
                        core::gen_const::kPairR1Unpaired);
      } else {
        container_.Push(core::gen_sub::kPairDecodingCase,
                        core::gen_const::kPairR2Unpaired);
      }
      // Other record
    } else {
      const auto alignment =
          rec.GetAlignments().front().GetAlignmentSplits().front().get();
      const auto split =
          *reinterpret_cast<const core::record::alignment_split::OtherRec*>(
              alignment);
      if (rec.IsRead1First()) {
        if (split.GetNextSeq() != rec.GetAlignmentSharedData().GetSeqId()) {
          container_.Push(core::gen_sub::kPairDecodingCase,
                          core::gen_const::kPairR2DiffRef);
          container_.Push(core::gen_sub::kPairR2DiffSeq, split.GetNextSeq());
          container_.Push(core::gen_sub::kPairR2DiffPos, split.GetNextPos());
        } else {
          container_.Push(core::gen_sub::kPairDecodingCase,
                          core::gen_const::kPairR2Split);
          container_.Push(core::gen_sub::kPairR2Split, split.GetNextPos());
        }
      } else {
        if (split.GetNextSeq() != rec.GetAlignmentSharedData().GetSeqId()) {
          container_.Push(core::gen_sub::kPairDecodingCase,
                          core::gen_const::kPairR1DiffRef);
          container_.Push(core::gen_sub::kPairR1DiffSeq, split.GetNextSeq());
          container_.Push(core::gen_sub::kPairR1DiffPos, split.GetNextPos());
        } else {
          container_.Push(core::gen_sub::kPairDecodingCase,
                          core::gen_const::kPairR1Split);
          container_.Push(core::gen_sub::kPairR1Split, split.GetNextPos());
        }
      }
    }
  }

  EncodeClips(clips);

  container_.AddRecord();
}

// -----------------------------------------------------------------------------

void Encoder::EncodeInsertion(CodingState& state) {
  for (size_t i = 0; i < state.count; ++i) {
    container_.Push(core::gen_sub::kMismatchPosTerminator,
                    core::gen_const::kMismatchPositionPersist);

    const auto position = state.read_pos - state.last_mismatch -
                          state.clips.soft_clips[0].length();
    state.last_mismatch =
        state.read_pos + 1 - state.clips.soft_clips[0].length();
    container_.Push(core::gen_sub::kMismatchPosDelta, position);

    container_.Push(core::gen_sub::kMismatchType,
                    core::gen_const::kMismatchTypeInsertion);

    const auto symbol = GetAlphabetProperties(core::AlphabetId::kAcgtn)
                            .inverse_lut[state.read[state.read_pos]];
    state.read_pos++;
    container_.Push(core::gen_sub::kMismatchTypeInsert, symbol);
  }
}

// -----------------------------------------------------------------------------

void Encoder::EncodeDeletion(CodingState& state) {
  for (size_t i = 0; i < state.count; ++i) {
    container_.Push(core::gen_sub::kMismatchPosTerminator,
                    core::gen_const::kMismatchPositionPersist);

    const auto position = state.read_pos - state.last_mismatch -
                          state.clips.soft_clips[0].length();
    state.last_mismatch = state.read_pos - state.clips.soft_clips[0].length();
    container_.Push(core::gen_sub::kMismatchPosDelta, position);
    container_.Push(core::gen_sub::kMismatchType,
                    core::gen_const::kMismatchTypeDeletion);
    state.ref_offset++;
  }
}

// -----------------------------------------------------------------------------

void Encoder::EncodeHardClip(CodingState& state) {
  state.clips.hard_clips[state.is_right_clip] += state.count;
}

// -----------------------------------------------------------------------------

void Encoder::EncodeSoftClip(CodingState& state) {
  for (size_t i = 0; i < state.count; ++i) {
    if (state.read_pos >= state.read.length()) {
      UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
    }
    state.clips.soft_clips[state.is_right_clip] +=
        GetAlphabetProperties(core::AlphabetId::kAcgtn)
            .inverse_lut[state.read[state.read_pos]];
    state.read_pos++;
  }
}

// -----------------------------------------------------------------------------

void Encoder::EncodeSubstitution(CodingState& state) {
  if (state.ref[state.ref_offset] == 0) {
    const auto symbol = GetAlphabetProperties(core::AlphabetId::kAcgtn)
                            .inverse_lut[state.read[state.read_pos]];
    container_.Push(core::gen_sub::kUnalignedReads, symbol);
    return;
  }
  container_.Push(core::gen_sub::kMismatchPosTerminator,
                  core::gen_const::kMismatchPositionPersist);

  const auto position =
      state.read_pos - state.last_mismatch - state.clips.soft_clips[0].length();
  state.last_mismatch = state.read_pos + 1 - state.clips.soft_clips[0].length();
  container_.Push(core::gen_sub::kMismatchPosDelta, position);

  if (state.type >= core::record::ClassType::kClassM) {
    container_.Push(core::gen_sub::kMismatchType,
                    core::gen_const::kMismatchTypeSubstitution);
    const auto symbol = GetAlphabetProperties(core::AlphabetId::kAcgtn)
                            .inverse_lut[state.read[state.read_pos]];
    container_.Push(core::gen_sub::kMismatchTypeSubstBase, symbol);
    container_.PushDependency(core::gen_sub::kMismatchTypeSubstBase,
                              GetAlphabetProperties(core::AlphabetId::kAcgtn)
                                  .inverse_lut[state.ref[state.ref_offset]]);
  }
}

// -----------------------------------------------------------------------------

void Encoder::EncodeMatch(CodingState& state) {
  state.is_right_clip = true;
  for (size_t i = 0; i < state.count; ++i) {
    if (state.read_pos >= state.read.length()) {
      UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
    }
    if (state.read[state.read_pos] != state.ref[state.ref_offset]) {
      EncodeSubstitution(state);
    }

    state.read_pos++;
    state.ref_offset++;
  }
}

// -----------------------------------------------------------------------------

bool Encoder::UpdateCount(const char cigar_char, CodingState& state) {
  if (isdigit(cigar_char)) {
    state.count *= 10;
    state.count += cigar_char - '0';
    return true;
  }
  return false;
}

// -----------------------------------------------------------------------------

void Encoder::EncodeCigarToken(const char cigar_char, CodingState& state) {
  if (GetAlphabetProperties(core::AlphabetId::kAcgtn)
          .IsIncluded(cigar_char)) {  // TODO(Fabian): other alphabets
    if (constexpr char ambiguous_char = '-';
        (cigar_char == ambiguous_char && state.count == 0) ||
        cigar_char != ambiguous_char) {
      state.count = std::max(state.count, static_cast<size_t>(1));
      EncodeMatch(state);
      return;
    }
  }
  switch (cigar_char) {
    case '=':
      EncodeMatch(state);
      break;
    case '+':
      EncodeInsertion(state);
      break;
    case '-':
      EncodeDeletion(state);
      break;
    case ')':
      EncodeSoftClip(state);
      break;
    case ']':
      EncodeHardClip(state);
      break;
    case '%':
    case '/':
    case '*':
      EncodeSplice(state);
      break;
    case '(':
    case '[':
      break;  // Ignore opening braces, so that number can be parsed.
              // Closing braces will be handled
    default:
      UTILS_THROW_RUNTIME_EXCEPTION("Unknown CIGAR character");
  }
}

// -----------------------------------------------------------------------------

Encoder::ClipInformation Encoder::EncodeCigar(
    const std::string& read, const std::string& cigar, const std::string& ref,
    const core::record::ClassType type) {
  CodingState state(read, ref, type);
  for (const char cigar_char : cigar) {
    if (UpdateCount(cigar_char, state)) {
      continue;
    }
    EncodeCigarToken(cigar_char, state);
    state.count = 0;
  }

  if (state.read_pos != read.length()) {
    UTILS_THROW_RUNTIME_EXCEPTION("CIGAR and Read lengths do not match");
  }

  if (type > core::record::ClassType::kClassP &&
      type != core::record::ClassType::kClassHm) {
    container_.Push(core::gen_sub::kMismatchPosTerminator,
                    core::gen_const::kMismatchPosTerminate);
  }
  return state.clips;
}

// -----------------------------------------------------------------------------

core::AccessUnit&& Encoder::MoveStreams() { return std::move(container_); }

// -----------------------------------------------------------------------------

bool Encoder::EncodeSingleClip(const ClipInformation& inf, const bool last) {
  bool clips_present = false;
  for (const auto& hard : inf.hard_clips) {
    clips_present = hard || clips_present;
  }
  for (const auto& soft : inf.soft_clips) {
    clips_present = clips_present || !soft.empty();
  }
  if (!clips_present) {
    return clips_present;
  }

  for (size_t index = 0; index < inf.soft_clips.size(); ++index) {
    if (!inf.soft_clips[index].empty()) {
      const auto type = static_cast<uint32_t>(last) << 1u | index;
      container_.Push(core::gen_sub::kClipsType, type);
      for (const auto& c : inf.soft_clips[index]) {
        container_.Push(core::gen_sub::kClipsSoftClip, c);
      }
      const auto terminator = GetAlphabetProperties(core::AlphabetId::kAcgtn)
                                  .lut.size();  // TODO(Fabian): other alphabets
      container_.Push(core::gen_sub::kClipsSoftClip, terminator);
    } else if (inf.hard_clips[index]) {
      const auto type = 0x4u | static_cast<uint32_t>(last) << 1u | index;
      container_.Push(core::gen_sub::kClipsType, type);
      container_.Push(core::gen_sub::kClipsHardClip, inf.hard_clips[index]);
    }
  }

  return clips_present;
}

// -----------------------------------------------------------------------------

void Encoder::EncodeClips(
    const std::pair<ClipInformation, ClipInformation>& clips) {
  bool present = EncodeSingleClip(clips.first, false);
  present = EncodeSingleClip(clips.second, true) || present;

  if (present) {
    container_.Push(core::gen_sub::kClipsRecordId, read_counter_);
    container_.Push(core::gen_sub::kClipsType,
                    core::gen_const::kClipsRecordEnd);
  }

  read_counter_ +=
      1;  // TODO(Fabian): Check if this applies to unpaired situations
}

// -----------------------------------------------------------------------------

void Encoder::EncodeSplice(const CodingState& state) {
  (void)state;
  UTILS_DIE("Splicing is currently not supported");
}

// -----------------------------------------------------------------------------

uint32_t Encoder::GetReadLength() const {
  return length_.value_or(0);
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::basecoder

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
