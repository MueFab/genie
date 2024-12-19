/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/sam/sam_to_mgrec/transcoder.h"

#include <algorithm>
#include <filesystem>  // NOLINT
#include <iostream>
#include <list>
#include <memory>
#include <mutex>  //NOLINT
#include <optional>
#include <queue>
#include <string>
#include <thread>  //NOLINT
#include <utility>
#include <vector>

#include "genie/core/record/alignment_split/other_rec.h"
#include "genie/core/record/alignment_split/same_rec.h"
#include "genie/format/sam/sam_to_mgrec/sam_group.h"
#include "genie/format/sam/sam_to_mgrec/sam_reader.h"
#include "genie/format/sam/sam_to_mgrec/sorter.h"
#include "genie/util/log.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "TranscoderSam";

namespace genie::format::sam::sam_to_mgrec {

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
    UTILS_DIE_IF(!fai_out, "Cannot open file to write: " + fai_name);
    fasta::FastaReader::index(fasta_in, fai_out);
  }
  if (!std::filesystem::exists(sha_name)) {
    std::ifstream fai_in(fai_name);
    UTILS_DIE_IF(!fai_in, "Cannot open file to read: " + fai_name);
    fasta::FaiFile fai_file(fai_in);
    std::ofstream sha_out(sha_name);
    fasta::FastaReader::hash(fai_file, fai_in, sha_out);
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

// -----------------------------------------------------------------------------
core::record::Record UnalignRecord(core::record::Record&& rec) {
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

// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
bool IsECigarSupported(const std::string& e_cigar) {
  // Splices not supported
  if (e_cigar.find_first_of('*') != std::string::npos ||
      e_cigar.find_first_of('/') != std::string::npos ||
      e_cigar.find_first_of('%') != std::string::npos) {
    return false;
  }
  return true;
}

// -----------------------------------------------------------------------------
struct CleanStatistics {
  size_t hm_recs{};
  size_t splice_recs{};
  size_t distance{};
  size_t additional_alignments{};
};

std::pair<std::vector<core::record::Record>, CleanStatistics> CleanRecord(
    core::record::Record&& input) {
  std::vector<core::record::Record> ret;
  CleanStatistics stats;
  stats.additional_alignments +=
      input.GetAlignments().empty() ? 0 : input.GetAlignments().size() - 1;
  ret.emplace_back(StripAdditionalAlignments(std::move(input)));
  for (size_t i = 0; i < ret.size(); ++i) {
    if (ret[i].GetClassId() == core::record::ClassType::kClassHm) {
      ret[i] = UnalignRecord(std::move(ret[i]));
      stats.hm_recs++;
      break;
    }
    if (ret[i].GetClassId() == core::record::ClassType::kClassU) {
      continue;
    }
    if (!IsECigarSupported(
            ret[i].GetAlignments().front().GetAlignment().GetECigar())) {
      ret[i] = UnalignRecord(std::move(ret[i]));
      stats.splice_recs++;
      break;
    }
    for (const auto& s : ret[i].GetAlignments().front().GetAlignmentSplits()) {
      if (s->GetType() == core::record::AlignmentSplit::Type::kSameRec) {
        if (!IsECigarSupported(
                dynamic_cast<core::record::alignment_split::SameRec&>(*s)
                    .GetAlignment()
                    .GetECigar())) {
          ret[i] = UnalignRecord(std::move(ret[i]));
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
void phase1_thread(SamReader& sam_reader, int& chunk_id,
                   const std::string& tmp_path, bool clean, std::mutex& lock,
                   CleanStatistics& stats) {
  while (true) {
    std::vector<core::record::Record> output_buffer;
    std::vector<std::vector<SamRecord>> queries;
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
      SamRecordGroup buffer;
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

    std::sort(output_buffer.begin(), output_buffer.end(), compare);

    // Write data
    {
      std::string file_path =
          tmp_path + "/" + std::to_string(this_chunk) + PHASE1_EXT;
      std::ofstream output_file(file_path, std::ios::trunc | std::ios::binary);
      util::BitWriter bit_writer(output_file);

      for (auto& r : output_buffer) {
        r.Write(bit_writer);
      }

      bit_writer.FlushBits();
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
std::vector<std::pair<std::string, size_t>> sam_to_mgrec_phase1(
    const Config& options, int& chunk_id) {
  auto sam_reader = SamReader(options.input_file_);
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
                "HM records unaligned: " + std::to_string(stats.hm_recs));
    }
    if (stats.splice_recs) {
      UTILS_LOG(util::Logger::Severity::WARNING,
                "Records unaligned because of splices: " +
                    std::to_string(stats.splice_recs));
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
  }

  auto refs = sam_reader.GetRefs();
  return refs;
}

// -----------------------------------------------------------------------------
std::string patch_e_cigar(const std::string& ref, const std::string& seq,
                          const std::string& e_cigar) {
  std::string fixed_cigar;
  auto class_checker = [&](const uint8_t cigar,                  // NOLINT
                           const std::pair<size_t, size_t>& bs,  // NOLINT
                           const std::pair<size_t, size_t>& rs) -> bool {
    const auto bs_ret = std::string_view(seq).substr(bs.first, bs.second);
    const auto rs_ret = std::string_view(ref).substr(rs.first, rs.second);
    const auto length = std::max(bs_ret.length(), rs_ret.length());
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

    UTILS_DIE_IF(cigar != '=', "Unknown e-cigar char.");

    size_t counter = 0;

    for (size_t i = 0; i < length; ++i) {
      if (*(bs_ret.begin() + i) != *(rs_ret.begin() + i)) {
        if (counter != 0) {
          fixed_cigar += std::to_string(counter) + "=";
          counter = 0;
        }
        fixed_cigar += std::string(1, *(bs_ret.begin() + i));

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
core::record::ClassType ClassifyECigar(const std::string& cigar) {
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
  return ret;
}

// -----------------------------------------------------------------------------
bool ValidateBases(const std::string& seq, const core::Alphabet& alphabet) {
  return std::all_of(seq.begin(), seq.end(), [&alphabet](const char& c) {
    return alphabet.IsIncluded(c);
  });
}

// -----------------------------------------------------------------------------
bool fix_e_cigar(core::record::Record& r,
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

    cigar = patch_e_cigar(ref_seq, seq, cigar);

    if (r.GetClassId() != core::record::ClassType::kClassHm) {
      r.SetClassType(ClassifyECigar(cigar));
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

        cigar = patch_e_cigar(ref_seq, seq, cigar);

        if (r.GetClassId() != core::record::ClassType::kClassHm) {
          r.SetClassType(std::max(r.GetClassId(), ClassifyECigar(cigar)));
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
std::string ECigar2Cigar(const std::string& e_cigar) {
  std::string cigar;
  cigar.reserve(e_cigar.size());
  size_t match_count = 0;
  std::string number_buffer;
  for (const auto& c : e_cigar) {
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
uint16_t ComputeSamFlags(const size_t s, const size_t a,
                         const core::record::Record& record) {
  uint16_t flags = 0;
  if (record.GetNumberOfTemplateSegments() > 1) {
    flags |= 0x1;
  }
  if (record.GetFlags() & core::gen_const::kFlagsProperPairMask) {
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
  if (a > 0) {
    flags |= 0x100;
  }
  if (record.GetFlags() & core::gen_const::kFlagsQualityFailMask) {
    flags |= 0x200;
  }
  if (record.GetFlags() & core::gen_const::kFlagsPcrDuplicateMask) {
    flags |= 0x400;
  }
  return flags;
}

// -----------------------------------------------------------------------------
void ProcessFirstMappedSegment(const size_t s, const size_t a,
                               const core::record::Record& record,
                               std::string& read_name, std::string& pos,
                               int64_t& template_length,
                               std::string& mapping_quality, std::string& cigar,
                               uint16_t& flags, const RefInfo& reference_info) {
  // This read is mapped, process mapping
  read_name = reference_info.IsValid()
                  ? reference_info.GetMgr()->Id2Ref(
                        record.GetAlignmentSharedData().GetSeqId())
                  : std::to_string(record.GetAlignmentSharedData().GetSeqId());
  if (s == 0) {
    // First segment is in primary alignment
    pos = std::to_string(record.GetAlignments()[a].GetPosition() + 1);
    template_length -=
        static_cast<int64_t>(record.GetAlignments()[a].GetPosition() + 1);
    mapping_quality =
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
    template_length -=
        static_cast<int64_t>(record.GetAlignments()[a].GetPosition() +
                             split.GetDelta() + 1 + MappedLength(cigar));
    mapping_quality =
        split.GetAlignment().GetMappingScores().empty()
            ? "0"
            : std::to_string(split.GetAlignment().GetMappingScores().front());
    if (split.GetAlignment().GetRComp()) {
      flags |= 0x10;
    }
  }
}

// -----------------------------------------------------------------------------
void ProcessSecondMappedSegment(const size_t s,
                                const core::record::Record& record,
                                int64_t& template_length, uint16_t& flags,
                                std::string& position_next,
                                std::string& reference_next,
                                const RefInfo& reference_info) {
  // According to SAM standard, primary alignments only
  const auto split_type =
      record.GetClassId() == core::record::ClassType::kClassHm
          ? core::record::AlignmentSplit::Type::kUnpaired
          : record.GetAlignments()[0].GetAlignmentSplits().front()->GetType();
  if ((s == 1 && split_type == core::record::AlignmentSplit::Type::kSameRec) ||
      split_type == core::record::AlignmentSplit::Type::kUnpaired) {
    // Paired read is first read
    position_next = std::to_string(record.GetAlignments()[0].GetPosition() + 1);
    template_length +=
        static_cast<int64_t>(record.GetAlignments()[0].GetPosition() + 1);
    reference_next =
        reference_info.IsValid()
            ? reference_info.GetMgr()->Id2Ref(
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
      position_next = std::to_string(record.GetAlignments()[0].GetPosition() +
                                     split.GetDelta() + 1);
      template_length += static_cast<int64_t>(
          record.GetAlignments()[0].GetPosition() + split.GetDelta() + 1 +
          MappedLength(ECigar2Cigar(split.GetAlignment().GetECigar())));

      reference_next =
          reference_info.IsValid()
              ? reference_info.GetMgr()->Id2Ref(
                    record.GetAlignmentSharedData().GetSeqId())
              : std::to_string(record.GetAlignmentSharedData().GetSeqId());
      if (split.GetAlignment().GetRComp()) {
        flags |= 0x20;
      }
    } else if (split_type == core::record::AlignmentSplit::Type::kOtherRec) {
      const auto& split =
          dynamic_cast<const core::record::alignment_split::OtherRec&>(
              *record.GetAlignments()[0].GetAlignmentSplits().front().get());
      reference_next = reference_info.IsValid()
                           ? reference_info.GetMgr()->Id2Ref(split.GetNextSeq())
                           : std::to_string(split.GetNextSeq());
      position_next = std::to_string(split.GetNextPos() + 1);
      template_length = 0;  // Not available without reading second record
    } else {
      template_length = 0;
    }
  }
}

// -----------------------------------------------------------------------------
bool compare(const core::record::Record& r1, const core::record::Record& r2) {
  if (r1.GetAlignments().empty()) {
    return false;
  }
  if (r2.GetAlignments().empty()) {
    return true;
  }
  if (r1.GetAlignmentSharedData().GetSeqId() !=
      r2.GetAlignmentSharedData().GetSeqId()) {
    return r1.GetAlignmentSharedData().GetSeqId() <
           r2.GetAlignmentSharedData().GetSeqId();
  }
  return r1.GetAlignments().front().GetPosition() <
         r2.GetAlignments().front().GetPosition();
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::sam::sam_to_mgrec

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
