/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifdef _WIN32
#define NOMINMAX
#endif

#include "genie/read/spring/preprocess.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <utility>

#include "genie/core/record/record.h"
#include "genie/read/spring/params.h"
#include "genie/read/spring/util.h"
#include "genie/util/drain.h"
#include "genie/util/log.h"
#include "genie/util/ordered_section.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "Spring";

namespace genie::read::spring {

void OpenFile(std::ofstream& file, const std::string& path) {
  file.open(path, std::ios::binary);
  UTILS_DIE_IF(!file.is_open(), "Failed to open file: " + path);
}

// -----------------------------------------------------------------------------

void Preprocessor::Setup(const std::string& working_dir_p, const size_t num_thr,
                         const bool paired_end) {
  cp.preserve_id = true;
  cp.preserve_quality = true;
  cp.num_thr = static_cast<int>(num_thr);
  working_dir = working_dir_p;
  used = false;

  cp.paired_end = paired_end;
  cp.unaligned_reads_flag = false;
  cp.num_reads = 0;
  cp.num_reads_clean = {0u, 0u};
  cp.max_read_len = 0;
  preprocess_progress_printed = 0;
  cp.num_reads_per_block = kNumReadsPerBlock;
  cp.num_blocks = 0;

  do {
    temp_dir = std::filesystem::path(working_dir) / ("tmp." + RandomString(10));
  } while (std::filesystem::exists(temp_dir));

  UTILS_DIE_IF(!std::filesystem::create_directory(temp_dir),
               "Cannot create temporary directory: " + temp_dir);

  // File paths setup
  outfile_clean = {temp_dir + "/input_clean_1.dna",
                   temp_dir + "/input_clean_2.dna"};
  outfile_n = {temp_dir + "/input_N.dna", temp_dir + "/input_N.dna.2"};
  outfile_order_n = {temp_dir + "/read_order_N.bin",
                     temp_dir + "/read_order_N.bin.2"};
  outfile_id = temp_dir + "/id_1";
  outfile_quality = {temp_dir + "/quality_1", temp_dir + "/quality_2"};

  // Open file streams
  for (size_t j = 0; j < outfile_clean.size(); ++j) {
    if (j == 1 && !cp.paired_end) continue;
    OpenFile(f_out_clean[j], outfile_clean[j]);
    OpenFile(f_out_n[j], outfile_n[j]);
    OpenFile(f_out_order_n[j], outfile_order_n[j]);
    if (cp.preserve_quality) {  // NOLINT
      OpenFile(f_out_quality[j], outfile_quality[j]);
    }
  }
  if (cp.preserve_id) {  // NOLINT
    OpenFile(f_out_id, outfile_id);
  }
}

void Preprocessor::preprocess_segment(const core::record::Segment& seg,
                                      const size_t seg_index,
                                      const size_t rec_index) {
  const auto& sequence = seg.GetSequence();
  const auto& qualities = seg.GetQualities();

  // Validate read length
  UTILS_DIE_IF(sequence.size() > kMaxReadLen,
               "Global assembly maximum read length " +
                   std::to_string(kMaxReadLen) + " exceeded.");

  // Update maximum read length
  cp.max_read_len =
      std::max(cp.max_read_len, static_cast<uint32_t>(sequence.size()));

  if (sequence.find('N') != std::string::npos) {
    // Write sequences with 'N'
    WriteDnaNInBits(sequence, f_out_n[seg_index]);
    const auto pos_n = static_cast<uint32_t>(cp.num_reads + rec_index);
    f_out_order_n[seg_index].write(reinterpret_cast<const char*>(&pos_n),
                                   sizeof(uint32_t));
  } else {
    // Write clean sequences
    WriteDnaInBits(sequence, f_out_clean[seg_index]);
    cp.num_reads_clean[seg_index]++;
  }

  // Write quality scores if available
  if (!qualities.empty()) {
    f_out_quality[seg_index] << qualities.front() << "\n";
  }
}

// -----------------------------------------------------------------------------

void Preprocessor::preprocess_record(const core::record::Record& rec,
                                     const size_t record_index) {
  UTILS_DIE_IF(
      rec.GetSegments().size() != static_cast<std::size_t>(cp.paired_end + 1),
      "Number of segments differs between global assembly data chunks.");

  std::size_t seg_index = 0;

  // Process each segment
  for (const auto& seg : rec.GetSegments()) {
    preprocess_segment(seg, seg_index, record_index);
    seg_index++;
  }

  // Write read ID
  f_out_id << rec.GetName() << "\n";
}

void Preprocessor::Preprocess(core::record::Chunk&& t,
                              const util::Section& id) {
  core::record::Chunk data = std::move(t);

  [[maybe_unused]] util::OrderedSection lock_sec(&lock, id);
  used = true;

  // Update performance statistics
  stats.Add(data.GetStats());

  // Validate read limits
  UTILS_DIE_IF(
      data.GetData().front().GetNumberOfTemplateSegments() *
              (data.GetData().size() + cp.num_reads) >
          kMaxNumReads,
      "Too many reads in the input. Global assembly only supports up to " +
          std::to_string(kMaxNumReads) + " reads.");

  std::size_t rec_index = 0;

  // Process each record
  for (const auto& rec : data.GetData()) {
    preprocess_record(rec, rec_index);
    ++rec_index;
  }

  // Update read and block counters
  cp.num_reads += static_cast<uint32_t>(rec_index);
  cp.num_blocks++;

  constexpr size_t print_progress = 1000000;
  const std::string pair_end_str =
      cp.paired_end ? " (paired-end)" : "(single-end)";
  while (cp.num_reads - preprocess_progress_printed > print_progress) {
    GENIE_LOG(util::Logger::Severity::INFO,
              "---- Preprocessed reads " + pair_end_str + ": " +
                  std::to_string(preprocess_progress_printed));
    preprocess_progress_printed += print_progress;
  }

  // Validate processed reads
  UTILS_DIE_IF(cp.num_reads == 0, "No reads found.");
}

// -----------------------------------------------------------------------------

void Preprocessor::Finish(size_t id) {
  if (!used) {
    return;
  }
  util::Section sec{id, 0, true};
  [[maybe_unused]] util::OrderedSection lock_sec(&lock, sec);
  for (int j = 0; j < 2; j++) {
    if (j == 1 && !cp.paired_end) continue;
    f_out_clean[j].close();
    f_out_n[j].close();
    f_out_order_n[j].close();
    if (cp.preserve_quality) f_out_quality[j].close();
  }
  if (cp.preserve_id) f_out_id.close();

  if (cp.paired_end) {
    // merge input_N and input_order_N for the two files
    std::ofstream f_out_n_pe(outfile_n[0], std::ios::app | std::ios::binary);
    std::ifstream fin_n_pe(outfile_n[1], std::ios::binary);
    UTILS_DIE_IF(!fin_n_pe, "Cannot open file to read: " + outfile_n[1]);
    f_out_n_pe << fin_n_pe.rdbuf();
    f_out_n_pe.close();
    fin_n_pe.close();
    remove(outfile_n[1].c_str());
    std::ofstream f_out_order_n_pe(outfile_order_n[0],
                                   std::ios::app | std::ios::binary);
    std::ifstream fin_order_n(outfile_order_n[1], std::ios::binary);
    UTILS_DIE_IF(!fin_order_n,
                 "Cannot open file to read: " + outfile_order_n[1]);
    uint32_t num_n_file_2 = cp.num_reads - cp.num_reads_clean[1];
    uint32_t order_n;
    for (uint32_t i = 0; i < num_n_file_2; i++) {
      fin_order_n.read(reinterpret_cast<char*>(&order_n), sizeof(uint32_t));
      order_n += cp.num_reads;
      f_out_order_n_pe.write(reinterpret_cast<char*>(&order_n),
                             sizeof(uint32_t));
    }
    fin_order_n.close();
    f_out_order_n_pe.close();
    remove(outfile_order_n[1].c_str());
  }

  cp.num_reads = cp.paired_end ? cp.num_reads * 2 : cp.num_reads;

  GENIE_LOG(util::Logger::Severity::INFO,
            "Max Read length: " + std::to_string(cp.max_read_len));
  GENIE_LOG(util::Logger::Severity::INFO,
            "Total number of reads: " + std::to_string(cp.num_reads));
  GENIE_LOG(util::Logger::Severity::INFO,
            "Total number of reads without N: " +
                std::to_string(cp.num_reads_clean[0] + cp.num_reads_clean[1]));
}

// -----------------------------------------------------------------------------

Preprocessor::~Preprocessor() {
  if (!used) {
    for (int j = 0; j < 2; j++) {
      if (j == 1 && !cp.paired_end) continue;
      f_out_clean[j].close();
      f_out_n[j].close();
      f_out_order_n[j].close();
      if (cp.preserve_quality) f_out_quality[j].close();
    }
    if (cp.preserve_id) f_out_id.close();

    for (int j = 0; j < 2; j++) {
      if (j == 1 && !cp.paired_end) continue;
      std::filesystem::remove(outfile_clean[j]);
      std::filesystem::remove(outfile_n[j]);
      std::filesystem::remove(outfile_order_n[j]);
      if (cp.preserve_quality) std::filesystem::remove(outfile_quality[j]);
    }
    if (cp.preserve_id) std::filesystem::remove(outfile_id);

    std::filesystem::remove(temp_dir);
  }
}

// -----------------------------------------------------------------------------

void Preprocessor::Skip(const util::Section& id) {
  [[maybe_unused]] util::OrderedSection lock_sec(&lock, id);
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
