/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

// -----------------------------------------------------------------------------

#include "genie/format/sam/sam_to_mgrec/sam_record.h"

#include <algorithm>
#include <string>
#include <utility>

#include "genie/core/record/alignment_external/other_rec.h"
#include "genie/core/record/alignment_split/other_rec.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam::sam_to_mgrec {

// -----------------------------------------------------------------------------
char SamRecord::FourBitBase2Char(const uint8_t int_base) {
  switch (int_base) {
    case 1:
      return 'A';
    case 2:
      return 'C';
    case 4:
      return 'G';
    case 8:
      return 'T';
    case 15:
      return 'N';
    default:
      // Raise error here
      return -1;
  }
}

// -----------------------------------------------------------------------------
std::string SamRecord::GetCigarString(const bam1_t* sam_alignment) {
  const auto n_cigar = sam_alignment->core.n_cigar;
  const auto cigar_ptr = bam_get_cigar(sam_alignment);

  std::string cigar;
  for (uint32_t i = 0; i < n_cigar; i++) {
    const char cigar_op = bam_cigar_opchr(bam_cigar_op(cigar_ptr[i]));
    const auto cigar_op_len = bam_cigar_oplen(cigar_ptr[i]);
    cigar += std::to_string(cigar_op_len) + cigar_op;
  }
  return cigar;
}

// -----------------------------------------------------------------------------
std::string SamRecord::GetSeqString(const bam1_t* sam_alignment) {
  const auto seq_len = sam_alignment->core.l_qseq;
  const auto seq_ptr = bam_get_seq(sam_alignment);
  std::string tmp_seq(seq_len, ' ');
  for (auto i = 0; i < seq_len; i++) {
    tmp_seq[i] = FourBitBase2Char(bam_seqi(seq_ptr, i));
  }

  return tmp_seq;
}

// -----------------------------------------------------------------------------
std::string SamRecord::GetQualString(const bam1_t* sam_alignment) {
  const auto seq_len = sam_alignment->core.l_qseq;
  const auto qual_ptr = bam_get_qual(sam_alignment);
  std::string tmp_qual(seq_len, ' ');
  for (auto i = 0; i < seq_len; i++) {
    tmp_qual[i] = static_cast<char>(qual_ptr[i] + 33);
  }
  return tmp_qual;
}

// -----------------------------------------------------------------------------
char SamRecord::ConvertCigar2ECigarChar(const char token) {
  static const auto lut_loc = []() -> std::string {  // NOLINT
    std::string lut(128, 0);
    lut['M'] = '=';
    lut['='] = '=';
    lut['X'] = '=';
    lut['I'] = '+';
    lut['D'] = '-';
    lut['N'] = '*';
    lut['S'] = ')';
    lut['H'] = ']';
    lut['P'] = ']';
    return lut;
  }();
  UTILS_DIE_IF(token < 0, "Invalid cigar token " + std::to_string(token));
  const char ret = lut_loc[token];
  UTILS_DIE_IF(ret == 0, "Invalid cigar token" + std::to_string(token));
  return ret;
}

// -----------------------------------------------------------------------------
int SamRecord::StepSequence(const char token) {
  static const auto lut_loc = []() -> std::string {  // NOLINT
    std::string lut(128, 0);
    lut['M'] = 1;
    lut['='] = 1;
    lut['X'] = 1;
    lut['I'] = 1;
    lut['D'] = 0;
    lut['N'] = 0;
    lut['S'] = 1;
    lut['H'] = 0;
    lut['P'] = 0;
    return lut;
  }();
  UTILS_DIE_IF(token < 0, "Invalid cigar token " + std::to_string(token));
  return lut_loc[token];
}

// -----------------------------------------------------------------------------
std::string SamRecord::ConvertCigar2ECigar(const std::string& cigar,
                                           const std::string& seq) {
  std::string ecigar;
  constexpr size_t expected_elongation =
      4;  // Additional braces for softclips + hardclips
  ecigar.reserve(cigar.length() + expected_elongation);
  size_t seq_pos = 0;
  std::string digits;
  for (const auto& a : cigar) {
    if (std::isdigit(a)) {
      digits += a;
      continue;
    }
    if (a == 'X') {
      const size_t end = std::stoi(digits) + seq_pos;
      UTILS_DIE_IF(end >= seq.length(), "CIGAR not valid for seq");
      for (; seq_pos < end; ++seq_pos) {
        ecigar += static_cast<char>(std::toupper(seq[seq_pos]));
      }
    } else {
      if (a == 'S') {
        ecigar += '(';
      } else if (a == 'H') {
        ecigar += '[';
      }
      const char token = ConvertCigar2ECigarChar(a);
      seq_pos += StepSequence(a) * std::stoi(digits);
      ecigar += digits;
      ecigar += token;
    }
    digits.clear();
  }
  return ecigar;
}

// -----------------------------------------------------------------------------
SamRecord::SamRecord()
    : flag_(0), rid_(0), pos_(0), mapq_(0), mate_rid_(0), mate_pos_(0) {}

// -----------------------------------------------------------------------------
SamRecord::SamRecord(const bam1_t* sam_alignment)
    : qname_(bam_get_qname(sam_alignment)),
      flag_(sam_alignment->core.flag),
      rid_(sam_alignment->core.tid),
      pos_(static_cast<uint32_t>(sam_alignment->core.pos)),
      mapq_(sam_alignment->core.qual),
      cigar_(GetCigarString(sam_alignment)),
      mate_rid_(sam_alignment->core.mtid),
      mate_pos_(static_cast<uint32_t>(sam_alignment->core.mpos)),
      //      tlen(sam_alignment->core.isize),
      seq_(GetSeqString(sam_alignment)),   // Initialized with empty char due to
                                           // conversion later
      qual_(GetQualString(sam_alignment))  // Initialized with empty char due to
                                           // conversion later
{}

