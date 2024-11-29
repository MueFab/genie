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
#include "genie/util/ordered_section.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::read::spring {

// -----------------------------------------------------------------------------

void Preprocessor::Setup(const std::string& working_dir_p, const size_t num_thr,
                         const bool paired_end) {
  cp.preserve_id = true;
  cp.preserve_quality = true;
  cp.num_thr = static_cast<int>(num_thr);
  working_dir = working_dir_p;
  used = false;

  lock.Reset();

  cp.paired_end = paired_end;
  cp.unaligned_reads_flag = false;
  cp.num_reads = 0;
  cp.num_reads_clean[0] = 0;
  cp.num_reads_clean[1] = 0;
  cp.max_read_len = 0;
  cp.num_reads_per_block = kNum_Reads_Per_Block;
  cp.num_blocks = 0;
  // generate random temp directory in the working directory

  while (true) {
    std::string random_str = "tmp." + RandomString(10);
    temp_dir = working_dir + "/" + random_str;
    if (!std::filesystem::exists(temp_dir)) break;
  }
  UTILS_DIE_IF(!std::filesystem::create_directory(temp_dir),
               "Cannot create temporary directory.");
  std::cerr << "Temporary directory: " << temp_dir << "\n";

  outfile_clean[0] = temp_dir + "/input_clean_1.dna";
  outfile_clean[1] = temp_dir + "/input_clean_2.dna";
  outfile_n[0] = temp_dir + "/input_N.dna";
  outfile_n[1] = temp_dir + "/input_N.dna.2";
  outfile_order_n[0] = temp_dir + "/read_order_N.bin";
  outfile_order_n[1] = temp_dir + "/read_order_N.bin.2";
  outfile_id = temp_dir + "/id_1";
  outfile_quality[0] = temp_dir + "/quality_1";
  outfile_quality[1] = temp_dir + "/quality_2";

  for (int j = 0; j < 2; j++) {
    if (j == 1 && !cp.paired_end) continue;
    f_out_clean[j].open(outfile_clean[j], std::ios::binary);
    f_out_n[j].open(outfile_n[j], std::ios::binary);
    f_out_order_n[j].open(outfile_order_n[j], std::ios::binary);
    if (cp.preserve_quality)                      // NOLINT
      f_out_quality[j].open(outfile_quality[j]);  // NOLINT
  }
  if (cp.preserve_id) f_out_id.open(outfile_id);  // NOLINT
}

// -----------------------------------------------------------------------------

void Preprocessor::Preprocess(core::record::Chunk&& t,
                              const util::Section& id) {
  core::record::Chunk data = std::move(t);

  [[maybe_unused]] util::OrderedSection lock_sec(&lock, id);
  used = true;
  stats.Add(data.GetStats());

  UTILS_DIE_IF(
      data.GetData().front().GetNumberOfTemplateSegments() *
              (data.GetData().size() + cp.num_reads) >
          kMax_Num_Reads,
      "Too many reads in the input. Global assembly only supports up to " +
          std::to_string(kMax_Num_Reads) + " reads.");

  size_t rec_index = 0;
  for (auto& rec : data.GetData()) {
    UTILS_DIE_IF(
        rec.GetSegments().size() != static_cast<size_t>(cp.paired_end + 1),
        "Number of segments differs between global assembly data chunks.");
    size_t seg_index = 0;
    for (auto& seq : rec.GetSegments()) {
      UTILS_DIE_IF(seq.GetSequence().size() > kMax_Read_Len,
                   "Global assembly maximum read length " +
                       std::to_string(kMax_Read_Len) + " exceeded.");
      cp.max_read_len = std::max(
          cp.max_read_len, static_cast<uint32_t>(seq.GetSequence().length()));
      if (seq.GetSequence().find('N') != std::string::npos) {
        WriteDnaNInBits(seq.GetSequence(), f_out_n[seg_index]);
        auto pos_n = static_cast<uint32_t>(cp.num_reads + rec_index);
        f_out_order_n[seg_index].write(reinterpret_cast<char*>(&pos_n),
                                       sizeof(uint32_t));
      } else {
        WriteDnaInBits(seq.GetSequence(), f_out_clean[seg_index]);
        cp.num_reads_clean[seg_index]++;
      }
      if (!seq.GetQualities().empty()) {
        f_out_quality[seg_index] << seq.GetQualities().front() << "\n";
      }
      ++seg_index;
    }
    f_out_id << rec.GetName() << "\n";
    ++rec_index;
  }
  cp.num_reads += static_cast<uint32_t>(rec_index);
  cp.num_blocks++;

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

  std::cerr << "Max Read length: " << cp.max_read_len << "\n";
  std::cerr << "Total number of reads: " << cp.num_reads << "\n";

  std::cerr << "Total number of reads without N: "
            << cp.num_reads_clean[0] + cp.num_reads_clean[1] << "\n";
}

// -----------------------------------------------------------------------------

core::stats::PerfStats& Preprocessor::GetStats() { return stats; }

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
