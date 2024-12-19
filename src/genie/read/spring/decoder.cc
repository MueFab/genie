/**
 * Copyright 2018-2024 The Genie Authors.
 * @file decoder.cc
 *
 * @brief Implementation of the Spring Decoder module for processing sequencing
 * data.
 *
 * This file contains the implementation of the decoder for the Spring
 * framework, including functions for reading, decoding, and managing paired and
 * unpaired sequencing records. The module handles complex pairing logic,
 * mismatch resolution, and temporary file management for efficient processing.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifdef _WIN32
#define NOMINMAX
#endif

#include "genie/read/spring/decoder.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "genie/read/spring/params.h"
#include "genie/read/spring/util.h"
#include "genie/util/log.h"
#include "genie/util/stop_watch.h"
#include "kwaymergesort/kwaymergesort.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "Spring";
namespace genie::read::spring {

// -----------------------------------------------------------------------------
void DecodeStreams(core::AccessUnit& au, bool paired_end, bool combine_pairs,
                   std::array<std::vector<Record>, 2>& matched_records,
                   std::array<std::vector<Record>, 2>& unmatched_records,
                   std::vector<std::string>& names,
                   std::vector<std::string>& qvs) {
  /*
   * return values are matched_records[2] (for pairs that are matched),
   * unmatched_records[2] (for pairs that are unmatched).
   * For single end case, only matched_records[0] is
   * populated, for paired end reads with combine_pairs false, only
   * matched_records[0] and matched_records[1] are populated (unmatched
   * records also put in these). For paired end with combine_pairs true,
   * matched_records arrays contain the matched records and have equal Size,
   * unmatched_records have the records that don't have pair within the same
   * AU. Unmatched pairs are combined based on matching read ids. For reads
   * whose pair is in same AU, we perform matching here itself (using a hash
   * table to map read name to the index and then using this to match the
   * pairs). Otherwise, we put them in unmatched_records and the matching is
   * performed across AUs later (see Decoder::flushIn).
   */
  std::vector<Record> unmatched_same_au[2];
  std::unordered_map<std::string, uint32_t> pos_in_unmatched_same_au_0;
  // hash table mapping the read name to its position in unmatched_same_au[0]
  // used to reorder the unmatched_same_au[1] in the same order
  size_t qv_pos = 0;
  size_t name_pos = 0;
  Record cur_record;
  std::string ref_buf;

  // some state variables
  uint64_t abs_pos = 0;

  while (!au.Get(core::gen_sub::kRtype).end()) {
    if (auto rtype = au.Get(core::gen_sub::kRtype).Pull(); rtype == 5) {
      // put in refBuf
      auto read_length =
          static_cast<uint32_t>(au.Get(core::gen_sub::kReadLength).Pull()) +
          1;  // read_length
      for (uint32_t i = 0; i < read_length; i++) {
        ref_buf.push_back(
            GetAlphabetProperties(core::AlphabetId::kAcgtn)
                .lut[au.Get(core::gen_sub::kUnalignedReads).Pull()]);
        // unaligned reads
      }
    } else {
      // rtype can be 1 (P) or 3 (M)
      uint8_t number_of_record_segments;
      uint16_t delta = 0;
      bool read_1_first = true;
      bool same_au_flag = false;  // when combine_pairs true and pair is split:
                                  // flag to indicate if mate in same AU
      if (paired_end) {
        uint64_t pairing_decoding_case =
            au.Get(core::gen_sub::kPairDecodingCase).Pull();
        // note that we don't Decode/use mateAUid and mateRecordIndex in
        // this decoder the compressor still stores this information
        // which can be a bit redundant given it is stored at two places
        // and also the ids are stored at both places. one way to
        // resolve this could be to use the R1_unpaired and R2_unpaired
        // decoding cases, but we don't use those as they are not
        // semantically correct
        uint16_t data_pair;
        switch (pairing_decoding_case) {
          case 0:
            data_pair = static_cast<uint16_t>(
                au.Get(core::gen_sub::kPairSameRec).Pull());
            read_1_first = !(data_pair & 1);
            delta = data_pair >> 1;
            number_of_record_segments = 2;
            break;
          case 1:
          case 3:
            number_of_record_segments = 1;
            read_1_first = false;
            if (combine_pairs) {
              if (pairing_decoding_case == 1) {
                same_au_flag = true;
              } else if (pairing_decoding_case == 3) {  // NOLINT
                same_au_flag = false;
              }
            }
            break;
          case 2:
          case 4:
            number_of_record_segments = 1;
            read_1_first = true;
            if (combine_pairs) {
              if (pairing_decoding_case == 2)
                same_au_flag = true;
              else if (pairing_decoding_case == 4)  // NOLINT
                same_au_flag = false;
            }
            break;
          default:
            throw std::runtime_error(
                "Unsupported pair decoding case encountered");
        }
      } else {
        number_of_record_segments = 1;
      }
      uint32_t read_length[2];
      for (int i = 0; i < number_of_record_segments; i++)
        read_length[i] =
            static_cast<uint32_t>(au.Get(core::gen_sub::kReadLength).Pull()) +
            1;  // read length
      auto pos = static_cast<uint32_t>(
          au.Get(core::gen_sub::kPositionFirst).Pull());  // pos

      abs_pos += pos;
      std::string cur_read[2];
      cur_read[0] = ref_buf.substr(abs_pos, read_length[0]);
      if (number_of_record_segments == 2) {
        uint64_t pair_abs_pos = abs_pos + delta;
        cur_read[1] = ref_buf.substr(pair_abs_pos, read_length[1]);
      }
      bool reverse_comp[2];
      for (int i = 0; i < number_of_record_segments; i++)
        reverse_comp[i] = au.Get(core::gen_sub::kReverseComplement).Pull();
      // reverse_complement
      if (rtype == 3) {
        // type M
        for (int i = 0; i < number_of_record_segments; i++) {
          uint32_t abs_mismatch_pos = 0;
          while (true) {
            if (bool mismatch_pos_flag = static_cast<bool>(
                    au.Get(core::gen_sub::kMismatchPosTerminator).Pull());
                mismatch_pos_flag == 1)
              break;
            auto mismatch_pos = static_cast<uint32_t>(
                au.Get(core::gen_sub::kMismatchPosDelta).Pull());
            abs_mismatch_pos += mismatch_pos;
            if (auto mismatch_type_0 = static_cast<uint32_t>(
                    au.Get(core::gen_sub::kMismatchType).Pull());
                mismatch_type_0 != 0)  // i.e., not substitution
              throw std::runtime_error("Non-zero mismatch type encountered.");
            uint64_t mismatch_type_1 = 0;
            if (au.Get(core::gen_sub::kMismatchTypeSubstBase)
                    .GetMismatchDecoder()) {
              mismatch_type_1 =
                  au.Get(core::gen_sub::kMismatchTypeSubstBase)
                          .GetMismatchDecoder()
                          ->DataLeft()
                      ? au.Get(core::gen_sub::kMismatchTypeSubstBase)
                            .GetMismatchDecoder()
                            ->DecodeMismatch(
                                GetAlphabetProperties(core::AlphabetId::kAcgtn)
                                    .inverse_lut[cur_read[i][abs_mismatch_pos]])
                      : GetAlphabetProperties(core::AlphabetId::kAcgtn)
                            .inverse_lut['N'];
            } else {
              mismatch_type_1 =
                  au.Get(core::gen_sub::kMismatchTypeSubstBase).Pull();
            }
            cur_read[i][abs_mismatch_pos] =
                GetAlphabetProperties(core::AlphabetId::kAcgtn)
                    .lut[mismatch_type_1];
            abs_mismatch_pos++;
          }
        }
      }
      // finally, reverse complement if needed
      for (int i = 0; i < number_of_record_segments; i++) {
        if (reverse_comp[i] == 1)
          cur_read[i] =
              ReverseComplement(cur_read[i], static_cast<int>(read_length[i]));
      }
      bool first_read_flag = true;
      for (int i = 0; i < number_of_record_segments; i++) {
        if (paired_end) {
          if ((i == 0 && read_1_first) || (i == 1 && !read_1_first)) {
            first_read_flag = true;
          } else {
            first_read_flag = false;
          }
        }
        cur_record.seq = cur_read[i];
        if (!names.empty()) {
          cur_record.name = names[name_pos];
        }
        if (!qvs.empty()) {
          cur_record.qv = std::move(qvs[qv_pos++]);
        }
        if (!paired_end) {
          matched_records[0].push_back(cur_record);
        } else {
          if (number_of_record_segments == 2) {
            matched_records[first_read_flag ? 0 : 1].push_back(cur_record);
          } else {
            if (same_au_flag) {
              unmatched_same_au[first_read_flag ? 0 : 1].push_back(cur_record);
              if (first_read_flag)
                pos_in_unmatched_same_au_0[cur_record.name] =
                    static_cast<unsigned int>(unmatched_same_au[0].size() - 1);
            } else {
              unmatched_records[first_read_flag ? 0 : 1].push_back(cur_record);
            }
          }
        }
      }
      name_pos++;
    }
  }
  // if combine_pairs is true, reorder reads in unmatched_same_au so that
  // pairs match, and push this to matched_records arrays
  if (paired_end && combine_pairs) {
    size_t size_unmatched = unmatched_same_au[0].size();
    if (size_unmatched != unmatched_same_au[1].size() ||
        size_unmatched != pos_in_unmatched_same_au_0.size())
      UTILS_DIE("Sizes of unmatched same AU vectors don't match.");
    if (size_unmatched > 0) {
      matched_records[0].insert(matched_records[0].end(),
                                unmatched_same_au[0].begin(),
                                unmatched_same_au[0].end());
      std::vector<std::pair<uint32_t, uint32_t>> record_index_for_sorting(
          size_unmatched);
      for (size_t i = 0; i < size_unmatched; i++)
        record_index_for_sorting[i] = std::make_pair(
            pos_in_unmatched_same_au_0[unmatched_same_au[1][i].name],
            static_cast<uint32_t>(i));
      std::sort(record_index_for_sorting.begin(),
                record_index_for_sorting.end(),
                [](const std::pair<uint32_t, uint32_t>& a,
                   const std::pair<uint32_t, uint32_t>& b) {
                  return a.first < b.first;
                });
      for (size_t i = 0; i < size_unmatched; i++)
        matched_records[1].push_back(
            unmatched_same_au[1][record_index_for_sorting[i].second]);
    }
  }
}

