/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.cc
 * @brief Implementation of the Decoder class for decoding base sequences and
 * metadata.
 *
 * This file contains the implementation of the Decoder class, which is
 * responsible for decoding base sequences from access units, extracting
 * metadata, and reconstructing genomic records. It handles various scenarios
 * involving multiple template segments and different alignment cases.
 *
 * @details The Decoder class interacts with other modules in the GENIE
 * framework, such as the core::AccessUnit, to decode quality values and manage
 * sequence alignments. The decoding process includes operations like alignment
 * restoration, segment handling, and sequence mismatch correction.
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#include "genie/read/basecoder/decoder.h"

#include <genie/core/record/alignment_split/other_rec.h>
#include <genie/core/record/alignment_split/unpaired.h>

#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "genie/core/qv_decoder.h"
#include "genie/core/record/alignment_split/same_rec.h"

// -----------------------------------------------------------------------------

namespace genie::read::basecoder {

// -----------------------------------------------------------------------------

Decoder::Decoder(core::AccessUnit&& au, const size_t segments, const size_t pos)
    : container_(std::move(au)),
      position_(pos),
      length_(container_.GetParameters().GetReadLength()),
      record_counter_(0),
      number_template_segments_(segments) {}

// -----------------------------------------------------------------------------

core::record::Record Decoder::Pull(uint16_t ref, std::vector<std::string>&& vec,
                                   const SegmentMeta& meta) {
  std::vector<std::string> sequences = std::move(vec);
  std::vector<std::string> cigars;
  cigars.reserve(sequences.size());
  for (auto& sequence : sequences) {
    cigars.emplace_back(sequence.size(), '=');
  }
  const auto clip_offset = ApplyClips(meta.clips, sequences, cigars);

  auto state = Decode(std::get<0>(clip_offset), std::move(sequences.front()),
                      std::move(cigars.front()));
  switch (meta.decoding_case) {
    case core::gen_const::kPairSameRecord:
      std::get<1>(state).SetRead1First(meta.first1);
      break;
    case core::gen_const::kPairR1Split:
      std::get<1>(state).SetRead1First(false);
      std::get<0>(state).AddAlignmentSplit(
          std::make_unique<core::record::alignment_split::OtherRec>(
              container_.Pull(core::gen_sub::kPairR1Split), ref));
      break;
    case core::gen_const::kPairR2Split:
      std::get<1>(state).SetRead1First(true);
      std::get<0>(state).AddAlignmentSplit(
          std::make_unique<core::record::alignment_split::OtherRec>(
              container_.Pull(core::gen_sub::kPairR2Split), ref));
      break;
    case core::gen_const::kPairR1DiffRef:
      std::get<1>(state).SetRead1First(false);
      std::get<0>(state).AddAlignmentSplit(
          std::make_unique<core::record::alignment_split::OtherRec>(
              container_.Pull(core::gen_sub::kPairR1DiffPos),
              static_cast<uint16_t>(
                  container_.Pull(core::gen_sub::kPairR1DiffSeq))));
      break;
    case core::gen_const::kPairR2DiffRef:
      std::get<1>(state).SetRead1First(true);
      std::get<0>(state).AddAlignmentSplit(
          std::make_unique<core::record::alignment_split::OtherRec>(
              container_.Pull(core::gen_sub::kPairR2DiffPos),
              static_cast<uint16_t>(
                  container_.Pull(core::gen_sub::kPairR2DiffSeq))));
      break;
    case core::gen_const::kPairR1Unpaired:
      std::get<1>(state).SetRead1First(true);
      std::get<0>(state).AddAlignmentSplit(
          std::make_unique<core::record::alignment_split::Unpaired>());
      break;
    case core::gen_const::kPairR2Unpaired:
      std::get<1>(state).SetRead1First(false);
      std::get<0>(state).AddAlignmentSplit(
          std::make_unique<core::record::alignment_split::Unpaired>());
      break;
    default:
      UTILS_DIE("Unknown decoding case for pair descriptor");
  }
  for (size_t i = 1; i < sequences.size(); ++i) {
    DecodeAdditional(
        std::get<1>(clip_offset), std::move(sequences[i]), std::move(cigars[i]),
        static_cast<uint16_t>(meta.position[1] - meta.position[0]), state);
  }

  std::get<1>(state).AddAlignment(ref, std::move(std::get<0>(state)));
  return std::get<1>(state);
}

// -----------------------------------------------------------------------------

Decoder::SegmentMeta Decoder::ReadSegmentMeta() {
  SegmentMeta meta{};
  meta.position[0] =
      position_ + container_.Get(core::gen_sub::kPositionFirst).Get(0);

  meta.num_segments = 1;
  if (number_template_segments_ == 2) {
    meta.decoding_case =
        container_.GetClassType() == core::record::ClassType::kClassHm
            ? core::gen_const::kPairSameRecord
            : container_.Pull(core::gen_sub::kPairDecodingCase);
    if (meta.decoding_case == core::gen_const::kPairSameRecord) {
      meta.num_segments = 2;
      const auto same_rec_data =
          static_cast<uint32_t>(container_.Pull(core::gen_sub::kPairSameRec));
      meta.first1 = !(same_rec_data & 1u);
      const auto delta =
          static_cast<int16_t>(static_cast<uint16_t>(same_rec_data >> 1u));
      meta.position[1] = meta.position[0] + delta;
    }
  }

  meta.clips = DecodeClips();

  const auto deletions = NumberDeletions(meta.num_segments);
  for (size_t i = 0; i < meta.num_segments; ++i) {
    if (length_ > 0) {
      meta.length[i] = length_ - meta.clips.hard_clips[i][0] -
                       meta.clips.hard_clips[i][1] + deletions[i];
    } else {
      meta.length[i] =
          container_.Pull(core::gen_sub::kReadLength) + 1 + deletions[i];
    }
  }
  return meta;
}

// -----------------------------------------------------------------------------

std::tuple<core::record::AlignmentBox, core::record::Record> Decoder::Decode(
    size_t clip_offset, std::string&& seq, std::string&& cigar) {
  auto sequence = std::move(seq);

  uint8_t rtype = 0;
  if (container_.IsEnd(core::gen_sub::kRtype)) {
    rtype = static_cast<uint8_t> (this->container_.GetClassType());
  } else {
    rtype = container_.Pull(core::gen_sub::kRtype);
    if (rtype == 5) {
      rtype = 6;
    } else if (rtype == 6) {
      rtype = 5;
    }
  }

  const auto reverse_comp =
      static_cast<uint8_t>(container_.Pull(core::gen_sub::kReverseComplement));

  const auto flag_pcr = container_.IsEnd(core::gen_sub::kFlagsPcrDuplicate)
                            ? 0
                            : container_.Pull(core::gen_sub::kFlagsPcrDuplicate)
                                  << core::gen_const::kFlagsPcrDuplicatePos;
  const auto flag_quality =
      container_.IsEnd(core::gen_sub::kFlagsQualityFail)
          ? 0
          : container_.Pull(core::gen_sub::kFlagsQualityFail)
                << core::gen_const::kFlagsQualityFailPos;
  const auto flag_pair = container_.IsEnd(core::gen_sub::kFlagsProperPair)
                             ? 0
                             : container_.Pull(core::gen_sub::kFlagsProperPair)
                                   << core::gen_const::kFlagsProperPairPos;
  const auto flags = static_cast<uint8_t>(flag_pcr | flag_quality | flag_pair);

  const auto mapping_score =
      static_cast<uint32_t>(container_.Pull(core::gen_sub::kMappingScore));

  position_ += container_.Pull(core::gen_sub::kPositionFirst);
  const auto position = position_;

  std::string e_cigar = std::move(cigar);
  DecodeMismatches(clip_offset, sequence, e_cigar);

  core::record::Alignment alignment(ContractECigar(e_cigar), reverse_comp);
  alignment.AddMappingScore(static_cast<int32_t>(mapping_score));

  std::tuple<core::record::AlignmentBox, core::record::Record> ret;
  std::get<0>(ret) = core::record::AlignmentBox(position, std::move(alignment));

  std::get<1>(ret) = core::record::Record(
      static_cast<uint8_t>(number_template_segments_),
      static_cast<core::record::ClassType>(rtype), "", "", flags);

  core::record::Segment segment(std::move(sequence));
  std::get<1>(ret).AddSegment(std::move(segment));
  return ret;
}

// -----------------------------------------------------------------------------

std::string Decoder::ContractECigar(const std::string& cigar_long) {
  char cur_tok = cigar_long.front();
  size_t count = 0;
  std::string result;
  for (const auto& c : cigar_long) {
    if (c != cur_tok) {
      if (cur_tok == ']') {
        result += '[';
      }
      if (cur_tok == ')') {
        result += '(';
      }
      if (!GetAlphabetProperties(core::AlphabetId::kAcgtn)
               .IsIncluded(cur_tok)) {
        result += std::to_string(count);
        result += cur_tok;
      } else {
        for (size_t i = 0; i < count; ++i) {
          result += cur_tok;
        }
      }
      cur_tok = c;
      count = 0;
    }
    count++;
  }
  if (cur_tok == ']') {
    result += '[';
  }
  if (cur_tok == ')') {
    result += '(';
  }
  if (!GetAlphabetProperties(core::AlphabetId::kAcgtn).IsIncluded(cur_tok)) {
    result += std::to_string(count);
    result += cur_tok;
  } else {
    for (size_t i = 0; i < count; ++i) {
      result += cur_tok;
    }
  }
  return result;
}

// -----------------------------------------------------------------------------

void Decoder::DecodeAdditional(
    const size_t softclip_offset, std::string&& seq, std::string&& cigar,
    uint16_t delta_pos,
    std::tuple<core::record::AlignmentBox, core::record::Record>& state) {
  auto sequence = std::move(seq);

  if (std::get<1>(state).GetClassId() != core::record::ClassType::kClassHm) {
    std::string e_cigar = std::move(cigar);
    DecodeMismatches(softclip_offset, sequence, e_cigar);
    const auto reverse_comp = static_cast<uint8_t>(
        container_.Pull(core::gen_sub::kReverseComplement));
    const auto mapping_score =
        static_cast<int32_t>(container_.Pull(core::gen_sub::kMappingScore));

    core::record::Alignment alignment(ContractECigar(e_cigar), reverse_comp);
    alignment.AddMappingScore(mapping_score);
    std::get<0>(state).AddAlignmentSplit(
        std::make_unique<core::record::alignment_split::SameRec>(delta_pos,
                                                                 alignment));
  } else {
    for (auto& c : sequence) {
      if (c == 0) {
        c = GetAlphabetProperties(core::AlphabetId::kAcgtn)
                .lut[container_.Pull(core::gen_sub::kUnalignedReads)];
      }
    }
  }
  core::record::Segment segment(std::move(sequence));
  std::get<1>(state).AddSegment(std::move(segment));
}

// -----------------------------------------------------------------------------

std::vector<int32_t> Decoder::NumberDeletions(const size_t number) {
  std::vector counters(number, 0);
  if (container_.Get(core::gen_sub::kMismatchPosTerminator).IsEmpty() ||
      container_.Get(core::gen_sub::kMismatchType).IsEmpty()) {
    return counters;
  }
  size_t lookahead = 0;
  for (size_t r = 0; r < number; ++r) {
    while (!container_.Peek(core::gen_sub::kMismatchPosTerminator,
                            lookahead + r)) {
      if (container_.Peek(core::gen_sub::kMismatchType, lookahead) ==
          core::gen_const::kMismatchTypeDeletion) {
        counters[r]++;
      } else if (container_.Peek(core::gen_sub::kMismatchType, lookahead) ==
                 core::gen_const::kMismatchTypeInsertion) {
        counters[r]--;
      }
      lookahead++;
    }
  }
  return counters;
}

// -----------------------------------------------------------------------------

void Decoder::DecodeMismatches(const size_t clip_offset, std::string& sequence,
                               std::string& cigar_extended) {
  uint64_t mismatch_position = 0;
  const auto start_pos = cigar_extended.find_first_not_of(']');
  uint64_t cigar_offset = start_pos == std::string::npos ? 0 : start_pos;
  if (container_.IsEnd(core::gen_sub::kMismatchPosTerminator)) {
    for (auto& c : sequence) {
      if (c == 0) {
        c = GetAlphabetProperties(core::AlphabetId::kAcgtn)
                .lut[container_.Pull(core::gen_sub::kUnalignedReads)];
      }
    }
    return;
  }
  while (!container_.Pull(core::gen_sub::kMismatchPosTerminator)) {
    mismatch_position += container_.Pull(core::gen_sub::kMismatchPosDelta) + 1;
    const auto position = mismatch_position - 1 + clip_offset;
    const auto type = container_.Get(core::gen_sub::kMismatchType).IsEmpty()
                          ? core::gen_const::kMismatchTypeSubstitution
                          : container_.Pull(core::gen_sub::kMismatchType);
    if (type == core::gen_const::kMismatchTypeSubstitution) {
      const auto substitution =
          container_.Get(core::gen_sub::kMismatchTypeSubstBase)
                      .GetMismatchDecoder() &&
                  container_.Get(core::gen_sub::kMismatchTypeSubstBase)
                      .GetMismatchDecoder()
                      ->DataLeft()
              ? container_.Get(core::gen_sub::kMismatchTypeSubstBase)
                    .GetMismatchDecoder()
                    ->DecodeMismatch(
                        GetAlphabetProperties(core::AlphabetId::kAcgtn)
                            .inverse_lut[sequence[position]])
          : container_.Get(core::gen_sub::kMismatchTypeSubstBase).end()
              ? GetAlphabetProperties(core::AlphabetId::kAcgtn).inverse_lut['N']
              : container_.Get(core::gen_sub::kMismatchTypeSubstBase).Pull();
      const auto substitution_char =
          GetAlphabetProperties(core::AlphabetId::kAcgtn).lut[substitution];
      sequence[position] = substitution_char;
      cigar_extended[position + cigar_offset] = substitution_char;
    } else if (type == core::gen_const::kMismatchTypeInsertion) {
      const auto insertion =
          container_.Pull(core::gen_sub::kMismatchTypeInsert);
      const auto insertion_char =
          GetAlphabetProperties(core::AlphabetId::kAcgtn).lut[insertion];
      sequence.insert(position, 1, insertion_char);
      cigar_extended.insert(position + cigar_offset, 1, '+');
    } else {
      sequence.erase(position, 1);
      cigar_extended[position + cigar_offset] = '-';
      cigar_offset += 1;
      mismatch_position -= 1;
    }
  }

  for (auto& c : sequence) {
    if (c == 0) {
      c = GetAlphabetProperties(core::AlphabetId::kAcgtn)
              .lut[container_.Pull(core::gen_sub::kUnalignedReads)];
    }
  }
}

// -----------------------------------------------------------------------------

Decoder::Clips Decoder::DecodeClips() {
  Clips ret;
  const size_t num = record_counter_++;
  std::tuple<size_t, size_t> softclip_offset{0, 0};
  if (container_.IsEnd(core::gen_sub::kClipsRecordId) ||
      num != container_.Peek(core::gen_sub::kClipsRecordId)) {
    return ret;
  }
  container_.Pull(core::gen_sub::kClipsRecordId);
  auto clip_type = container_.Pull(core::gen_sub::kClipsType);
  while (clip_type != core::gen_const::kClipsRecordEnd) {
    const bool hardclip = clip_type & 4u;
    const size_t record_no = clip_type & 2u ? 1 : 0;
    const bool end = clip_type & 1u;

    if (hardclip) {
      const auto hardclip_size = container_.Pull(core::gen_sub::kClipsHardClip);
      ret.hard_clips[record_no][end] = hardclip_size;
    } else {
      std::string soft_clip;
      auto symbol = container_.Pull(core::gen_sub::kClipsSoftClip);
      while (symbol !=
             GetAlphabetProperties(core::AlphabetId::kAcgtn).lut.size()) {
        soft_clip +=
            GetAlphabetProperties(core::AlphabetId::kAcgtn).lut[symbol];
        symbol = container_.Pull(core::gen_sub::kClipsSoftClip);
      }

      ret.soft_clips[record_no][end] = soft_clip;
    }
    clip_type = container_.Pull(core::gen_sub::kClipsType);
  }

  return ret;
}

// -----------------------------------------------------------------------------

std::tuple<size_t, size_t> Decoder::ApplyClips(
    const Clips& clips, std::vector<std::string>& sequences,
    std::vector<std::string>& cigar_extended) {
  std::tuple<size_t, size_t> softclip_offset{0, 0};

  for (size_t segment_no = 0; segment_no < 2; ++segment_no) {
    for (size_t end = 0; end < 2; ++end) {
      if (clips.hard_clips[segment_no][end] != 0) {
        const size_t cigar_position =
            end ? cigar_extended[segment_no].size() : 0;
        cigar_extended[segment_no].insert(
            cigar_position, clips.hard_clips[segment_no][end], ']');
      }
      if (!clips.soft_clips[segment_no][end].empty()) {
        std::string soft_clip = clips.soft_clips[segment_no][end];

        sequences[segment_no].erase(sequences[segment_no].length() -
                                    soft_clip.length());
        size_t cigar_position =
            end ? cigar_extended[segment_no].find_last_not_of(']') + 1 -
                      soft_clip.size()
                : cigar_extended[segment_no].find_first_not_of(']');
        const size_t seq_position = end ? sequences[segment_no].length() : 0;
        sequences[segment_no].insert(seq_position, soft_clip);
        for (size_t i = 0; i < soft_clip.size(); ++i) {
          cigar_extended[segment_no][cigar_position++] = ')';
        }

        if (seq_position == 0) {
          if (segment_no == 0) {
            std::get<0>(softclip_offset) =
                soft_clip.length();  // TODO(Fabian): not working for paired
          } else {
            std::get<1>(softclip_offset) = soft_clip.length();
          }
        }
      }
    }
  }

  return softclip_offset;
}

// -----------------------------------------------------------------------------

void Decoder::Clear() { this->container_.Clear(); }

// -----------------------------------------------------------------------------

}  // namespace genie::read::basecoder

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
