/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/core/constants.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/record/class_type.h"

// -----------------------------------------------------------------------------

namespace genie::core {

// -----------------------------------------------------------------------------
const std::string& GetMpegVersionString(MpegMinorVersion v) {
  static const std::string lut[] = {"1900", "2000"};
  return lut[static_cast<uint8_t>(v)];
}

// -----------------------------------------------------------------------------
MpegMinorVersion GetMpegVersion(const std::string& v) {
  for (auto ret = MpegMinorVersion::kV1900; ret < MpegMinorVersion::kUnknown;
       ret = static_cast<MpegMinorVersion>(static_cast<uint8_t>(ret) + 1)) {
    if (GetMpegVersionString(ret) == v) {
      return ret;
    }
  }
  return MpegMinorVersion::kUnknown;
}

// -----------------------------------------------------------------------------
const std::vector<GenomicDescriptorProperties>& GetDescriptors() {
  static const std::vector<GenomicDescriptorProperties> prop = {
      {GenDesc::kPosition,
       "pos",
       false,
       {{gen_sub::kPositionFirst,
         "first",
         false,
         {std::numeric_limits<uint32_t>::min(),
          std::numeric_limits<uint32_t>::max()}},
        {gen_sub::kPositionAdditional,
         "additional",
         false,
         {std::numeric_limits<uint32_t>::min(),
          std::numeric_limits<uint32_t>::max()}}}},

      {GenDesc::kReverseComplement,
       "rcomp",  // NOLINT
       false,
       {{gen_sub::kReverseComplement,
         "rcomp",  // NOLINT
         false,
         {std::numeric_limits<bool>::min(),
          std::numeric_limits<bool>::max()}}}},

      {GenDesc::kFlags,
       "flags",
       false,
       {{gen_sub::kFlagsPcrDuplicate,
         "pcr_duplicate",
         false,
         {std::numeric_limits<bool>::min(), std::numeric_limits<bool>::max()}},
        {gen_sub::kFlagsQualityFail,
         "quality_flag",
         false,
         {std::numeric_limits<bool>::min(), std::numeric_limits<bool>::max()}},
        {gen_sub::kFlagsProperPair,
         "proper_pair",
         false,
         {std::numeric_limits<bool>::min(),
          std::numeric_limits<bool>::max()}}}},

      {GenDesc::kMismatchPosition,
       "mmpos",  // NOLINT
       false,
       {{gen_sub::kMismatchPosTerminator,
         "terminator",
         false,
         {std::numeric_limits<bool>::min(), std::numeric_limits<bool>::max()}},
        {gen_sub::kMismatchPosDelta,
         "position",
         false,
         {std::numeric_limits<uint32_t>::min(),
          std::numeric_limits<uint32_t>::max()}}}},

      {GenDesc::kMismatchType,
       "mmtype",  // NOLINT
       false,
       {{gen_sub::kMismatchType,
         "type",
         false,
         {std::numeric_limits<uint8_t>::min(),
          gen_const::kMismatchTypeDeletion}},
        {gen_sub::kMismatchTypeSubstBase,
         "substitution",
         true,
         {0, GetAlphabetProperties(AlphabetId::kAcgtn).lut.size() - 1}},
        {gen_sub::kMismatchTypeInsert,
         "insertion",
         false,
         {0, GetAlphabetProperties(AlphabetId::kAcgtn).lut.size() - 1}}}},

      {GenDesc::kClips,
       "clips",
       false,
       {{gen_sub::kClipsRecordId,
         "record_id",
         false,
         {std::numeric_limits<uint32_t>::min(),
          std::numeric_limits<uint32_t>::max()}},
        {gen_sub::kClipsType,
         "type",
         false,
         {std::numeric_limits<uint8_t>::min(), gen_const::kClipsRecordEnd}},
        {gen_sub::kClipsSoftClip,
         "soft_string",
         false,
         {std::numeric_limits<uint8_t>::min(),
          GetAlphabetProperties(AlphabetId::kAcgtn).lut.size()}},
        {gen_sub::kClipsHardClip,
         "hard_length",
         false,
         {std::numeric_limits<uint32_t>::min(),
          std::numeric_limits<uint32_t>::max()}}}},

      {GenDesc::kUnalignedReads,
       "ureads",  // NOLINT
       false,
       {{gen_sub::kUnalignedReads,
         "ureads",  // NOLINT
         false,
         {std::numeric_limits<uint8_t>::min(),
          GetAlphabetProperties(AlphabetId::kAcgtn).lut.size() - 1}}}},

      {GenDesc::kReadLength,
       "rlen",  // NOLINT
       false,
       {{gen_sub::kReadLength,
         "rlen",  // NOLINT
         false,
         {std::numeric_limits<uint32_t>::min(),
          std::numeric_limits<uint32_t>::max()}}}},

      {GenDesc::kPair,
       "pair",
       false,
       {{gen_sub::kPairDecodingCase,
         "decoding_case",
         false,
         {std::numeric_limits<uint8_t>::min(), gen_const::kPairR2Unpaired}},
        {gen_sub::kPairSameRec,
         "same_rec",
         false,
         {std::numeric_limits<uint16_t>::min(),
          std::numeric_limits<uint16_t>::max()}},
        {gen_sub::kPairR1Split,
         "r1_split",
         false,
         {std::numeric_limits<uint32_t>::min(),
          std::numeric_limits<uint32_t>::max()}},
        {gen_sub::kPairR2Split,
         "r2_split",
         false,
         {std::numeric_limits<uint32_t>::min(),
          std::numeric_limits<uint32_t>::max()}},
        {gen_sub::kPairR1DiffSeq,
         "r1_diff_seq",
         false,
         {std::numeric_limits<uint16_t>::min(),
          std::numeric_limits<uint16_t>::max()}},
        {gen_sub::kPairR2DiffSeq,
         "r2_diff_seq",
         false,
         {std::numeric_limits<uint16_t>::min(),
          std::numeric_limits<uint16_t>::max()}},
        {gen_sub::kPairR1DiffPos,
         "r1_diff_pos",
         false,
         {std::numeric_limits<uint32_t>::min(),
          std::numeric_limits<uint32_t>::max()}},
        {gen_sub::kPairR2DiffPos,
         "r2_diff_pos",
         false,
         {std::numeric_limits<uint32_t>::min(),
          std::numeric_limits<uint32_t>::max()}}}},

      {GenDesc::kMappingScore,
       "mscore",  // NOLINT
       false,
       {{gen_sub::kMappingScore,
         "mscore",  // NOLINT
         false,
         {std::numeric_limits<uint8_t>::min(),
          std::numeric_limits<uint8_t>::max()}}}},

      {GenDesc::kMultiMap,
       "mmap",
       false,
       {{gen_sub::kMmapNumberAlign,
         "number_alignments",
         false,
         {std::numeric_limits<uint8_t>::min(),
          std::numeric_limits<uint8_t>::max()}},
        {gen_sub::kMmapRightAlignId,
         "right_alignment_id",
         false,
         {std::numeric_limits<uint8_t>::min(),
          std::numeric_limits<uint8_t>::max()}},
        {gen_sub::kMmapOtherRecFlag,
         "other_rec_flag",
         false,
         {std::numeric_limits<bool>::min(), std::numeric_limits<bool>::max()}},
        {gen_sub::kMmapRefSeq,
         "reference_seq",
         false,
         {std::numeric_limits<uint16_t>::min(),
          std::numeric_limits<uint16_t>::max()}},
        {gen_sub::kMmapRefPos,
         "reference_pos",
         false,
         {std::numeric_limits<uint32_t>::min(),
          std::numeric_limits<uint32_t>::max()}}}},

      {GenDesc::kMultiSegmentAlignment,
       "msar",  // NOLINT
       true,
       {{gen_sub::kMultiSegmentAlignmentCabac0,
         "cabac_0",
         false,
         {std::numeric_limits<uint32_t>::min(),
          std::numeric_limits<uint32_t>::max()}},
        {gen_sub::kMultiSegmentAlignmentCabac1,
         "cabac_1",
         false,
         {std::numeric_limits<uint32_t>::min(),
          std::numeric_limits<uint32_t>::max()}}}},

      {GenDesc::kRtype,
       "rtype",
       false,
       {{gen_sub::kRtype,
         "rtype",
         false,
         {std::numeric_limits<uint8_t>::min(),
          static_cast<uint8_t>(record::ClassType::kCount)}}}},

      {GenDesc::kReadGroup,
       "rgroup",  // NOLINT
       false,
       {{gen_sub::kReadGroup,
         "rgroup",  // NOLINT
         false,
         {std::numeric_limits<uint8_t>::min(),
          std::numeric_limits<uint8_t>::max()}}}},

      {GenDesc::kQv,
       "qv",
       false,
       {
           {gen_sub::kQvPresent,
            "present",
            false,
            {std::numeric_limits<bool>::min(),
             std::numeric_limits<bool>::max()}},
           {gen_sub::kQvCodebook,
            "codebook",
            false,
            {std::numeric_limits<uint8_t>::min(), 4}},
           {gen_sub::kQvSteps0,
            "steps_0",
            false,
            {std::numeric_limits<uint8_t>::min(),
             std::numeric_limits<uint8_t>::max()}},
           {gen_sub::kQvSteps1,
            "steps_1",
            false,
            {std::numeric_limits<uint8_t>::min(),
             std::numeric_limits<uint8_t>::max()}},
           {gen_sub::kQvSteps2,
            "steps_2",
            false,
            {std::numeric_limits<uint8_t>::min(),
             std::numeric_limits<uint8_t>::max()}},
           {gen_sub::kQvSteps3,
            "steps_3",
            false,
            {std::numeric_limits<uint8_t>::min(),
             std::numeric_limits<uint8_t>::max()}},
           {gen_sub::kQvSteps4,
            "steps_4",
            false,
            {std::numeric_limits<uint8_t>::min(),
             std::numeric_limits<uint8_t>::max()}},
           {gen_sub::kQvSteps5,
            "steps_5",
            false,
            {std::numeric_limits<uint8_t>::min(),
             std::numeric_limits<uint8_t>::max()}},
           {gen_sub::kQvSteps6,
            "steps_6",
            false,
            {std::numeric_limits<uint8_t>::min(),
             std::numeric_limits<uint8_t>::max()}},
           {gen_sub::kQvSteps7,
            "steps_7",
            false,
            {std::numeric_limits<uint8_t>::min(),
             std::numeric_limits<uint8_t>::max()}},
       }},

      {GenDesc::kReadName,
       "rname",  // NOLINT
       true,
       {{gen_sub::kReadNameCabac0,
         "cabac0",
         false,
         {std::numeric_limits<uint32_t>::min(),
          std::numeric_limits<uint32_t>::max()}},
        {gen_sub::kReadNameCabac1,
         "cabac1",
         false,
         {std::numeric_limits<uint32_t>::min(),
          std::numeric_limits<uint32_t>::max()}}}},

      {GenDesc::kRefTransPosition,
       "rftp",  // NOLINT
       false,
       {{gen_sub::kRefTransPosition,
         "rftp",  // NOLINT
         false,
         {std::numeric_limits<uint32_t>::min(),
          std::numeric_limits<uint32_t>::max()}}}},

      {GenDesc::kRefTransTransform,
       "rftt",  // NOLINT
       false,
       {{gen_sub::kRefTransPosition,
         "rftt",  // NOLINT
         true,
         {std::numeric_limits<uint8_t>::min(),
          GetAlphabetProperties(AlphabetId::kAcgtn).lut.size() - 1}}}},
  };

  return prop;
}

// -----------------------------------------------------------------------------
const GenomicDescriptorProperties& GetDescriptor(GenDesc desc) {
  return GetDescriptors()[static_cast<uint8_t>(desc)];
}

// -----------------------------------------------------------------------------
const GenomicSubDescriptorProperties& GetSubsequence(GenSubIndex idx) {
  if (GetDescriptors()[static_cast<uint8_t>(idx.first)].token_type) {
    return GetDescriptors()[static_cast<uint8_t>(idx.first)].sub_seqs[0];
  }
  return GetDescriptors()[static_cast<uint8_t>(idx.first)]
      .sub_seqs[static_cast<uint8_t>(idx.second)];
}

// -----------------------------------------------------------------------------
const Alphabet& GetAlphabetProperties(AlphabetId id) {
  static const auto prop = []() -> std::vector<Alphabet> {
    std::vector<Alphabet> loc;
    loc.emplace_back();
    loc.back().lut = {'A', 'C', 'G', 'T', 'N'};
    loc.back().base_bits = 3;
    loc.emplace_back();
    loc.back().lut = {'A', 'C', 'G', 'T', 'R', 'Y', 'S', 'W',
                      'K', 'M', 'B', 'D', 'H', 'V', 'N', '-'};
    loc.back().base_bits = 5;
    for (auto& l : loc) {
      l.inverse_lut = std::vector<char>(
          *std::max_element(l.lut.begin(), l.lut.end()) + 1, 0);
      for (size_t i = 0; i < l.lut.size(); ++i) {
        l.inverse_lut[l.lut[i]] = static_cast<char>(i);
      }
    }
    return loc;
  }();
  return prop[static_cast<uint8_t>(id)];
}

// -----------------------------------------------------------------------------
const GenomicSubDescriptorProperties& GenomicDescriptorProperties::GetSubSeq(
    const uint8_t sub) const {
  return sub_seqs[sub];
}

// -----------------------------------------------------------------------------
bool Alphabet::IsIncluded(char c) const {
  return std::any_of(lut.begin(), lut.end(),
                     [c](const char lc) { return lc == c; });
}

// -----------------------------------------------------------------------------
const CigarFormatInfo& GetECigarInfo() {
  static const auto format_info = []() -> CigarFormatInfo {
    const auto ref_step2 = []() -> std::vector<uint8_t> {
      std::vector<uint8_t> lut(128, 0);
      lut['='] = 1;
      lut['+'] = 0;
      lut['-'] = 1;
      lut['*'] = 1;
      lut['/'] = 1;
      lut['%'] = 1;
      lut[')'] = 0;
      lut[']'] = 0;
      return lut;
    }();
    const auto seq_step2 = []() -> std::vector<uint8_t> {
      std::vector<uint8_t> lut(128, 0);
      lut['='] = 1;
      lut['+'] = 1;
      lut['-'] = 0;
      lut['%'] = 0;
      lut['/'] = 0;
      lut['*'] = 0;
      lut[')'] = 1;
      lut[']'] = 0;
      return lut;
    }();
    const auto ignore2 = []() -> std::vector<uint8_t> {
      std::vector<uint8_t> lut(128, 0);
      lut['('] = 1;
      lut['['] = 1;
      return lut;
    }();
    return {ref_step2, seq_step2, ignore2, true};
  }();
  return format_info;
}

// -----------------------------------------------------------------------------
uint8_t Bits2Bytes(const uint8_t bits) {
  auto bytes = static_cast<uint8_t>(std::ceil(static_cast<float>(bits) / 8.0f));
  bytes = static_cast<uint8_t>(std::pow(2, std::ceil(log2(bytes))));
  return bytes;
}

// -----------------------------------------------------------------------------
uint8_t Range2Bytes(const std::pair<int64_t, int64_t>& range) {
  auto bits =
      static_cast<uint8_t>(std::ceil(std::log2(std::abs(range.first) + 1)));
  bits = std::max(bits, static_cast<uint8_t>(
                            std::ceil(std::log2(std::abs(range.second) + 1))));
  if (range.first < 0) {
    bits++;
  }
  return Bits2Bytes(bits);
}

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