// -----------------------------------------------------------------------------
const std::string& SamRecord::GetQname() const { return qname_; }

// -----------------------------------------------------------------------------
std::string&& SamRecord::MoveQname() { return std::move(qname_); }

// -----------------------------------------------------------------------------
uint16_t SamRecord::GetFlag() const { return flag_; }

// -----------------------------------------------------------------------------
int32_t SamRecord::GetRid() const { return rid_; }

// -----------------------------------------------------------------------------
uint32_t SamRecord::GetPos() const { return pos_; }

// -----------------------------------------------------------------------------
uint8_t SamRecord::GetMapq() const { return mapq_; }

// -----------------------------------------------------------------------------
const std::string& SamRecord::GetCigar() const { return cigar_; }

// -----------------------------------------------------------------------------
std::string&& SamRecord::MoveCigar() { return std::move(cigar_); }

// -----------------------------------------------------------------------------
std::string SamRecord::GetECigar() const {
  return ConvertCigar2ECigar(cigar_, seq_);
}

// -----------------------------------------------------------------------------
int32_t SamRecord::GetMrid() const { return mate_rid_; }

// -----------------------------------------------------------------------------
uint32_t SamRecord::GetMPos() const { return mate_pos_; }

// -----------------------------------------------------------------------------
const std::string& SamRecord::GetSeq() const { return seq_; }

// -----------------------------------------------------------------------------
std::string&& SamRecord::MoveSeq() { return std::move(seq_); }

// -----------------------------------------------------------------------------
void SamRecord::SetSeq(std::string&& seq) { seq_ = seq; }

// -----------------------------------------------------------------------------
void SamRecord::SetSeq(const std::string& seq) { seq_ = seq; }

// -----------------------------------------------------------------------------
const std::string& SamRecord::GetQual() const { return qual_; }

// -----------------------------------------------------------------------------
std::string&& SamRecord::MoveQual() { return std::move(qual_); }

// -----------------------------------------------------------------------------
void SamRecord::SetQual(const std::string& qual) { qual_ = qual; }

// -----------------------------------------------------------------------------
bool SamRecord::CheckFlag(const uint16_t flag) const {
  return (flag_ & flag) == flag;
}  // All must be set

// -----------------------------------------------------------------------------
bool SamRecord::CheckNFlag(const uint16_t flag) const {
  return (flag_ & flag) == 0;
}  // All must be unset

// -----------------------------------------------------------------------------
bool SamRecord::IsUnmapped() const { return CheckFlag(BAM_FUNMAP); }

// -----------------------------------------------------------------------------
bool SamRecord::IsMateUnmapped() const { return CheckFlag(BAM_FMUNMAP); }

// -----------------------------------------------------------------------------
bool SamRecord::IsPrimary() const {
  // "... satisfies ‘FLAG& 0x900 == 0’.  This line is called the primary line of
  // the read."
  return CheckNFlag(BAM_FSECONDARY | BAM_FSUPPLEMENTARY);
}

// -----------------------------------------------------------------------------
bool SamRecord::IsSecondary() const { return CheckFlag(BAM_FSECONDARY); }

// -----------------------------------------------------------------------------
bool SamRecord::IsDuplicates() const { return CheckFlag(BAM_FDUP); }

// -----------------------------------------------------------------------------
bool SamRecord::IsSupplementary() const {
  return CheckFlag(BAM_FSUPPLEMENTARY);
}

// -----------------------------------------------------------------------------
bool SamRecord::IsPaired() const { return CheckFlag(BAM_FPAIRED); }

// -----------------------------------------------------------------------------
bool SamRecord::IsRead1() const { return CheckFlag(BAM_FPAIRED | BAM_FREAD1); }

// -----------------------------------------------------------------------------
bool SamRecord::IsRead2() const { return CheckFlag(BAM_FPAIRED | BAM_FREAD2); }

// -----------------------------------------------------------------------------
bool SamRecord::IsProperlyPaired() const {
  return CheckFlag(BAM_FPAIRED | BAM_FPROPER_PAIR) && CheckNFlag(BAM_FUNMAP);
}

// -----------------------------------------------------------------------------
bool SamRecord::IsPairedAndBothMapped() const {
  return CheckFlag(BAM_FPAIRED) && CheckNFlag(BAM_FUNMAP | BAM_FMUNMAP);
}

// -----------------------------------------------------------------------------
bool SamRecord::IsReverse() const { return CheckFlag(BAM_FREVERSE); }

// -----------------------------------------------------------------------------
bool SamRecord::IsPairOf(const SamRecord& r) const {
  /// Both must be paired-ended
  if (IsPaired() != r.IsPaired() || !IsPaired()) {
    return false;
  }

  if (IsPrimary() == r.IsPrimary()) {
    if (IsUnmapped() == r.IsMateUnmapped() &&
        r.IsUnmapped() == IsMateUnmapped() && GetMPos() == r.GetPos() &&
        r.GetMPos() == GetPos()) {
      /// Is pair because one of the RNEXT cannot be determined
      if (GetMPos() == 0 || r.GetMPos() == 0) {
        return true;
      }
      return GetRid() == r.GetMrid() && r.GetRid() == GetMrid() &&
             IsRead1() != r.IsRead1();
    }
  }

  return false;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam::sam_to_mgrec

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