// -----------------------------------------------------------------------------
Decoder::Decoder(const std::string& working_dir, const bool comb_p,
                 const bool paired_end)
    : combine_pairs_(comb_p), unmatched_record_index_{} {
  basedir_ = working_dir;

  while (true) {
    const std::string random_str = "tmp." + RandomString(10);
    basedir_ = working_dir;
    basedir_ += "/";
    basedir_ += random_str;
    basedir_ += '/';
    if (!std::filesystem::exists(basedir_)) break;
  }
  UTILS_DIE_IF(!std::filesystem::create_directory(basedir_),
               "Cannot create temporary directory.");
  UTILS_LOG(util::Logger::Severity::INFO, "Temporary directory: " + basedir_);
  cp_.unaligned_reads_flag = false;
  cp_.paired_end = paired_end;
  // temporary files for combine pairs case
  file_unmatched_fastq1_ = basedir_ + "/unmatched_1.fastq";
  file_unmatched_fastq2_ = basedir_ + "/unmatched_2.fastq";
  file_unmatched_read_names_1_ = basedir_ + "/unmatched_read_names_1.txt";
  file_unmatched_read_names_2_ = basedir_ + "/unmatched_read_names_2.txt";
  if (cp_.paired_end) {
    if (combine_pairs_ && !cp_.unaligned_reads_flag) {
      file_out_unmatched1_.open(file_unmatched_fastq1_);
      file_out_unmatched2_.open(file_unmatched_fastq2_);
      file_out_unmatched_read_names_1_.open(file_unmatched_read_names_1_);
      file_out_unmatched_read_names_2_.open(file_unmatched_read_names_2_);
      unmatched_record_index_[0] = unmatched_record_index_[1] = 0;
    }
  }
}

