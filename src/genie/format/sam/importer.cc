/**
 * Copyright 2018-2024 The Genie Authors.
 * @file importer.cc
 *
 * @brief Implements SAM to MPEG-G format importer for genomic data processing.
 *
 * This file is part of the Genie project and provides the implementation of the
 * `Importer` class, which handles the conversion of SAM and FASTQ files into
 * the MPEG-G record (MGREC) format. The file includes multiple phases of
 * processing, such as reading, cleaning, splitting, and merging genomic data,
 * with support for references and quality validation.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/sam/importer.h"

#include <algorithm>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/cigar_tokenizer.h"
#include "genie/core/record/alignment_split/other_rec.h"
#include "genie/core/record/alignment_split/same_rec.h"
#include "genie/core/record/class_type.h"
#include "genie/format/sam/sam_reader.h"
#include "genie/format/sam/sam_sorter.h"
#include "genie/util/ordered_section.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "TranscoderSam";

// -----------------------------------------------------------------------------

namespace genie::format::sam {

// -----------------------------------------------------------------------------

core::record::Record::Flags GetFlags(const SamRecord& rec) {
  core::record::Record::Flags flags;
  flags.duplicate = rec.IsDuplicates();
  flags.quality_check_fail = rec.IsQualityFail();
  flags.proper_mapped_pair = rec.IsProperlyPaired();
  flags.not_primary_alignment = !rec.IsPrimary();
  flags.supplementary_alignment = rec.IsSupplementary();
  return flags;
}

// -----------------------------------------------------------------------------

RefInfo::RefInfo(const std::string& fasta_name)
    : ref_mgr_(std::make_unique<core::ReferenceManager>(4)), valid_(false) {
  if (!std::filesystem::exists(fasta_name)) {
    return;
  }

  std::string fai_name =
      fasta_name.substr(0, fasta_name.find_last_of('.')) + ".fai";
  std::string sha_name =
      fasta_name.substr(0, fasta_name.find_last_of('.')) + ".sha256";
  if (!std::filesystem::exists(fai_name)) {
    std::ifstream fasta_in(fasta_name);
    UTILS_DIE_IF(!fasta_in, "Cannot open file to read: " + fasta_name);
    std::ofstream fai_out(fai_name);
    fasta::FastaReader::index(fasta_in, fai_out);
  }
  if (!std::filesystem::exists(sha_name)) {
    std::ifstream fasta_in(fasta_name);
    UTILS_DIE_IF(!fasta_in, "Cannot open file to read: " + fasta_name);
    std::ifstream fai_in(fai_name);
    UTILS_DIE_IF(!fai_in, "Cannot open file to read: " + fai_name);
    fasta::FaiFile faifile(fai_in);
    std::ofstream sha_out(sha_name);
    fasta::FastaReader::hash(faifile, fasta_in, sha_out);
  }

  fasta_file_ = std::make_unique<std::ifstream>(fasta_name);
  UTILS_DIE_IF(!fasta_file_, "Cannot open file to read: " + fasta_name);
  fai_file_ = std::make_unique<std::ifstream>(fai_name);
  UTILS_DIE_IF(!fai_file_, "Cannot open file to read: " + fai_name);
  sha_file_ = std::make_unique<std::ifstream>(sha_name);
  UTILS_DIE_IF(!sha_file_, "Cannot open file to read: " + sha_name);

  fasta_mgr_ = std::make_unique<fasta::Manager>(
      *fasta_file_, *fai_file_, *sha_file_, ref_mgr_.get(), fasta_name);
  valid_ = true;
}

// -----------------------------------------------------------------------------

bool RefInfo::IsValid() const { return valid_; }

// -----------------------------------------------------------------------------

core::ReferenceManager* RefInfo::GetMgr() const { return ref_mgr_.get(); }

std::pair<std::vector<SamRecord>, std::optional<int>> Importer::ReadSamChunk() {
  RefIdWatcher watcher;
  std::vector<SamRecord> sam_records;
  std::optional<uint64_t> last_pos = std::nullopt;
  if (eof_) {
    return {sam_records, watcher.get()};
  }
  for (int i = 0; i < PHASE2_BUFFER_SIZE; ++i) {
    if (sam_reader_.Peek() == std::nullopt) {
      eof_ = true;
      return {sam_records, watcher.get()};
    }
    if (watcher.watch(sam_reader_.Peek()->rid_)) {
      return {sam_records, watcher.get()};
    }
    sam_records.emplace_back(*sam_reader_.Move());

    if (!sam_records.back().IsUnmapped()) {
      if (!last_pos.has_value()) {
        last_pos = sam_records.back().pos_;
      }
      UTILS_DIE_IF(last_pos > sam_records.back().pos_,
                   "SAM file is not sorted");
      last_pos = sam_records.back().pos_;
    }

    if (!sam_records.back().IsUnmapped()) {
      const auto is_invalid =
          sam_records.back().rid_ < 0 ||
          sam_records.back().rid_ >=
              static_cast<int32_t>(sam_hdr_to_fasta_lut_.size());
      UTILS_DIE_IF(is_invalid, "Reference ID not found in FASTA header: " +
                                   std::to_string(sam_records.back().rid_));
      sam_records.back().rid_ =
          sam_hdr_to_fasta_lut_.at(sam_records.back().rid_);
    }
    if (sam_records.back().IsPaired() && !sam_records.back().IsMateUnmapped()) {
      const auto is_invalid =
          sam_records.back().mate_rid_ < 0 ||
          sam_records.back().mate_rid_ >=
              static_cast<int32_t>(sam_hdr_to_fasta_lut_.size());
      UTILS_DIE_IF(is_invalid,
                   "Reference ID for mate not found in FASTA header: " +
                       std::to_string(sam_records.back().mate_rid_));
      sam_records.back().mate_rid_ =
          sam_hdr_to_fasta_lut_.at(sam_records.back().mate_rid_);
    }
    sam_reader_.Read();
  }
  return {sam_records, watcher.get()};
}

// -----------------------------------------------------------------------------

std::vector<SamRecordPair> Importer::MatchPairs(
    std::vector<SamRecord>&& records) {
  for (auto& r : records) {
    sorter_.AddSamRead(r);
  }
  records.clear();
  if (eof_) {
    sorter_.Finish();
  }
  return sorter_.GetPairs();
}

// -----------------------------------------------------------------------------

core::record::Segment CreateSegment(SamRecord& sam_record) {
  UTILS_DIE_IF(sam_record.seq_.empty() || sam_record.seq_ == "*",
               "Empty nucleotide sequence not supported");
  core::record::Segment segment(std::move(sam_record.seq_));
  segment.AddQualities(std::move(sam_record.qual_));
  return segment;
}

// -----------------------------------------------------------------------------

core::record::Alignment CreateAlignment(const SamRecord& sam_record,
                                        const std::string& seq,
                                        const bool extended_alignment) {
  std::string e_cigar = SamRecord::ConvertCigar2ECigar(sam_record.cigar_, seq);
  core::record::Alignment alignment(std::move(e_cigar), sam_record.IsReverse());
  if (extended_alignment) {
    alignment.SetFlags(GetFlags(sam_record).Get());
  }
  return alignment;
}

// -----------------------------------------------------------------------------

uint8_t GetNumberOfTemplateSegments(const SamRecordPair& pair) {
  return pair.first.IsPaired() ? 2 : 1;
}

// -----------------------------------------------------------------------------

core::record::ClassType GetClassType(const SamRecordPair& pair) {
  // Unpaired
  if (!pair.first.IsPaired()) {
    if (pair.first.IsUnmapped()) {
      return core::record::ClassType::kClassU;
    }
    return core::record::ClassType::kClassI;
  }
  // Paired
  if (pair.first.IsUnmapped() && pair.first.IsMateUnmapped()) {
    return core::record::ClassType::kClassU;
  }
  if (!pair.first.IsUnmapped() && !pair.first.IsMateUnmapped()) {
    return core::record::ClassType::kClassI;
  }
  // Half mapped
  if (pair.second.has_value()) {
    return core::record::ClassType::kClassHm;
  }
  if (pair.first.IsUnmapped()) {
    return core::record::ClassType::kClassU;
  }
  return core::record::ClassType::kClassI;
}

// -----------------------------------------------------------------------------

SamRecordPair FixOrder(SamRecordPair&& pair) {
  if (!pair.second.has_value()) {
    return std::move(pair);
  }
  // HM record: Mapped record first
  if (pair.first.IsUnmapped() && !pair.first.IsMateUnmapped()) {
    return {std::move(*pair.second), std::move(pair.first)};
  }
  // I record: Read with lower mapping position first
  if (!pair.first.IsUnmapped() && !pair.first.IsMateUnmapped()) {
    if (pair.first.pos_ > pair.first.mate_pos_) {
      return {std::move(*pair.second), std::move(pair.first)};
    }
  }
  // U record: Read1 of template first
  if (pair.first.IsUnmapped() && pair.first.IsMateUnmapped()) {
    if (pair.first.IsRead2()) {
      return {std::move(*pair.second), std::move(pair.first)};
    }
  }
  return std::move(pair);
}

// -----------------------------------------------------------------------------

Importer::Importer(const size_t block_size, const bool extended_alignment,
                   std::string input, std::string ref)
    : block_size_(block_size),
      input_sam_file_(std::move(input)),
      input_ref_file_(std::move(ref)),
      nref_(0),
      phase1_complete_(false),
      refinf_(input_ref_file_),
      eof_(false),
      sam_reader_(this->input_sam_file_),
      sorter_(100000),
      extended_alignment_(extended_alignment) {
  refs_ = sam_reader_.GetRefs();
  if (!input_ref_file_.empty()) {
    for (const auto& [fst, snd] : refs_) {
      bool found = false;
      for (size_t j = 0; j < refinf_.GetMgr()->GetSequences().size(); ++j) {
        if (fst == refinf_.GetMgr()->GetSequences().at(j)) {
          sam_hdr_to_fasta_lut_.push_back(j);
          found = true;
          break;
        }
      }
      UTILS_DIE_IF(!found, "Did not find ref " + fst);
    }
  } else {
    for (size_t i = 0; i < refs_.size(); ++i) {
      sam_hdr_to_fasta_lut_.push_back(i);
    }
  }
}

// -----------------------------------------------------------------------------

bool IsOriginalOrder(const SamRecordPair& pair) {
  if (pair.first.IsPaired() && pair.first.IsRead2()) {
    return false;
  }
  return true;
}

// -----------------------------------------------------------------------------

core::record::Record AddSegments(core::record::Record&& rec,
                                 SamRecordPair& corrected_pair) {
  rec.AddSegment(CreateSegment(corrected_pair.first));
  if (corrected_pair.second.has_value()) {
    rec.AddSegment(CreateSegment(*corrected_pair.second));
  }
  return std::move(rec);
}

// -----------------------------------------------------------------------------

core::record::AlignmentBox AddSplitAlignment(
    core::record::AlignmentBox&& box, const core::record::Record& rec,
    const SamRecordPair& corrected_pair,
    const core::record::ClassType class_type, const bool extended_alignment) {
  // Class HM or unpaired: No split alignment
  if (class_type != core::record::ClassType::kClassI ||
      !corrected_pair.first.IsPaired()) {
    return std::move(box);
  }

  if (corrected_pair.second.has_value()) {
    auto alignment = CreateAlignment(*corrected_pair.second,
                                     rec.GetSegments().back().GetSequence(),
                                     extended_alignment);
    alignment.AddMappingScore(corrected_pair.second->mapq_);
    // Second mate available
    box.AddAlignmentSplit(
        std::make_unique<core::record::alignment_split::SameRec>(
            corrected_pair.second->pos_ - corrected_pair.first.pos_,
            std::move(alignment)));
  } else {
    std::optional<core::record::Alignment> a = std::nullopt;
    if (extended_alignment) {
      a = core::record::Alignment("", corrected_pair.first.GetFlag() & 0x20);
      a->AddMappingScore(255);
    }
    // Second mate unavailable
    box.AddAlignmentSplit(
        std::make_unique<core::record::alignment_split::OtherRec>(
            corrected_pair.first.mate_pos_, corrected_pair.first.mate_rid_, a));
  }
  return std::move(box);
}

// -----------------------------------------------------------------------------

core::record::AlignmentBox CreateFirstAlignment(
    const core::record::Record& rec, const SamRecordPair& corrected_pair,
    const bool extended_alignment_) {
  auto alignment = CreateAlignment(corrected_pair.first,
                                   rec.GetSegments().front().GetSequence(),
                                   extended_alignment_);
  alignment.AddMappingScore(corrected_pair.first.mapq_);
  return core::record::AlignmentBox(corrected_pair.first.pos_,
                                    std::move(alignment));
}

// -----------------------------------------------------------------------------

core::record::Record AddAlignments(core::record::Record&& rec,
                                   const SamRecordPair& corrected_pair,
                                   const core::record::ClassType class_type,
                                   const bool extended_alignment) {
  // Class U -> No Alignments
  if (class_type == core::record::ClassType::kClassU) {
    return std::move(rec);
  }
  // First alignment
  auto alignment_box =
      CreateFirstAlignment(rec, corrected_pair, extended_alignment);
  // Split alignment
  alignment_box =
      AddSplitAlignment(std::move(alignment_box), rec, corrected_pair,
                        class_type, extended_alignment);
  rec.AddAlignment(corrected_pair.first.rid_, std::move(alignment_box));
  return std::move(rec);
}

// -----------------------------------------------------------------------------

core::record::Record ConvertPair(SamRecordPair&& pair,
                                 bool extended_alignment) {
  auto corrected_pair = FixOrder(std::move(pair));
  auto class_type = GetClassType(corrected_pair);
  core::record::Record ret(GetNumberOfTemplateSegments(corrected_pair),
                           class_type, std::move(corrected_pair.first.qname_),
                           "", GetFlags(corrected_pair.first).Get(),
                           IsOriginalOrder(corrected_pair));
  ret = AddSegments(std::move(ret), corrected_pair);
  ret = AddAlignments(std::move(ret), corrected_pair, class_type,
                      extended_alignment);
  if (extended_alignment) {
    if (ret.GetClassId() == core::record::ClassType::kClassHm) {
      ret.SetFlags(0, GetFlags(pair.second.value()).Get());
    }
    if (ret.GetClassId() == core::record::ClassType::kClassU) {
      ret.SetExtendedAlignment(true);
      ret.SetFlags(0, GetFlags(pair.first).Get());
      if (pair.second.has_value()) {
        ret.SetFlags(1, GetFlags(pair.second.value()).Get());
      }
    }
  }
  return ret;
}

// -----------------------------------------------------------------------------

struct SamStats {
  size_t size_seq = 0;
  size_t size_qual = 0;
  size_t size_name = 0;
};

// -----------------------------------------------------------------------------

SamStats CollectStats(SamStats stats, const SamRecordPair& pair) {
  stats.size_seq += pair.first.seq_.size();
  stats.size_qual += pair.first.qual_.size();
  stats.size_name += pair.first.qname_.size();
  if (pair.second.has_value()) {
    stats.size_seq += pair.second->seq_.size();
    stats.size_qual += pair.second->qual_.size();
    stats.size_name += pair.second->qname_.size();
  }
  return stats;
}

// -----------------------------------------------------------------------------

bool Importer::PumpRetrieve(core::Classifier* classifier) {
  core::record::Chunk chunk;
  util::Watch watch;
  const util::Watch full_watch;
  std::vector<SamRecordPair> sam_pairs;
  {
    std::lock_guard guard(lock_);
    auto [sam_records, ref_id] = ReadSamChunk();
    chunk.GetStats().AddDouble("time-sam-importer-read", watch.Check());
    watch.Reset();
    sam_pairs = MatchPairs(std::move(sam_records));
    chunk.GetStats().AddDouble("time-sam-importer-match-pairs", watch.Check());
    watch.Reset();
  }

  std::optional<size_t> current_ref_id;
  SamStats stats;
  for (auto& p : sam_pairs) {
    stats = CollectStats(stats, p);
    chunk.GetData().emplace_back(
        ConvertPair(std::move(p), extended_alignment_));
    if (!current_ref_id.has_value() &&
        !chunk.GetData().back().GetAlignments().empty()) {
      current_ref_id =
          chunk.GetData().back().GetAlignmentSharedData().GetSeqId();
    }
  }
  if (current_ref_id.has_value()) {
    chunk.SetRefId(current_ref_id.value());
  }
  sam_pairs.clear();
  chunk.GetStats().AddDouble("time-sam-importer-convert", watch.Check());
  chunk.GetStats().AddDouble("time-sam-importer-total", full_watch.Check());
  chunk.GetStats().AddInteger("size-sam-importer-seq", stats.size_seq);
  chunk.GetStats().AddInteger("size-sam-importer-qual", stats.size_qual);
  chunk.GetStats().AddInteger("size-sam-importer-name", stats.size_name);
  chunk.GetStats().AddInteger(
      "size-sam-importer-total",
      stats.size_name + stats.size_seq + stats.size_qual);

  classifier->Add(std::move(chunk));
  return !eof_;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
