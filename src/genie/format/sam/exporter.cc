/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/sam/exporter.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/record/alignment_split/other_rec.h"
#include "genie/core/record/alignment_split/same_rec.h"
#include "genie/core/record/record.h"
#include "genie/format/sam/importer.h"
#include "genie/util/ordered_section.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

namespace genie::format::sam {

// -----------------------------------------------------------------------------

Exporter::Exporter(std::string ref_file, std::string output_file)
    : fasta_file_path_(std::move(ref_file)),
      output_file_path_(std::move(output_file)) {}

// -----------------------------------------------------------------------------

int StepRef(const char token) {
  static const auto lut_loc = []() -> std::string {  // NOLINT
    std::string lut(128, 0);
    lut['M'] = 1;
    lut['='] = 1;
    lut['X'] = 1;
    lut['I'] = 0;
    lut['D'] = 1;
    lut['N'] = 1;
    lut['S'] = 0;
    lut['H'] = 0;
    lut['P'] = 0;
    return lut;
  }();
  UTILS_DIE_IF(token < 0, "Invalid cigar token" + std::to_string(token));
  return lut_loc[token];
}

// -----------------------------------------------------------------------------

uint64_t MappedLength(const std::string& cigar) {
  if (cigar == "*") {
    return 0;
  }
  std::string digits;
  uint64_t length = 0;
  for (const auto& c : cigar) {
    if (std::isdigit(c)) {
      digits += c;
      continue;
    }
    length += std::stoi(digits) * StepRef(c);
    digits.clear();
  }
  return length;
}

// -----------------------------------------------------------------------------

char ConvertECigar2CigarChar(const char token) {
  static const auto lut_loc = []() -> std::string {  // NOLINT
    std::string lut(128, 0);
    lut['='] = 'M';
    lut['+'] = 'I';
    lut['-'] = 'D';
    lut['*'] = 'N';
    lut[')'] = 'S';
    lut[']'] = 'H';
    lut['%'] = 'N';
    lut['/'] = 'N';
    return lut;
  }();
  UTILS_DIE_IF(token < 0, "Invalid cigar token" + std::to_string(token));
  const char ret = lut_loc[token];
  UTILS_DIE_IF(ret == 0, "Invalid cigar token" + std::to_string(token));
  return ret;
}

// -----------------------------------------------------------------------------

std::string ECigar2Cigar(const std::string& ecigar) {
  std::string cigar;
  cigar.reserve(ecigar.size());
  size_t match_count = 0;
  std::string number_buffer;
  for (const auto& c : ecigar) {
    if (c == '[' || c == '(') {
      continue;
    }
    if (std::isdigit(c)) {
      number_buffer.push_back(c);
      continue;
    }
    if (GetAlphabetProperties(core::AlphabetId::kAcgtn).IsIncluded(c)) {
      match_count += 1;
    } else {
      if (c == '=') {
        match_count += std::stol(number_buffer);
        number_buffer.clear();
        continue;
      }
      if (match_count) {
        cigar += std::to_string(match_count) + "M";
        match_count = 0;
      }
      cigar += number_buffer;
      number_buffer.clear();
      cigar.push_back(ConvertECigar2CigarChar(c));
    }
  }
  if (match_count) {
    cigar += std::to_string(match_count) + "M";
  }
  return cigar;
}

// -----------------------------------------------------------------------------

void ProcessSecondMappedSegment(const size_t s,
                                const core::record::Record& record,
                                int64_t& tlen, uint16_t& flags,
                                std::string& pnext, std::string& rnext,
                                const RefInfo& refinfo) {
  // According to SAM standard, primary alignments only
  const auto split_type =
      record.GetClassId() == core::record::ClassType::kClassHm
          ? core::record::AlignmentSplit::Type::kUnpaired
          : record.GetAlignments()[0].GetAlignmentSplits().front()->GetType();
  if ((s == 1 && split_type == core::record::AlignmentSplit::Type::kSameRec) ||
      split_type == core::record::AlignmentSplit::Type::kUnpaired) {
    // Paired read is first read
    pnext = std::to_string(record.GetAlignments()[0].GetPosition() + 1);
    tlen += static_cast<int64_t>(record.GetAlignments()[0].GetPosition() + 1);
    rnext = refinfo.IsValid()
                ? refinfo.GetMgr()->Id2Ref(
                      record.GetAlignmentSharedData().GetSeqId())
                : std::to_string(record.GetAlignmentSharedData().GetSeqId());
    if (record.GetAlignments()[0].GetAlignment().GetRComp()) {
      flags |= 0x20;
    }
  } else {
    // Paired read is second read
    if (split_type == core::record::AlignmentSplit::Type::kSameRec) {
      const auto& split =
          dynamic_cast<const core::record::alignment_split::SameRec&>(
              *record.GetAlignments()[0].GetAlignmentSplits().front().get());
      pnext = std::to_string(record.GetAlignments()[0].GetPosition() +
                             split.GetDelta() + 1);
      tlen += static_cast<int64_t>(
          record.GetAlignments()[0].GetPosition() + split.GetDelta() + 1 +
          MappedLength(ECigar2Cigar(split.GetAlignment().GetECigar())));

      rnext = refinfo.IsValid()
                  ? refinfo.GetMgr()->Id2Ref(
                        record.GetAlignmentSharedData().GetSeqId())
                  : std::to_string(record.GetAlignmentSharedData().GetSeqId());
      if (split.GetAlignment().GetRComp()) {
        flags |= 0x20;
      }
    } else if (split_type == core::record::AlignmentSplit::Type::kOtherRec) {
      const auto& split =
          dynamic_cast<const core::record::alignment_split::OtherRec&>(
              *record.GetAlignments()[0].GetAlignmentSplits().front().get());
      rnext = refinfo.IsValid() ? refinfo.GetMgr()->Id2Ref(split.GetNextSeq())
                                : std::to_string(split.GetNextSeq());
      pnext = std::to_string(split.GetNextPos() + 1);
      if (record.IsExtendedAlignment()) {
        if (split.GetExtendedAlignment().value().GetRComp()) {
          flags |= 0x20;
        }
      }
      tlen = 0;  // Not available without reading second record
    } else {
      tlen = 0;
    }
  }
}

// -----------------------------------------------------------------------------

uint16_t ComputeSamFlags(const size_t s, const core::record::Record& record) {
  uint16_t flags = 0;
  if (record.GetNumberOfTemplateSegments() > 1) {
    flags |= 0x1;
  }
  if (record.GetFlagsOfSegment(s) & core::gen_const::kFlagsProperPairMask) {
    flags |= 0x2;
  }
  // This read is unmapped
  if (record.GetClassId() == core::record::ClassType::kClassU ||
      (record.GetClassId() == core::record::ClassType::kClassHm && s == 1)) {
    flags |= 0x4;
  }
  // Paired read is unmapped
  if (record.GetClassId() == core::record::ClassType::kClassU ||
      (record.GetClassId() == core::record::ClassType::kClassHm && s == 0)) {
    flags |= 0x8;
  }
  // First or second read?
  if ((record.IsRead1First() && s == 0) || (!record.IsRead1First() && s == 1)) {
    flags |= 0x40;
  } else {
    flags |= 0x80;
  }
  // Secondary alignment
  if (record.GetFlagsOfSegment(s) & core::gen_const::kFlagsNotPrimaryMask) {
    flags |= 0x100;
  }
  if (record.GetFlagsOfSegment(s) & core::gen_const::kFlagsSupplementaryMask) {
    flags |= 0x800;
  }
  if (record.GetFlagsOfSegment(s) & core::gen_const::kFlagsQualityFailMask) {
    flags |= 0x200;
  }
  if (record.GetFlagsOfSegment(s) & core::gen_const::kFlagsPcrDuplicateMask) {
    flags |= 0x400;
  }

  // If 0x4 is set, no assumptions can be made about 0x2, 0x100, and 0x800.
  if (flags & 0x4) {
    flags &= ~0x2;
    flags &= ~0x100;
    flags &= ~0x800;
  }

  // If 0x1 is unset, no assumptions can be made about 0x2, 0x8, 0x20,
  // 0x40 and 0x80
  if (!(flags & 0x1)) {
    flags &= ~0x2;
    flags &= ~0x8;
    flags &= ~0x20;
    flags &= ~0x40;
    flags &= ~0x80;
  }
  return flags;
}

// -----------------------------------------------------------------------------

void ProcessFirstMappedSegment(const size_t s, const size_t a,
                               const core::record::Record& record,
                               std::string& rname, std::string& pos,
                               int64_t& tlen, std::string& mapping_qual,
                               std::string& cigar, uint16_t& flags,
                               const RefInfo& refinfo) {
  // This read is mapped, process mapping
  rname =
      refinfo.IsValid()
          ? refinfo.GetMgr()->Id2Ref(record.GetAlignmentSharedData().GetSeqId())
          : std::to_string(record.GetAlignmentSharedData().GetSeqId());
  if (s == 0) {
    // First segment is in primary alignment
    pos = std::to_string(record.GetAlignments()[a].GetPosition() + 1);
    tlen -= static_cast<int64_t>(record.GetAlignments()[a].GetPosition() + 1);
    mapping_qual =
        record.GetAlignments()[a].GetAlignment().GetMappingScores().empty()
            ? "0"
            : std::to_string(record.GetAlignments()[a]
                                 .GetAlignment()
                                 .GetMappingScores()
                                 .front());
    cigar = ECigar2Cigar(record.GetAlignments()[a].GetAlignment().GetECigar());
    if (record.GetAlignments()[a].GetAlignment().GetRComp()) {
      flags |= 0x10;
    }
  } else {
    // First segment is mapped in split alignment
    const auto& split =
        dynamic_cast<const core::record::alignment_split::SameRec&>(
            *record.GetAlignments()[a].GetAlignmentSplits().front().get());
    cigar = ECigar2Cigar(split.GetAlignment().GetECigar());
    pos = std::to_string(record.GetAlignments()[a].GetPosition() +
                         split.GetDelta() + 1);
    tlen -= static_cast<int64_t>(record.GetAlignments()[a].GetPosition() +
                                 split.GetDelta() + 1 + MappedLength(cigar));
    mapping_qual =
        split.GetAlignment().GetMappingScores().empty()
            ? "0"
            : std::to_string(split.GetAlignment().GetMappingScores().front());
    if (split.GetAlignment().GetRComp()) {
      flags |= 0x10;
    }
  }
}

// -----------------------------------------------------------------------------

void Exporter::SkipIn(const util::Section& id) {
  [[maybe_unused]] util::OrderedSection sec(&lock_, id);
}

void Exporter::FlowIn(core::record::Chunk&& records, const util::Section& id) {
  core::record::Chunk data = std::move(records);
  GetStats().Add(data.GetStats());
  util::Watch watch;
  [[maybe_unused]] util::OrderedSection section(&lock_, id);
  size_t size_seq = 0;
  size_t size_qual = 0;
  size_t size_name = 0;

  RefInfo refinf(fasta_file_path_);

  if (!output_set_ && output_file_path_.substr(0, 2) != "-.") {
    output_stream_ = std::ofstream(output_file_path_);
    output_file_ = &output_stream_.value();
  }
  if (!output_set_) {
    *output_file_ << "@HD\tVN:1.6" << std::endl;
    for (const auto& s : refinf.GetMgr()->GetSequences()) {
      *output_file_ << "@SQ\tSN:" << s
                    << "\tLN:" << std::to_string(refinf.GetMgr()->GetLength(s))
                    << std::endl;
    }
    output_set_ = true;
  }

  for (auto& record : data.GetData()) {
    // One line per segment and alignment
    for (size_t s = 0; s < record.GetSegments().size(); ++s) {
      for (size_t a = 0;
           a < std::max(record.GetAlignments().size(), static_cast<size_t>(1));
           ++a) {
        std::string sam_record = record.GetName() + "\t";

        uint16_t flags = ComputeSamFlags(s, record);
        bool mapped = !(flags & 0x4);
        bool other_mapped = !(flags & 0x8);

        std::string rname = "*";
        std::string pos = "0";
        std::string mapping_qual = "0";
        std::string cigar = "*";
        std::string rnext = "*";
        std::string pnext = "0";
        int64_t tlen = 0;
        if (mapped) {
          // First segment is mapped
          ProcessFirstMappedSegment(s, a, record, rname, pos, tlen,
                                    mapping_qual, cigar, flags, refinf);
        } else if (record.GetClassId() == core::record::ClassType::kClassHm) {
          // According to SAM standard, HM-like records should have
          // same position for the unmapped part, too
          pos = std::to_string(record.GetAlignments()[a].GetPosition() + 1);
          rname =
              refinf.IsValid()
                  ? refinf.GetMgr()->Id2Ref(
                        record.GetAlignmentSharedData().GetSeqId())
                  : std::to_string(record.GetAlignmentSharedData().GetSeqId());
          if (a > 0) {
            // No need to write unmapped records for secondary
            // alignmentd
            continue;
          }
        }

        if (other_mapped && record.GetNumberOfTemplateSegments() == 2) {
          ProcessSecondMappedSegment(s, record, tlen, flags, pnext, rnext,
                                     refinf);
          // Use "=" shorthand
          if (rnext == rname && rnext != "*") {
            rnext = "=";
          }
        } else {
          tlen = 0;
        }

        if (record.GetClassId() == core::record::ClassType::kClassHm ||
            record.GetClassId() == core::record::ClassType::kClassU) {
          tlen = 0;
        }

        sam_record += std::to_string(flags) + "\t";
        sam_record += rname + "\t";
        sam_record += pos + "\t";
        sam_record += mapping_qual + "\t";
        sam_record += cigar + "\t";
        sam_record += rnext + "\t";
        sam_record += pnext + "\t";
        sam_record += std::to_string(tlen) + "\t";
        sam_record += record.GetSegments()[s].GetSequence() + "\t";
        if (record.GetSegments()[s].GetQualities().empty() ||
            record.GetSegments()[s].GetQualities().front().empty()) {
          sam_record += "*\n";
        } else {
          sam_record += record.GetSegments()[s].GetQualities()[0] + "\n";
        }

        output_file_->write(sam_record.c_str(),
                            static_cast<std::streamsize>(sam_record.length()));
      }
    }
  }

  GetStats().AddInteger("size-sam-seq", static_cast<int64_t>(size_seq));
  GetStats().AddInteger("size-sam-name", static_cast<int64_t>(size_name));
  GetStats().AddInteger("size-sam-qual", static_cast<int64_t>(size_qual));
  GetStats().AddInteger("size-sam-total",
                        static_cast<int64_t>(size_qual + size_name + size_seq));
  GetStats().AddDouble("time-sam-export", watch.Check());
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