// -----------------------------------------------------------------------------
void Decoder::FlowIn(core::AccessUnit&& t, const util::Section& id) {
  core::record::Chunk chunk;
  core::AccessUnit au = EntropyCodeAu(std::move(t), true);
  util::Watch watch;
  std::array<std::vector<Record>, 2> matched_records;
  std::array<std::vector<Record>, 2> unmatched_records;

  std::vector<std::string> e_cigars;
  std::vector<uint64_t> positions;
  while (!au.Get(core::gen_sub::kRtype).end()) {
    if (au.Get(core::gen_sub::kRtype).Pull() != 5) {
      e_cigars.emplace_back(
          std::to_string(au.Get(core::gen_sub::kReadLength).Pull() + 1) + "+");
      positions.emplace_back(std::numeric_limits<uint64_t>::max());
      if (cp_.paired_end) {
        if (au.Get(core::gen_sub::kPairDecodingCase).Pull() == 0) {
          e_cigars.emplace_back(
              std::to_string(au.Get(core::gen_sub::kReadLength).Pull() + 1) +
              "+");
          positions.emplace_back(std::numeric_limits<uint64_t>::max());
        }
      }
    } else {
      au.Get(core::gen_sub::kReadLength).Pull();
    }
  }
  au.Get(core::gen_sub::kPairDecodingCase).SetPosition(0);
  au.Get(core::gen_sub::kRtype).SetPosition(0);
  au.Get(core::gen_sub::kReadLength).SetPosition(0);

  watch.Pause();
  auto names = namecoder_->Process(au.Get(core::GenDesc::kReadName));

  // if read names is empty but combine_pairs is set to true, raise error
  if (au.GetNumReads() > 0 && std::get<0>(names).empty() && combine_pairs_)
    UTILS_DIE(
        "combinePairsFlag cannot be set to true when read names are not "
        "present for all records.");

  au.GetStats().Add(std::get<1>(names));
  auto qvs = qvcoder_->Process(
      au.GetParameters().GetQvConfig(core::record::ClassType::kClassU),
      e_cigars, positions, au.Get(core::GenDesc::kQv));
  au.GetStats().Add(std::get<1>(qvs));
  watch.Resume();

  DecodeStreams(au, cp_.paired_end, combine_pairs_, matched_records,
                unmatched_records, std::get<0>(names), std::get<0>(qvs));

  for (size_t i = 0; i < matched_records[0].size(); ++i) {
    chunk.GetData().emplace_back(
        cp_.paired_end ? static_cast<uint8_t>(2) : static_cast<uint8_t>(1),
        core::record::ClassType::kClassU, std::move(matched_records[0][i].name),
        "", static_cast<uint8_t>(0));
    core::record::Segment seg(std::move(matched_records[0][i].seq));
    if (!matched_records[0][i].qv.empty()) {
      seg.AddQualities(std::move(matched_records[0][i].qv));
    }
    chunk.GetData().back().AddSegment(std::move(seg));
    if (cp_.paired_end) {
      core::record::Segment seg2(std::move(matched_records[1][i].seq));
      if (!matched_records[1][i].qv.empty()) {
        seg2.AddQualities(std::move(matched_records[1][i].qv));
      }
      chunk.GetData().back().AddSegment(std::move(seg2));
    }
  }
  // now put unmatched reads to chunks if combine_pairs is false
  if (!combine_pairs_) {
    for (auto& [name, seq, qv] : unmatched_records[0]) {
      chunk.GetData().emplace_back(
          cp_.paired_end ? static_cast<uint8_t>(2) : static_cast<uint8_t>(1),
          core::record::ClassType::kClassU, std::move(name), "",
          static_cast<uint8_t>(0), true);
      // last parameter is read_1_first
      core::record::Segment seg(std::move(seq));
      if (!qv.empty()) {
        seg.AddQualities(std::move(qv));
      }
      chunk.GetData().back().AddSegment(std::move(seg));
    }
    for (auto& [name, seq, qv] : unmatched_records[1]) {
      chunk.GetData().emplace_back(
          cp_.paired_end ? static_cast<uint8_t>(2) : static_cast<uint8_t>(1),
          core::record::ClassType::kClassU, std::move(name), "",
          static_cast<uint8_t>(0), false);
      // last parameter is read_1_first
      core::record::Segment seg(std::move(seq));
      if (!qv.empty()) {
        seg.AddQualities(std::move(qv));
      }
      chunk.GetData().back().AddSegment(std::move(seg));
    }
  }
  {
    [[maybe_unused]] util::OrderedSection sec(&lock_, id);
    if (cp_.paired_end && combine_pairs_) {
      for (int j = 0; j < 2; j++) {
        std::ostream& tmp_out =
            j == 0 ? file_out_unmatched1_ : file_out_unmatched2_;
        std::ostream& tmp_out_read_names =
            j == 0 ? file_out_unmatched_read_names_1_
                   : file_out_unmatched_read_names_2_;
        for (auto& [name, seq, qv] : unmatched_records[j]) {
          tmp_out << name << "\n";
          tmp_out << seq << "\n";
          tmp_out << qv << "\n";
          tmp_out_read_names << name << "\t" << unmatched_record_index_[j]++
                             << "\n";
        }
      }
    }
  }

  size_t chunk_size = chunk.GetData().size();
  if (cp_.paired_end) {
    chunk_size *= 2;
  }
  chunk.SetStats(std::move(au.GetStats()));
  chunk.GetStats().AddDouble("time-spring-decoder", watch.Check());
  au.Clear();
  FlowOut(std::move(chunk), util::Section{id.start, chunk_size, true});
  if (id.length - chunk_size > 0) {
    SkipOut(util::Section{id.start + chunk_size, id.length - chunk_size, true});
  }
}

