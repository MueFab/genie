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
#include <iostream>
#include <list>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/cigar_tokenizer.h"
#include "genie/core/record/alignment_split/other_rec.h"
#include "genie/core/record/alignment_split/same_rec.h"
#include "genie/core/record/class_type.h"
#include "genie/format/sam/sam_to_mgrec/sam_group.h"
#include "genie/format/sam/sam_to_mgrec/sam_reader.h"
#include "genie/format/sam/sam_to_mgrec/sorter.h"
#include "genie/format/sam/sam_to_mgrec/transcoder.h"
#include "genie/util/log.h"
#include "genie/util/ordered_section.h"
#include "genie/util/stop_watch.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "TranscoderSam";

namespace genie::format::sam {

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

// -----------------------------------------------------------------------------
Importer::Importer(const size_t block_size, std::string input, std::string ref)
    : block_size_(block_size),
      input_sam_file_(std::move(input)),
      input_ref_file_(std::move(ref)),
      nref_(0),
      phase1_complete_(false),
      reader_prio_(CmpReaders()),
      refinf_(input_ref_file_) {}

// -----------------------------------------------------------------------------
bool CmpReaders::operator()(const sam_to_mgrec::SubfileReader* a,
                            const sam_to_mgrec::SubfileReader* b) const {
  return !sam_to_mgrec::compare(a->GetRecord().value(), b->GetRecord().value());
}

struct CleanStatistics {
  size_t hm_recs{};
  size_t splice_recs{};
  size_t distance{};
  size_t additional_alignments{};
};

// -----------------------------------------------------------------------------
std::string patch_ecigar(const std::string& ref, const std::string& seq,
                         const std::string& e_cigar) {
  std::string fixed_cigar;
  auto class_checker = [&](const uint8_t cigar,                  // NOLINT
                           const std::pair<size_t, size_t>& bs,  // NOLINT
                           const std::pair<size_t, size_t>& rs) -> bool {
    const auto bs_ext = std::string_view(seq).substr(bs.first, bs.second);
    const auto rs_ext = std::string_view(ref).substr(rs.first, rs.second);
    const auto length = std::max(bs_ext.length(), rs_ext.length());
    switch (cigar) {
      case '+':
      case '-':
      case ')':
      case ']':
      case '*':
      case '/':
      case '%':
        if (cigar == ')') {
          fixed_cigar += std::string(1, '(');
        }
        if (cigar == ']') {
          fixed_cigar += std::string(1, '[');
        }
        fixed_cigar +=
            std::to_string(length) + std::string(1, static_cast<char>(cigar));
        return true;
      default:
        break;
    }

    if (GetAlphabetProperties(core::AlphabetId::kAcgtn)
            .IsIncluded(static_cast<char>(cigar))) {
      fixed_cigar += std::string(1, static_cast<char>(cigar));
      return true;
    }

    UTILS_DIE_IF(cigar != '=', "Unknown ecigar char.");

    size_t counter = 0;

    for (size_t i = 0; i < length; ++i) {
      if (*(bs_ext.begin() + i) != *(rs_ext.begin() + i)) {
        if (counter != 0) {
          fixed_cigar += std::to_string(counter) + "=";
          counter = 0;
        }
        fixed_cigar += std::string(1, *(bs_ext.begin() + i));

      } else {
        counter++;
      }
    }

    if (counter != 0) {
      fixed_cigar += std::to_string(counter) + "=";
    }

    return true;
  };

  core::CigarTokenizer::Tokenize(e_cigar, core::GetECigarInfo(), class_checker);
  return fixed_cigar;
}

// -----------------------------------------------------------------------------
core::record::ClassType ClassifyEcigar(const std::string&) {
  return core::record::ClassType::kClassI;
  /*
  auto ret = core::record::ClassType::kClassP;
  for (const auto& c : cigar) {
    if (c >= '0' && c <= '9') {
      continue;
    }
    if (c == '+' || c == '-' || c == '(' || c == '[') {
      return core::record::ClassType::kClassI;
    }
    if (GetAlphabetProperties(core::AlphabetId::kAcgtn).IsIncluded(c)) {
      if (c == 'N') {
        ret = std::max(ret, core::record::ClassType::kClassN);
      } else {
        ret = std::max(ret, core::record::ClassType::kClassM);
      }
    }
  }
  return ret;*/
}

// -----------------------------------------------------------------------------
bool ValidateBases(const std::string& seq, const core::Alphabet& alphabet) {
  return std::all_of(seq.begin(), seq.end(), [&alphabet](const char& c) {
    return alphabet.IsIncluded(c);
  });
}

// -----------------------------------------------------------------------------
bool fix_ecigar(core::record::Record& r,
                const std::vector<std::pair<std::string, size_t>>&,
                const RefInfo& ref) {
  if (r.GetClassId() == core::record::ClassType::kClassU) {
    return true;
  }

  if (!ref.IsValid()) {
    return true;
  }

  size_t alignment_ctr = 0;
  for (auto& a : r.GetAlignments()) {
    auto pos = a.GetPosition();
    auto ref_seq =
        ref.GetMgr()
            ->Load(ref.GetMgr()->Id2Ref(r.GetAlignmentSharedData().GetSeqId()),
                   pos, pos + r.GetMappedLength(alignment_ctr, 0))
            .GetString(pos, pos + r.GetMappedLength(alignment_ctr, 0));
    auto cigar = a.GetAlignment().GetECigar();
    auto seq = r.GetSegments()[0].GetSequence();

    if (!ValidateBases(seq, GetAlphabetProperties(core::AlphabetId::kAcgtn))) {
      return false;
    }

    if (!ValidateBases(ref_seq,
                       GetAlphabetProperties(core::AlphabetId::kAcgtn))) {
      return false;
    }

    cigar = patch_ecigar(ref_seq, seq, cigar);

    if (r.GetClassId() != core::record::ClassType::kClassHm) {
      r.SetClassType(ClassifyEcigar(cigar));
    }

    auto alg =
        core::record::Alignment(std::move(cigar), a.GetAlignment().GetRComp());
    for (const auto& s : a.GetAlignment().GetMappingScores()) {
      alg.AddMappingScore(s);
    }
    core::record::AlignmentBox new_box(a.GetPosition(), std::move(alg));

    // -----------

    if (a.GetAlignmentSplits().size() == 1) {
      if (a.GetAlignmentSplits().front()->GetType() ==
          core::record::AlignmentSplit::Type::kSameRec) {
        const auto& split =
            dynamic_cast<const core::record::alignment_split::SameRec&>(
                *a.GetAlignmentSplits().front());
        pos = a.GetPosition() + split.GetDelta();
        auto ex = ref.GetMgr()->Load(
            ref.GetMgr()->Id2Ref(r.GetAlignmentSharedData().GetSeqId()), pos,
            pos + r.GetMappedLength(alignment_ctr, 1));
        ref_seq = ex.GetString(pos, pos + r.GetMappedLength(alignment_ctr, 1));
        cigar = split.GetAlignment().GetECigar();
        seq = r.GetSegments()[1].GetSequence();

        if (!ValidateBases(seq,
                           GetAlphabetProperties(core::AlphabetId::kAcgtn))) {
          return false;
        }

        if (!ValidateBases(ref_seq,
                           GetAlphabetProperties(core::AlphabetId::kAcgtn))) {
          return false;
        }

        cigar = patch_ecigar(ref_seq, seq, cigar);

        if (r.GetClassId() != core::record::ClassType::kClassHm) {
          r.SetClassType(std::max(r.GetClassId(), ClassifyEcigar(cigar)));
        }

        alg = core::record::Alignment(std::move(cigar),
                                      split.GetAlignment().GetRComp());
        for (const auto& s : split.GetAlignment().GetMappingScores()) {
          alg.AddMappingScore(s);
        }

        new_box.AddAlignmentSplit(
            std::make_unique<core::record::alignment_split::SameRec>(
                split.GetDelta(), std::move(alg)));
      } else {
        new_box.AddAlignmentSplit(a.GetAlignmentSplits().front()->clone());
      }
    }
    r.SetAlignment(alignment_ctr, std::move(new_box));
    alignment_ctr++;
  }
  return true;
}

std::vector<core::record::Record> SplitRecord(core::record::Record&& rec) {
  auto input = std::move(rec);
  std::vector<core::record::Record> ret;
  if (input.GetSegments().size() != 2) {
    ret.emplace_back(std::move(input));
    return ret;
  }
  switch (input.GetClassId()) {
    case core::record::ClassType::kClassHm: {
      ret.emplace_back(
          input.GetNumberOfTemplateSegments(), core::record::ClassType::kClassI,
          std::string(input.GetName()), std::string(input.GetGroup()),
          input.GetFlags(), input.IsRead1First());
      ret.back().SetQvDepth(1);
      ret.back().GetSegments().emplace_back(input.GetSegments()[0]);

      ret.emplace_back(
          input.GetNumberOfTemplateSegments(), core::record::ClassType::kClassU,
          std::string(input.GetName()), std::string(input.GetGroup()),
          input.GetFlags(), !input.IsRead1First());
      ret.back().SetQvDepth(1);
      ret.back().GetSegments().emplace_back(input.GetSegments()[1]);
    } break;
    case core::record::ClassType::kClassU:
      ret.emplace_back(
          input.GetNumberOfTemplateSegments(), core::record::ClassType::kClassU,
          std::string(input.GetName()), std::string(input.GetGroup()),
          input.GetFlags(), input.IsRead1First());
      ret.back().GetSegments().emplace_back(input.GetSegments()[0]);
      ret.back().SetQvDepth(1);

      ret.emplace_back(
          input.GetNumberOfTemplateSegments(), core::record::ClassType::kClassU,
          std::string(input.GetName()), std::string(input.GetGroup()),
          input.GetFlags(), !input.IsRead1First());
      ret.back().GetSegments().emplace_back(input.GetSegments()[1]);
      ret.back().SetQvDepth(1);
      break;
    default:
      ret.emplace_back(input.GetNumberOfTemplateSegments(), input.GetClassId(),
                       std::string(input.GetName()),
                       std::string(input.GetGroup()), input.GetFlags(),
                       input.IsRead1First());
      ret.back().SetQvDepth(1);
      ret.back().GetSegments().emplace_back(input.GetSegments()[0]);
      core::record::AlignmentBox box(
          input.GetAlignments().front().GetPosition(),
          core::record::Alignment(
              input.GetAlignments().front().GetAlignment()));

      const auto& split =
          dynamic_cast<const core::record::alignment_split::SameRec&>(
              *input.GetAlignments().front().GetAlignmentSplits().front());
      core::record::AlignmentBox box2(
          input.GetAlignments().front().GetPosition() + split.GetDelta(),
          core::record::Alignment(split.GetAlignment()));

      box.AddAlignmentSplit(
          std::make_unique<core::record::alignment_split::OtherRec>(
              box2.GetPosition(), input.GetAlignmentSharedData().GetSeqId()));

      box2.AddAlignmentSplit(
          std::make_unique<core::record::alignment_split::OtherRec>(
              box.GetPosition(), input.GetAlignmentSharedData().GetSeqId()));

      ret.back().AddAlignment(input.GetAlignmentSharedData().GetSeqId(),
                              core::record::AlignmentBox(box));

      ret.emplace_back(input.GetNumberOfTemplateSegments(), input.GetClassId(),
                       std::string(input.GetName()),
                       std::string(input.GetGroup()), input.GetFlags(),
                       !input.IsRead1First());
      ret.back().SetQvDepth(1);
      ret.back().GetSegments().emplace_back(input.GetSegments()[1]);
      ret.back().AddAlignment(input.GetAlignmentSharedData().GetSeqId(),
                              core::record::AlignmentBox(box2));
  }
  return ret;
}

core::record::Record DealignRecord(core::record::Record&& rec) {
  auto input = std::move(rec);
  core::record::Record ret(
      input.GetNumberOfTemplateSegments(), core::record::ClassType::kClassU,
      std::string(input.GetName()), std::string(input.GetGroup()),
      input.GetFlags(), input.IsRead1First());

  for (auto& i : input.GetSegments()) {
    ret.GetSegments().emplace_back(i);
  }
  ret.SetQvDepth(1);
  return ret;
}

bool IsECigarSupported(const std::string& ecigar) {
  // Splices not supported
  if (ecigar.find_first_of('*') != std::string::npos ||
      ecigar.find_first_of('/') != std::string::npos ||
      ecigar.find_first_of('%') != std::string::npos) {
    return false;
  }
  return true;
}

core::record::Record StripAdditionalAlignments(core::record::Record&& rec) {
  if (rec.GetClassId() == core::record::ClassType::kClassU ||
      rec.GetAlignments().size() < 2) {
    return std::move(rec);
  }

  auto input = std::move(rec);
  core::record::Record ret(input.GetNumberOfTemplateSegments(),
                           input.GetClassId(), std::string(input.GetName()),
                           std::string(input.GetGroup()), input.GetFlags(),
                           input.IsRead1First());

  for (auto& i : input.GetSegments()) {
    ret.GetSegments().emplace_back(i);
  }

  ret.AddAlignment(input.GetAlignmentSharedData().GetSeqId(),
                   core::record::AlignmentBox(input.GetAlignments().front()));
  ret.SetQvDepth(1);
  return ret;
}

std::pair<std::vector<core::record::Record>, CleanStatistics> CleanRecord(
    core::record::Record&& input) {
  std::vector<core::record::Record> ret;
  CleanStatistics stats;
  stats.additional_alignments +=
      input.GetAlignments().empty() ? 0 : input.GetAlignments().size() - 1;
  ret.emplace_back(StripAdditionalAlignments(std::move(input)));
  for (size_t i = 0; i < ret.size(); ++i) {
    if (ret[i].GetClassId() == core::record::ClassType::kClassHm) {
      ret[i] = DealignRecord(std::move(ret[i]));
      stats.hm_recs++;
      break;
    }
    if (ret[i].GetClassId() == core::record::ClassType::kClassU) {
      continue;
    }
    if (!IsECigarSupported(
            ret[i].GetAlignments().front().GetAlignment().GetECigar())) {
      ret[i] = DealignRecord(std::move(ret[i]));
      stats.splice_recs++;
      break;
    }
    for (const auto& s : ret[i].GetAlignments().front().GetAlignmentSplits()) {
      if (s->GetType() == core::record::AlignmentSplit::Type::kSameRec) {
        if (!IsECigarSupported(
                dynamic_cast<core::record::alignment_split::SameRec&>(*s)
                    .GetAlignment()
                    .GetECigar())) {
          ret[i] = DealignRecord(std::move(ret[i]));
          stats.splice_recs++;
          break;
        }
        // Splits with more than 32767 delta must be encoded in separate
        // records, which is not yet supported
        if (std::abs(dynamic_cast<core::record::alignment_split::SameRec&>(*s)
                         .GetDelta()) > 32767) {
          auto split = SplitRecord(std::move(ret[i]));
          ret[i] = std::move(split[0]);
          stats.distance++;
          for (size_t j = 1; j < split.size(); ++j) {
            ret.emplace_back(std::move(split[j]));
          }
          break;
        }
      }
    }
  }
  return std::make_pair(ret, stats);
}

// -----------------------------------------------------------------------------
void phase1_thread(sam_to_mgrec::SamReader& sam_reader, int& chunk_id,
                   const std::string& tmp_path, bool clean, std::mutex& lock,
                   CleanStatistics& stats) {
  while (true) {
    std::vector<core::record::Record> output_buffer;
    std::vector<std::vector<sam_to_mgrec::SamRecord>> queries;
    int ret = 0;
    int this_chunk = 0;

    CleanStatistics local_stats;

    // Load data

    {
      std::lock_guard guard(lock);
      this_chunk = chunk_id++;
      UTILS_LOG(util::Logger::Severity::INFO,
                "Processing chunk " + std::to_string(this_chunk) + "...");
      for (int i = 0; i < PHASE2_BUFFER_SIZE; ++i) {
        queries.emplace_back();
        ret = sam_reader.ReadSamQuery(queries.back());
        if (ret == EOF) {
          if (queries.back().empty()) {
            queries.pop_back();
          }
          break;
        }
        UTILS_DIE_IF(ret,
                     "Error reading sam query: " + std::string(strerror(ret)));
      }
    }

    // Convert data
    for (auto& q : queries) {
      sam_to_mgrec::SamRecordGroup buffer;
      for (auto& s : q) {
        buffer.AddRecord(std::move(s));
      }
      std::list<core::record::Record> records;
      buffer.convert(records);
      for (auto& m : records) {
        std::vector<core::record::Record> buf;
        if (clean) {
          auto [fst, snd] = CleanRecord(std::move(m));
          buf = std::move(fst);
          local_stats.splice_recs += snd.splice_recs;
          local_stats.distance += snd.distance;
          local_stats.additional_alignments += snd.additional_alignments;
          local_stats.hm_recs += snd.hm_recs;
        } else {
          buf.emplace_back(std::move(m));
        }
        for (auto& b : buf) {
          output_buffer.push_back(std::move(b));
        }
      }
      q.clear();
    }
    queries.clear();

    // Sort data

    std::sort(output_buffer.begin(), output_buffer.end(),
              sam_to_mgrec::compare);

    // Write data
    {
      std::string fpath =
          tmp_path + "/" + std::to_string(this_chunk) + PHASE1_EXT;
      std::ofstream output_file(fpath, std::ios::trunc | std::ios::binary);
      util::BitWriter bwriter(output_file);

      for (auto& r : output_buffer) {
        r.Write(bwriter);
      }

      bwriter.FlushBits();
    }

    {
      std::lock_guard guard(lock);
      stats.splice_recs += local_stats.splice_recs;
      stats.hm_recs += local_stats.hm_recs;
      stats.additional_alignments += local_stats.additional_alignments;
      stats.distance += local_stats.distance;
    }

    if (ret == EOF) {
      return;
    }
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::vector<std::pair<std::string, size_t>> Importer::sam_to_mgrec_phase1(
    const Config& options, int& chunk_id) {
  auto sam_reader = sam_to_mgrec::SamReader(options.input_file_);
  UTILS_DIE_IF(!sam_reader.IsReady() || !sam_reader.IsValid(),
               "Cannot open SAM file.");

  chunk_id = 0;
  std::mutex lock;
  std::vector<std::thread> threads;
  threads.reserve(options.num_threads_);
  CleanStatistics stats;
  for (uint32_t i = 0; i < options.num_threads_; ++i) {
    threads.emplace_back([&] {
      phase1_thread(sam_reader, chunk_id, options.tmp_dir_path_, options.clean_,
                    lock, stats);
    });
  }
  for (auto& t : threads) {
    t.join();
  }

  if (options.clean_) {
    if (stats.hm_recs) {
      UTILS_LOG(util::Logger::Severity::WARNING,
                "HM records dealigned: " + std::to_string(stats.hm_recs));
    }
    if (stats.distance) {
      UTILS_LOG(util::Logger::Severity::WARNING,
                "I records split because of large mapping distance: " +
                    std::to_string(stats.distance));
    }
    if (stats.additional_alignments) {
      UTILS_LOG(util::Logger::Severity::WARNING,
                "Additional alignments removed: " +
                    std::to_string(stats.additional_alignments));
    }
    if (stats.splice_recs) {
      UTILS_LOG(util::Logger::Severity::WARNING,
                "Records dealigned because of splices: " +
                    std::to_string(stats.splice_recs));
    }
  }

  refs_ = sam_reader.GetRefs();
  return refs_;
}

void Importer::setup_merge(const int num_chunks) {
  UTILS_LOG(util::Logger::Severity::INFO,
            "Merging " + std::to_string(num_chunks) + " chunks...");

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

  // std::unique_ptr<std::ostream> total_output;
  // std::ostream* out_stream = &std::cout;
  // genie::util::BitWriter total_output_writer(*out_stream);

  readers_.reserve(num_chunks);

  const std::string tmp_dir_path("/tmp");
  for (int i = 0; i < num_chunks; ++i) {
    readers_.emplace_back(std::make_unique<sam_to_mgrec::SubfileReader>(
        tmp_dir_path + "/" + std::to_string(i) + PHASE1_EXT));
    if (!readers_.back()->GetRecord()) {
      auto path = readers_.back()->GetPath();
      UTILS_LOG(util::Logger::Severity::INFO, path + " depleted");
      readers_.pop_back();
      std::remove(path.c_str());
    } else {
      reader_prio_.push(readers_.back().get());
    }
  }
}

bool Importer::PumpRetrieve(core::Classifier* classifier) {
  if (!phase1_complete_) {
    Config options;
    options.input_file_ = input_sam_file_;
    options.fasta_file_path_ = input_ref_file_;
    options.clean_ = true;
    sam_to_mgrec_phase1(options, nref_);
    phase1_complete_ = true;
    setup_merge(nref_);  // beginning of phase 2
  }
  // rest of phase 2
  util::Watch watch;
  core::record::Chunk chunk;
  size_t size_seq = 0;
  size_t size_qual = 0;
  size_t size_name = 0;
  std::optional<size_t> current_ref_id;
  bool eof = false;
  {
    if (!reader_prio_.empty()) {
      for (int i = 0; i < 100000; ++i) {
        auto* reader = reader_prio_.top();
        if (reader->GetRecord()->GetClassId() !=
            core::record::ClassType::kClassU) {
          if (current_ref_id.has_value()) {
            if (current_ref_id.value() !=
                sam_hdr_to_fasta_lut_[reader->GetRecord()
                                          ->GetAlignmentSharedData()
                                          .GetSeqId()]) {
              break;
            }
          } else {
            current_ref_id = sam_hdr_to_fasta_lut_
                [reader->GetRecord()->GetAlignmentSharedData().GetSeqId()];
          }
        }
        reader_prio_.pop();
        auto rec = reader->MoveRecord();
        rec.PatchRefId(
            sam_hdr_to_fasta_lut_[rec.GetAlignmentSharedData().GetSeqId()]);

        if (fix_ecigar(rec, refs_, refinf_)) {
          chunk.GetData().push_back(std::move(rec));
        } else {
          removed_unsupported_base_++;
        }

        if (reader->GetRecord()) {
          reader_prio_.push(reader);
        } else {
          auto path = reader->GetPath();
          UTILS_LOG(util::Logger::Severity::INFO, path + " depleted");
          for (auto it = readers_.begin(); it != readers_.end(); ++it) {
            if (it->get() == reader) {
              readers_.erase(it);
              break;
            }
          }
          std::remove(path.c_str());
          if (reader_prio_.empty()) {
            eof = true;
            break;
          }
        }
      }
    } else {
      eof = true;
    }
  }
  if (current_ref_id.has_value()) {
    chunk.SetRefId(current_ref_id.value());
  }
  chunk.GetStats().AddInteger("size-sam-seq", static_cast<int64_t>(size_seq));
  chunk.GetStats().AddInteger("size-sam-qual", static_cast<int64_t>(size_qual));
  chunk.GetStats().AddInteger("size-sam-name", static_cast<int64_t>(size_name));
  chunk.GetStats().AddInteger(
      "size-sam-total", static_cast<int64_t>(size_name + size_qual + size_seq));
  chunk.GetStats().AddDouble("time-sam-import", watch.Check());
  classifier->Add(std::move(chunk));
  return !eof;
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