// -----------------------------------------------------------------------------
void Decoder::ReadRec(std::ifstream& i, Record& r) {
  UTILS_DIE_IF(!std::getline(i, r.name), "Error reading tmp file");
  UTILS_DIE_IF(!std::getline(i, r.seq), "Error reading tmp file");
  UTILS_DIE_IF(!std::getline(i, r.qv), "Error reading tmp file");
}

// -----------------------------------------------------------------------------
void Decoder::Add(core::record::Chunk& chunk, core::record::Record&& r,
                  uint64_t& pos) {
  chunk.GetData().push_back(std::move(r));
  if (constexpr size_t chunk_size = 100000;
      chunk.GetData().size() >= chunk_size) {
    const size_t size = chunk.GetData().size() * 2;
    FlowOut(std::move(chunk), {pos, size, true});
    pos += size;
  }
}

// -----------------------------------------------------------------------------
void Decoder::FlushIn(uint64_t& pos) {
  core::record::Chunk chunk;

  // now reorder the remaining unmatched reads so that they are paired and
  // then write them to file.

  // Strategy: we wrote all unmatched records for read 1 and read 2
  // to files. We also wrote the read names along with their
  // position in the unmatched reads to separate files.
  // First step will be to sort the read names for unmatched_1 and unmatched_2
  // - that will provide us with a mapping between the records in
  // unmatched_1 and unmatched_2. Then we write the unmatched_1 in their
  // original order, and unmatched_2 in the corresponding sorted order. This
  // last part is done in chunks whose Size is determined by
  // BIN_SIZE_COMBINE_PAIRS, to put a limit on the memory usage for this step.
  // Higher BIN_SIZE_COMBINE_PAIRS can lead to speed up due to fewer disk
  // accesses.

  double time_name_sorting = 0;
  double time_record_sorting = 0;
  if (!cp_.unaligned_reads_flag && cp_.paired_end && combine_pairs_) {
    UTILS_LOG(util::Logger::Severity::INFO, "Order unmatched decoded reads...");
    file_out_unmatched1_.close();
    file_out_unmatched2_.close();
    file_out_unmatched_read_names_1_.close();
    file_out_unmatched_read_names_2_.close();

    // verify that the two unmatched reads have same number of reads
    if (unmatched_record_index_[0] != unmatched_record_index_[1])
      UTILS_DIE("Sizes of unmatched reads across AUs don't match.");
    uint32_t size_unmatched = unmatched_record_index_[0];
    UTILS_LOG(util::Logger::Severity::INFO,
              "Pairs to match: " + std::to_string(size_unmatched));

    std::string file_unmatched_read_names_1_sorted =
        file_unmatched_read_names_1_ + ".sorted";
    std::string file_unmatched_read_names_2_sorted =
        file_unmatched_read_names_2_ + ".sorted";

    if (size_unmatched > 0) {
      UTILS_LOG(util::Logger::Severity::INFO,
                "Processing " + std::to_string(size_unmatched) +
                    " unmatched reads...");
      UTILS_LOG(util::Logger::Severity::INFO, "---- Sorting read names");
      util::Watch sort_watch;
      // first sort f_out_unmatched_read_n_ames_* using disk-based merge
      // sort from fork of https://github.com/arq5x/kway-mergesort
      size_t max_buffer_size = 2000000000;  // roughly 2 GB
      std::ofstream f_out_unmatched_read_names_1_sorted(
          file_unmatched_read_names_1_sorted);
      std::ofstream f_out_unmatched_read_names_2_sorted(
          file_unmatched_read_names_2_sorted);
      std::make_unique<kwaymergesort::KwayMergeSort>(
          file_unmatched_read_names_1_, &f_out_unmatched_read_names_1_sorted,
          static_cast<int>(max_buffer_size), false, basedir_)
          ->Sort();
      f_out_unmatched_read_names_1_sorted.close();
      std::make_unique<kwaymergesort::KwayMergeSort>(
          file_unmatched_read_names_2_, &f_out_unmatched_read_names_2_sorted,
          static_cast<int>(max_buffer_size), false, basedir_)
          ->Sort();
      f_out_unmatched_read_names_2_sorted.close();

      time_name_sorting = sort_watch.Check();

      util::Watch reorder_unmatched_start;
      // now build an index that tells for read `i` in file_2, where it's
      // pair in file_1 lies.
      std::vector<uint32_t> index_file_1(size_unmatched);
      // map sorted position to unsorted
      std::vector<uint32_t> reverse_index_file_2(size_unmatched);
      // map unsorted to sorted (for now, reused later to map unsorted in
      // file 2 to unsorted in file 1)

      std::ifstream fin_unmatched_read_names_1_sorted(
          file_unmatched_read_names_1_sorted);
      UTILS_DIE_IF(
          !fin_unmatched_read_names_1_sorted,
          "Cannot open file to read: " + file_unmatched_read_names_1_sorted);
      std::string line;
      uint32_t num_lines = 0;
      while (std::getline(fin_unmatched_read_names_1_sorted, line)) {
        auto pos_tab = line.find_last_of('\t');  // find last tab
        index_file_1[num_lines++] =
            static_cast<uint32_t>(std::stoull(line.substr(pos_tab + 1)));
      }
      if (num_lines != size_unmatched)
        UTILS_DIE(
            "Sizes of unmatched reads across AUs don't match "
            "(read_names_1_sorted).");
      fin_unmatched_read_names_1_sorted.close();

      std::ifstream fin_unmatched_read_names_2_sorted(
          file_unmatched_read_names_2_sorted);
      UTILS_DIE_IF(
          !fin_unmatched_read_names_2_sorted,
          "Cannot open file to read: " + file_unmatched_read_names_2_sorted);
      num_lines = 0;
      while (std::getline(fin_unmatched_read_names_2_sorted, line)) {
        auto pos_tab = line.find_last_of('\t');  // find last tab
        reverse_index_file_2[std::stoull(line.substr(pos_tab + 1))] =
            num_lines++;
      }
      if (num_lines != size_unmatched)
        UTILS_DIE(
            "Sizes of unmatched reads across AUs don't match "
            "(read_names_2_sorted).");
      fin_unmatched_read_names_2_sorted.close();

      // now build index from file 2 to file 1
      for (uint32_t i = 0; i < size_unmatched; i++) {
        reverse_index_file_2[i] = index_file_1[reverse_index_file_2[i]];
      }

      index_file_1.clear();

      // we will write the reads in file 1 in the same order as in
      // file_unmatched_fastq1
      std::ifstream fin_unmatched1(file_unmatched_fastq1_);
      UTILS_DIE_IF(!fin_unmatched1,
                   "Cannot open file to read: " + file_unmatched_fastq1_);
      // now reorder the unmatched records in file 2, by picking them in
      // chunks
      uint32_t bin_size = std::min(kBinSizeCombinePairs, size_unmatched);
      std::vector<Record> records_bin(bin_size);
      Record tmp_fastq_record;
      for (uint32_t i = 0; i <= size_unmatched / bin_size; i++) {
        uint32_t num_records_bin = bin_size;
        if (i == size_unmatched / bin_size)
          num_records_bin = size_unmatched % bin_size;
        if (num_records_bin == 0) break;  // we are done
        std::ifstream fin_unmatched2(file_unmatched_fastq2_);
        UTILS_DIE_IF(!fin_unmatched2,
                     "Cannot open file to read: " + file_unmatched_fastq2_);
        uint32_t bin_start = i * bin_size;
        for (uint32_t j = 0; j < size_unmatched; j++) {
          if (reverse_index_file_2[j] >= bin_start &&
              reverse_index_file_2[j] < bin_start + num_records_bin) {
            ReadRec(fin_unmatched2,
                    records_bin[reverse_index_file_2[j] - bin_start]);
          } else {
            ReadRec(fin_unmatched2, tmp_fastq_record);
          }
        }
        fin_unmatched2.close();
        for (uint32_t j = 0; j < num_records_bin; j++) {
          ReadRec(fin_unmatched1, tmp_fastq_record);

          core::record::Record r(2, core::record::ClassType::kClassU,
                                 std::move(tmp_fastq_record.name), "", 0);
          core::record::Segment s1(std::move(tmp_fastq_record.seq));
          if (!tmp_fastq_record.qv.empty()) {
            s1.AddQualities(std::move(tmp_fastq_record.qv));
          }

          core::record::Segment s2(std::move(records_bin[j].seq));
          if (!records_bin[j].qv.empty()) {
            s2.AddQualities(std::move(records_bin[j].qv));
          }

          r.AddSegment(std::move(s1));
          r.AddSegment(std::move(s2));

          Add(chunk, std::move(r), pos);
        }
      }
      std::filesystem::remove(file_unmatched_read_names_1_sorted);
      std::filesystem::remove(file_unmatched_read_names_2_sorted);
      time_record_sorting = reorder_unmatched_start.Check();
    }
    std::filesystem::remove(file_unmatched_fastq1_);
    std::filesystem::remove(file_unmatched_fastq2_);
    std::filesystem::remove(file_unmatched_read_names_1_);
    std::filesystem::remove(file_unmatched_read_names_2_);
  }

  if (size_t size = chunk.GetData().size() * 2) {
    chunk.GetStats().AddDouble("time-spring-unmatched-name-sort",
                               time_name_sorting);
    chunk.GetStats().AddDouble("time-spring-unmatched-record-sort",
                               time_record_sorting);
    FlowOut(std::move(chunk), {pos, size, true});
    pos += size;
  }

  std::filesystem::remove(basedir_);

  FlushOut(pos);
}

// -----------------------------------------------------------------------------
void Decoder::SkipIn(const util::Section& id) {
  {
    util::OrderedSection sec(&lock_, id);  // NOLINT
  }  // NOLINT
  SkipOut(id);
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
