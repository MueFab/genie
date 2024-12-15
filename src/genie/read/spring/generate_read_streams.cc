/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/spring/generate_read_streams.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/access_unit.h"
#include "genie/core/parameter/parameter_set.h"
#include "genie/core/read_encoder.h"
#include "genie/read/spring/dynamic_scheduler.h"
#include "genie/read/spring/util.h"
#include "genie/util/log.h"

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "Spring";

namespace genie::read::spring {

// -----------------------------------------------------------------------------
struct SeData {
  CompressionParams cp{};
  std::vector<bool> flag_arr;
  std::vector<uint64_t> pos_arr;
  std::vector<uint16_t> read_length_arr;
  std::vector<char> noise_arr;
  std::vector<uint16_t> noise_pos_arr;
  std::vector<uint64_t> pos_in_noise_arr;
  std::vector<uint16_t> noise_len_arr;
  std::vector<char> unaligned_arr;
  std::string seq;
  std::vector<char> rc_arr;
  std::vector<uint32_t> order_arr;
};

// -----------------------------------------------------------------------------
void GenerateSubSeqs(const SeData& data, const uint64_t block_num,
                     core::AccessUnit& raw_au) {
  int64_t rc_to_int[128];
  rc_to_int[static_cast<uint8_t>('d')] = 0;
  rc_to_int[static_cast<uint8_t>('r')] = 1;

  const uint64_t start_read_num = block_num * data.cp.num_reads_per_block;
  uint64_t end_read_num = (block_num + 1) * data.cp.num_reads_per_block;

  if (end_read_num >= data.cp.num_reads) {
    end_read_num = data.cp.num_reads;
  }

  // first find the seq
  uint64_t seq_start, seq_end;
  if (!data.flag_arr[start_read_num]) {
    seq_start = seq_end = 0;  // all reads unaligned
  } else {
    seq_end = seq_start = data.pos_arr[start_read_num];
    // find last read in AU that's aligned
    for (uint64_t i = start_read_num; i < end_read_num; i++) {
      if (!data.flag_arr[i]) break;
      seq_end = std::max(seq_end, data.pos_arr[i] + data.read_length_arr[i]);
    }
  }
  if (seq_start != seq_end) {
    // not all unaligned
    raw_au.Get(core::gen_sub::kReadLength).Push(seq_end - seq_start - 1);
    raw_au.Get(core::gen_sub::kRtype).Push(5);
    for (uint64_t i = seq_start; i < seq_end; i++)
      raw_au.Get(core::gen_sub::kUnalignedReads)
          .Push(GetAlphabetProperties(core::AlphabetId::kAcgtn)
                    .inverse_lut[data.seq[i]]);
  }
  uint64_t prev_position = 0;
  // Write streams
  for (uint64_t i = start_read_num; i < end_read_num; i++) {
    if (data.flag_arr[i]) {
      raw_au.Get(core::gen_sub::kReadLength).Push(data.read_length_arr[i] - 1);
      raw_au.Get(core::gen_sub::kReverseComplement)
          .Push(rc_to_int[static_cast<uint8_t>(data.rc_arr[i])]);
      if (i == start_read_num) {
        // Note: In order non-preserving mode, if the first read of
        // the block is a singleton, then the rest are too.
        raw_au.Get(core::gen_sub::kPositionFirst).Push(0);  // pos
        prev_position = data.pos_arr[i];
      } else {
        const uint64_t diff_pos = data.pos_arr[i] - prev_position;
        raw_au.Get(core::gen_sub::kPositionFirst).Push(diff_pos);  // pos
        prev_position = data.pos_arr[i];
      }
      if (data.noise_len_arr[i] == 0) {
        raw_au.Get(core::gen_sub::kRtype).Push(1);  // rtype = P
      } else {
        raw_au.Get(core::gen_sub::kRtype).Push(3);  // rtype = M
        uint16_t curr_noise_pos = 0;
        for (uint16_t j = 0; j < data.noise_len_arr[i]; j++) {
          curr_noise_pos += data.noise_pos_arr[data.pos_in_noise_arr[i] + j];
          raw_au.Get(core::gen_sub::kMismatchPosTerminator).Push(0);
          if (j == 0)
            raw_au.Get(core::gen_sub::kMismatchPosDelta)
                .Push(data.noise_pos_arr[data.pos_in_noise_arr[i] + j]);
          else
            raw_au.Get(core::gen_sub::kMismatchPosDelta)
                .Push(data.noise_pos_arr[data.pos_in_noise_arr[i] + j] -
                      1);  // decoder adds +1
          raw_au.Get(core::gen_sub::kMismatchType).Push(0);
          raw_au.Get(core::gen_sub::kMismatchTypeSubstBase)
              .Push(GetAlphabetProperties(core::AlphabetId::kAcgtn)
                        .inverse_lut[data.noise_arr[data.pos_in_noise_arr[i] +
                                                    j]]);
          raw_au.PushDependency(
              core::gen_sub::kMismatchTypeSubstBase,
              GetAlphabetProperties(core::AlphabetId::kAcgtn)
                  .inverse_lut[data.seq[data.pos_arr[i] + curr_noise_pos]]);
        }
        raw_au.Get(core::gen_sub::kMismatchPosTerminator).Push(1);
      }
    } else {
      raw_au.Get(core::gen_sub::kRtype).Push(5);
      raw_au.Get(core::gen_sub::kReadLength).Push(data.read_length_arr[i] - 1);
      for (uint64_t j = 0; j < data.read_length_arr[i]; j++) {
        raw_au.Get(core::gen_sub::kUnalignedReads)
            .Push(GetAlphabetProperties(core::AlphabetId::kAcgtn)
                      .inverse_lut[data.unaligned_arr[data.pos_arr[i] + j]]);
      }
      raw_au.Get(core::gen_sub::kPositionFirst).Push(seq_end - prev_position);
      raw_au.Get(core::gen_sub::kReverseComplement).Push(0);
      raw_au.Get(core::gen_sub::kReadLength).Push(data.read_length_arr[i] - 1);
      raw_au.Get(core::gen_sub::kRtype).Push(1);
      prev_position = seq_end;
      seq_end = prev_position + data.read_length_arr[i];
    }
  }
}

// -----------------------------------------------------------------------------

// Task to process a single block
void process_block_task(size_t block_num,
                        std::vector<core::parameter::EncodingSet>& params,
                        const SeData& data,
                        std::vector<uint32_t>& num_reads_per_block,
                        bool write_raw,
                        core::ReadEncoder::entropy_selector* entropy_encoder,
                        std::vector<core::stats::PerfStats>& stat_vec,
                        const std::string& temp_dir) {
  UTILS_LOG(util::Logger::Severity::INFO,
          "-------- Processing block " + std::to_string(block_num) + "/" +
              std::to_string(num_reads_per_block.size()));
  params[block_num] = core::parameter::EncodingSet(
      core::parameter::ParameterSet::DatasetType::kNonAligned,
      core::AlphabetId::kAcgtn, 0, false, false, 1, 0, false, false);
  params[block_num].SetComputedRef(core::parameter::ComputedRef(
      core::parameter::ComputedRef::Algorithm::kGlobalAssembly));
  core::AccessUnit au(std::move(params[block_num]), 0);

  GenerateSubSeqs(data, block_num, au);
  num_reads_per_block[block_num] = static_cast<uint32_t>(
      au.Get(core::gen_sub::kReverseComplement).GetNumSymbols());

  au = core::ReadEncoder::EntropyCodeAu(entropy_encoder, std::move(au),
                                        write_raw);
  stat_vec[block_num].Add(au.GetStats());

  params[block_num] = std::move(au.MoveParameters());

  std::string file_to_save_streams =
      temp_dir + "/read_streams." + std::to_string(block_num);
  for (const auto& d : au) {
    if (d.IsEmpty()) {
      continue;
    }
    std::ofstream out(file_to_save_streams + "." +
                          std::to_string(static_cast<uint8_t>(d.GetId())),
                      std::ios::binary);
    util::BitWriter bw(out);
    d.Write(bw);
  }
}

// -----------------------------------------------------------------------------

void parallel_process_blocks_dynamic(
    const size_t blocks, std::vector<core::parameter::EncodingSet>& params,
    const SeData& data, std::vector<uint32_t>& num_reads_per_block,
    const bool write_raw, core::ReadEncoder::entropy_selector* entropy_encoder,
    std::vector<core::stats::PerfStats>& stat_vec, const std::string& temp_dir,
    const int num_threads) {
  // Create an instance of the DynamicScheduler
  DynamicScheduler scheduler(num_threads);

  // Run the dynamic scheduler with tasks
  scheduler.run(blocks, [&](const SchedulerInfo& info) {
    process_block_task(info.task_id, params, data, num_reads_per_block,
                       write_raw, entropy_encoder, stat_vec, temp_dir);
  });
}

// -----------------------------------------------------------------------------
void GenerateAndCompressSe(const std::string& temp_dir, const SeData& data,
                           core::ReadEncoder::entropy_selector* entropy_encoder,
                           std::vector<core::parameter::EncodingSet>& params,
                           core::stats::PerfStats& stats, bool write_raw) {
  // Now generate new streams and Compress blocks in parallel
  // this is actually number of read pairs per block for PE
  auto blocks = static_cast<uint64_t>(
      std::ceil(static_cast<float>(data.cp.num_reads) /
                static_cast<float>(data.cp.num_reads_per_block)));

  params.resize(blocks);

  std::vector<uint32_t> num_reads_per_block(blocks);
  std::vector<core::stats::PerfStats> stat_vec(blocks);
  parallel_process_blocks_dynamic(blocks, params, data, num_reads_per_block,
                                  write_raw, entropy_encoder, stat_vec,
                                  temp_dir, data.cp.num_thr);

  for (const auto& s : stat_vec) {
    stats.Add(s);
  }

  // write num blocks, reads per block to a file
  const std::string block_info_file = temp_dir + "/block_info.bin";
  std::ofstream f_block_info(block_info_file, std::ios::binary);
  auto num_blocks = static_cast<uint32_t>(blocks);
  f_block_info.write(reinterpret_cast<char*>(&num_blocks), sizeof(uint32_t));
  f_block_info.write(
      reinterpret_cast<char*>(&num_reads_per_block[0]),
      static_cast<std::streamsize>(num_blocks * sizeof(uint32_t)));
}

// -----------------------------------------------------------------------------
void LoadSeData(const CompressionParams& cp, const std::string& temp_dir,
                SeData* data) {
  const std::string file_seq = temp_dir + "/read_seq.txt";
  const std::string file_pos = temp_dir + "/read_pos.bin";
  const std::string file_rc = temp_dir + "/read_rev.txt";
  const std::string file_read_length = temp_dir + "/read_lengths.bin";
  const std::string file_unaligned = temp_dir + "/read_unaligned.txt";
  const std::string file_noise = temp_dir + "/read_noise.txt";
  const std::string file_noise_pos = temp_dir + "/read_noise_pos.bin";

  data->cp = cp;
  // load some params
  uint32_t num_reads = cp.num_reads, num_reads_aligned = 0, num_reads_unaligned;
  data->rc_arr = std::vector<char>(cp.num_reads);
  data->read_length_arr = std::vector<uint16_t>(cp.num_reads);
  data->flag_arr = std::vector<bool>(cp.num_reads);
  data->pos_in_noise_arr = std::vector<uint64_t>(cp.num_reads);
  data->pos_arr = std::vector<uint64_t>(cp.num_reads);
  data->noise_len_arr = std::vector<uint16_t>(cp.num_reads);

  // read streams for aligned reads
  std::ifstream f_seq(file_seq);
  UTILS_DIE_IF(!f_seq, "Cannot open file to read: " + file_seq);
  f_seq.seekg(0, std::ifstream::end);
  uint64_t seq_len = f_seq.tellg();
  data->seq.resize(seq_len);
  f_seq.seekg(0);
  f_seq.read(&data->seq[0], static_cast<std::streamsize>(seq_len));
  f_seq.close();
  std::ifstream f_rc(file_rc);
  UTILS_DIE_IF(!f_rc, "Cannot open file to read: " + file_rc);
  std::ifstream f_read_length(file_read_length, std::ios::binary);
  UTILS_DIE_IF(!f_read_length, "Cannot open file to read: " + file_read_length);
  std::ifstream f_noise(file_noise);
  UTILS_DIE_IF(!f_noise, "Cannot open file to read: " + file_noise);
  std::ifstream f_noise_pos(file_noise_pos, std::ios::binary);
  UTILS_DIE_IF(!f_noise_pos, "Cannot open file to read: " + file_noise_pos);
  std::ifstream f_pos(file_pos, std::ios::binary);
  UTILS_DIE_IF(!f_pos, "Cannot open file to read: " + file_pos);
  f_noise_pos.seekg(0, std::ifstream::end);
  uint64_t noise_array_size = f_noise_pos.tellg() / 2;
  f_noise_pos.seekg(0, std::ifstream::beg);
  // divide by 2 because we have 2 bytes per noise
  data->noise_arr = std::vector<char>(noise_array_size);
  data->noise_pos_arr = std::vector<uint16_t>(noise_array_size);
  char rc, noise_char;
  uint32_t order = 0;
  uint64_t current_pos_noise_arr = 0;
  uint64_t current_pos_noise_pos_arr = 0;
  uint64_t pos;
  uint16_t num_noise_in_curr_read;
  uint16_t read_length, noise_pos;

  while (f_rc.get(rc)) {
    f_read_length.read(reinterpret_cast<char*>(&read_length), sizeof(uint16_t));
    f_pos.read(reinterpret_cast<char*>(&pos), sizeof(uint64_t));
    data->rc_arr[order] = rc;
    data->read_length_arr[order] = read_length;
    data->flag_arr[order] = true;  // aligned
    data->pos_arr[order] = pos;
    data->pos_in_noise_arr[order] = current_pos_noise_arr;
    num_noise_in_curr_read = 0;
    f_noise.get(noise_char);
    while (noise_char != '\n') {
      data->noise_arr[current_pos_noise_arr++] = noise_char;
      num_noise_in_curr_read++;
      f_noise.get(noise_char);
    }
    for (uint16_t i = 0; i < num_noise_in_curr_read; i++) {
      f_noise_pos.read(reinterpret_cast<char*>(&noise_pos), sizeof(uint16_t));
      data->noise_pos_arr[current_pos_noise_pos_arr] = noise_pos;
      current_pos_noise_pos_arr++;
    }
    data->noise_len_arr[order] = num_noise_in_curr_read;
    num_reads_aligned++;
    order++;
  }
  f_noise.close();
  f_noise_pos.close();
  f_rc.close();
  f_pos.close();

  // Now start with unaligned reads
  num_reads_unaligned = num_reads - num_reads_aligned;
  std::string file_unaligned_count = file_unaligned + ".count";
  std::ifstream f_unaligned_count(file_unaligned_count,
                                  std::ios::in | std::ios::binary);
  UTILS_DIE_IF(!f_unaligned_count,
               "Cannot open file to read: " + file_unaligned_count);
  uint64_t unaligned_array_size;
  f_unaligned_count.read(reinterpret_cast<char*>(&unaligned_array_size),
                         sizeof(uint64_t));
  f_unaligned_count.close();
  remove(file_unaligned_count.c_str());
  data->unaligned_arr = std::vector<char>(unaligned_array_size);
  std::ifstream f_unaligned(file_unaligned, std::ios::binary);
  UTILS_DIE_IF(!f_unaligned, "Cannot open file to read: " + file_unaligned);
  std::string unaligned_read;
  uint64_t pos_in_unaligned_arr = 0;
  for (uint32_t i = 0; i < num_reads_unaligned; i++) {
    ReadDnaNFromBits(unaligned_read, f_unaligned);
    std::memcpy(data->unaligned_arr.data() + pos_in_unaligned_arr,
                &unaligned_read[0], unaligned_read.size());
    pos_in_unaligned_arr += unaligned_read.size();
  }
  f_unaligned.close();
  uint64_t current_pos_in_unaligned_arr = 0;
  for (uint32_t i = 0; i < num_reads_unaligned; i++) {
    f_read_length.read(reinterpret_cast<char*>(&read_length), sizeof(uint16_t));
    data->read_length_arr[order] = read_length;
    data->pos_arr[order] = current_pos_in_unaligned_arr;
    current_pos_in_unaligned_arr += read_length;
    data->flag_arr[order] = false;  // unaligned
    order++;
  }
  f_read_length.close();

  // delete old streams
  remove(file_noise.c_str());
  remove(file_noise_pos.c_str());
  remove(file_rc.c_str());
  remove(file_read_length.c_str());
  remove(file_unaligned.c_str());
  remove(file_pos.c_str());
  remove(file_seq.c_str());
}

// -----------------------------------------------------------------------------
void GenerateReadStreamsSe(const std::string& temp_dir,
                           const CompressionParams& cp,
                           core::ReadEncoder::entropy_selector* entropy_encoder,
                           std::vector<core::parameter::EncodingSet>& params,
                           core::stats::PerfStats& stats,
                           const bool write_raw) {
  SeData data;
  LoadSeData(cp, temp_dir, &data);

  GenerateAndCompressSe(temp_dir, data, entropy_encoder, params, stats,
                        write_raw);
}

// -----------------------------------------------------------------------------
void LoadPeData(const CompressionParams& cp, const std::string& temp_dir,
                SeData* data) {
  const std::string file_seq = temp_dir + "/read_seq.txt";
  const std::string file_pos = temp_dir + "/read_pos.bin";
  const std::string file_rc = temp_dir + "/read_rev.txt";
  const std::string file_read_length = temp_dir + "/read_lengths.bin";
  const std::string file_unaligned = temp_dir + "/read_unaligned.txt";
  const std::string file_noise = temp_dir + "/read_noise.txt";
  const std::string file_noise_pos = temp_dir + "/read_noise_pos.bin";
  const std::string file_order = temp_dir + "/read_order.bin";

  data->cp = cp;

  // load some params
  uint32_t num_reads_aligned = 0, num_reads_unaligned;
  data->rc_arr = std::vector<char>(cp.num_reads);
  data->read_length_arr = std::vector<uint16_t>(cp.num_reads);
  data->flag_arr = std::vector<bool>(cp.num_reads);
  data->pos_in_noise_arr = std::vector<uint64_t>(cp.num_reads);
  data->pos_arr = std::vector<uint64_t>(cp.num_reads);
  data->noise_len_arr = std::vector<uint16_t>(cp.num_reads);

  data->order_arr = std::vector<uint32_t>(cp.num_reads);
  // PE step 1: read all streams indexed by original position in FASTQ, also
  // read order array

  // read streams for aligned reads
  std::ifstream f_seq(file_seq);
  UTILS_DIE_IF(!f_seq, "Cannot open file to read: " + file_seq);
  f_seq.seekg(0, std::ifstream::end);
  uint64_t seq_len = f_seq.tellg();
  data->seq.resize(seq_len);
  f_seq.seekg(0);
  f_seq.read(&data->seq[0], static_cast<std::streamsize>(seq_len));
  f_seq.close();
  std::ifstream f_order(file_order, std::ios::binary);
  UTILS_DIE_IF(!f_order, "Cannot open file to read: " + file_order);
  std::ifstream f_rc(file_rc);
  UTILS_DIE_IF(!f_rc, "Cannot open file to read: " + file_rc);
  std::ifstream f_read_length(file_read_length, std::ios::binary);
  UTILS_DIE_IF(!f_read_length, "Cannot open file to read: " + file_read_length);
  std::ifstream f_noise(file_noise);
  UTILS_DIE_IF(!f_noise, "Cannot open file to read: " + file_noise);
  std::ifstream f_noise_pos(file_noise_pos, std::ios::binary);
  UTILS_DIE_IF(!f_noise_pos, "Cannot open file to read: " + file_noise_pos);
  std::ifstream f_pos(file_pos, std::ios::binary);
  UTILS_DIE_IF(!f_pos, "Cannot open file to read: " + file_pos);
  f_noise_pos.seekg(0, std::ifstream::end);
  uint64_t noise_array_size = f_noise_pos.tellg() / 2;
  f_noise_pos.seekg(0, std::ifstream::beg);
  // divide by 2 because we have 2 bytes per noise
  data->noise_arr = std::vector<char>(noise_array_size);
  data->noise_pos_arr = std::vector<uint16_t>(noise_array_size);
  char rc, noise_char;
  uint32_t order = 0;
  uint64_t current_pos_noise_arr = 0;
  uint64_t current_pos_noise_pos_arr = 0;
  uint64_t pos;
  uint16_t num_noise_in_curr_read;
  uint16_t read_length, noise_pos;
  uint32_t ind = 0;
  while (f_rc.get(rc)) {
    f_order.read(reinterpret_cast<char*>(&order), sizeof(uint32_t));
    data->order_arr[ind++] = order;
    f_read_length.read(reinterpret_cast<char*>(&read_length), sizeof(uint16_t));
    f_pos.read(reinterpret_cast<char*>(&pos), sizeof(uint64_t));
    data->rc_arr[order] = rc;
    data->read_length_arr[order] = read_length;
    data->flag_arr[order] = true;  // aligned
    data->pos_arr[order] = pos;
    data->pos_in_noise_arr[order] = current_pos_noise_arr;
    num_noise_in_curr_read = 0;
    f_noise.get(noise_char);
    while (noise_char != '\n') {
      data->noise_arr[current_pos_noise_arr++] = noise_char;
      num_noise_in_curr_read++;
      f_noise.get(noise_char);
    }
    for (uint16_t i = 0; i < num_noise_in_curr_read; i++) {
      f_noise_pos.read(reinterpret_cast<char*>(&noise_pos), sizeof(uint16_t));
      data->noise_pos_arr[current_pos_noise_pos_arr] = noise_pos;
      current_pos_noise_pos_arr++;
    }
    data->noise_len_arr[order] = num_noise_in_curr_read;
    num_reads_aligned++;
  }
  f_noise.close();
  f_noise_pos.close();
  f_rc.close();
  f_pos.close();

  // Now start with unaligned reads
  num_reads_unaligned = cp.num_reads - num_reads_aligned;
  std::string file_unaligned_count = file_unaligned + ".count";
  std::ifstream f_unaligned_count(file_unaligned_count,
                                  std::ios::in | std::ios::binary);
  UTILS_DIE_IF(!f_unaligned_count,
               "Cannot open file to read: " + file_unaligned_count);
  uint64_t unaligned_array_size;
  f_unaligned_count.read(reinterpret_cast<char*>(&unaligned_array_size),
                         sizeof(uint64_t));
  f_unaligned_count.close();
  remove(file_unaligned_count.c_str());
  data->unaligned_arr = std::vector<char>(unaligned_array_size);
  std::ifstream f_unaligned(file_unaligned, std::ios::binary);
  std::string unaligned_read;
  uint64_t pos_in_unaligned_arr = 0;
  for (uint32_t i = 0; i < num_reads_unaligned; i++) {
    ReadDnaNFromBits(unaligned_read, f_unaligned);
    std::memcpy(data->unaligned_arr.data() + pos_in_unaligned_arr,
                &unaligned_read[0], unaligned_read.size());
    pos_in_unaligned_arr += unaligned_read.size();
  }
  f_unaligned.close();

  uint64_t current_pos_in_unaligned_arr = 0;
  for (uint32_t i = 0; i < num_reads_unaligned; i++) {
    f_order.read(reinterpret_cast<char*>(&order), sizeof(uint32_t));
    data->order_arr[ind++] = order;
    f_read_length.read(reinterpret_cast<char*>(&read_length), sizeof(uint16_t));
    data->read_length_arr[order] = read_length;
    data->pos_arr[order] = current_pos_in_unaligned_arr;
    current_pos_in_unaligned_arr += read_length;
    data->flag_arr[order] = false;  // unaligned
  }
  f_order.close();
  f_read_length.close();

  // delete old streams
  remove(file_noise.c_str());
  remove(file_noise_pos.c_str());
  remove(file_rc.c_str());
  remove(file_read_length.c_str());
  remove(file_unaligned.c_str());
  remove(file_pos.c_str());
  remove(file_seq.c_str());
}

// -----------------------------------------------------------------------------
struct PeBlockData {
  std::vector<uint32_t> block_start;
  std::vector<uint32_t> block_end;  // block start and end positions wrt
  std::vector<uint32_t> block_num;
  std::vector<uint32_t> genomic_record_index;
  std::vector<uint32_t> read_index_genomic_record;
  std::vector<uint32_t> block_seq_start;
  std::vector<uint32_t> block_seq_end;
};

// -----------------------------------------------------------------------------
void GenerateBlocksPe(const SeData& data, PeBlockData* block_data) {
  block_data->block_num = std::vector<uint32_t>(data.cp.num_reads);

  block_data->genomic_record_index = std::vector<uint32_t>(data.cp.num_reads);

  // PE step 2: decide on the blocks for the reads
  uint32_t num_records_current_block = 0;
  uint32_t current_block_num = 0;
  uint32_t current_block_seq_end = 0;
  block_data->block_start.push_back(0);

  std::vector already_seen(data.cp.num_reads, false);
  std::vector<uint32_t> to_push_at_end_of_block;

  for (uint32_t i = 0; i < data.cp.num_reads; i++) {
    uint32_t current = data.order_arr[i];
    if (num_records_current_block == 0) {
      // first read of block
      if (!data.flag_arr[current])
        block_data->block_seq_start.push_back(0);
      else
        block_data->block_seq_start.push_back(
            static_cast<uint32_t>(data.pos_arr[current]));
    }
    if (!already_seen[current]) {
      // current is already seen when current is unaligned and its pair
      // has already appeared before - in such cases we don't need to
      // handle it now.
      already_seen[current] = true;
      uint32_t pair = current < data.cp.num_reads / 2
                          ? current + data.cp.num_reads / 2
                          : current - data.cp.num_reads / 2;
      if (already_seen[pair]) {
        if (block_data->block_num[pair] == current_block_num &&
            data.pos_arr[current] >= data.pos_arr[pair] &&
            data.pos_arr[current] - data.pos_arr[pair] < 32768) {
          // put in same record
          block_data->block_num[current] = current_block_num;
          block_data->genomic_record_index[current] =
              block_data->genomic_record_index[pair];
        } else {
          // new genomic record
          block_data->block_num[current] = current_block_num;
          block_data->genomic_record_index[current] =
              num_records_current_block++;
          block_data->read_index_genomic_record.push_back(current);
        }
      } else {
        // add new genomic record
        block_data->block_num[current] = current_block_num;
        block_data->genomic_record_index[current] = num_records_current_block++;

        if (!data.flag_arr[pair]) {
          already_seen[pair] = true;
          if (!data.flag_arr[current]) {
            // both unaligned - put in same record
            block_data->block_num[pair] = current_block_num;
            block_data->genomic_record_index[pair] =
                num_records_current_block - 1;
            block_data->read_index_genomic_record.push_back(
                std::min(current,
                         pair));  // always put read 1 first in this case
          } else {
            // pair is unaligned, put in same block at end (not in
            // same record) for now, put in to_push_at_end_of_block
            // vector (processed at end of block)
            block_data->read_index_genomic_record.push_back(current);
            to_push_at_end_of_block.push_back(pair);
          }
        } else {
          block_data->read_index_genomic_record.push_back(current);
        }
      }
      if (data.flag_arr[current]) {
        // update current_block_seq_end
        if (current_block_seq_end <
            data.pos_arr[current] + data.read_length_arr[current])
          current_block_seq_end = static_cast<uint32_t>(
              data.pos_arr[current] + data.read_length_arr[current]);
      }
    }
    if (num_records_current_block == data.cp.num_reads_per_block ||
        i == data.cp.num_reads - 1) {
      // block done
      // put in the reads in to_push_at_end_of_block vector and
      // clearStreamState the vector
      for (uint32_t j : to_push_at_end_of_block) {
        block_data->block_num[j] = current_block_num;
        block_data->genomic_record_index[j] = num_records_current_block++;
        block_data->read_index_genomic_record.push_back(j);
      }
      to_push_at_end_of_block.clear();
      block_data->block_end.push_back(block_data->block_start.back() +
                                      num_records_current_block);
      block_data->block_seq_end.push_back(current_block_seq_end);

      if (i != data.cp.num_reads - 1) {
        // start new block
        num_records_current_block = 0;
        current_block_num++;
        current_block_seq_end = 0;
        block_data->block_start.push_back(static_cast<uint32_t>(
            block_data->read_index_genomic_record.size()));
      }
    }
  }
  already_seen.clear();
}

// -----------------------------------------------------------------------------
void GenerateQualityIdPaired(const std::string& temp_dir,
                             const PeBlockData& block_data,
                             uint32_t num_reads) {
  // PE step 3: generate index for ids and quality

  const std::string file_order_quality = temp_dir + "/order_quality.bin";
  const std::string file_blocks_quality = temp_dir + "/blocks_quality.bin";
  const std::string file_order_id = temp_dir + "/order_id.bin";
  const std::string file_blocks_id = temp_dir + "/blocks_id.bin";

  // quality:
  std::ofstream f_order_quality(file_order_quality, std::ios::binary);
  // store order (as usual in uint32_t)
  std::ofstream f_blocks_quality(file_blocks_quality, std::ios::binary);
  // store block start and end positions (differs from the block_start and end
  // because here we measure in terms of quality values rather than records
  uint32_t quality_block_pos = 0;
  for (uint32_t i = 0; i < block_data.block_start.size(); i++) {
    f_blocks_quality.write(reinterpret_cast<char*>(&quality_block_pos),
                           sizeof(uint32_t));
    for (uint32_t j = block_data.block_start[i]; j < block_data.block_end[i];
         j++) {
      uint32_t current = block_data.read_index_genomic_record[j];
      uint32_t pair = current < num_reads / 2 ? current + num_reads / 2
                                              : current - num_reads / 2;
      if (block_data.block_num[current] == block_data.block_num[pair] &&
          block_data.genomic_record_index[pair] ==
              block_data.genomic_record_index[current]) {
        // pair in genomic record
        f_order_quality.write(reinterpret_cast<char*>(&current),
                              sizeof(uint32_t));
        quality_block_pos++;
        f_order_quality.write(reinterpret_cast<char*>(&pair), sizeof(uint32_t));
        quality_block_pos++;
      } else {
        // only single read in genomic record
        f_order_quality.write(reinterpret_cast<char*>(&current),
                              sizeof(uint32_t));
        quality_block_pos++;
      }
    }
    f_blocks_quality.write(reinterpret_cast<char*>(&quality_block_pos),
                           sizeof(uint32_t));
  }
  f_order_quality.close();
  f_blocks_quality.close();
  // id:
  std::ofstream f_blocks_id(file_blocks_id, std::ios::binary);
  // store block start and end positions (measured in terms of records since 1
  // record = 1 id)
  for (uint32_t i = 0; i < block_data.block_start.size(); i++) {
    f_blocks_id.write(reinterpret_cast<const char*>(&block_data.block_start[i]),
                      sizeof(uint32_t));
    f_blocks_id.write(reinterpret_cast<const char*>(&block_data.block_end[i]),
                      sizeof(uint32_t));
    std::ofstream f_order_id(file_order_id + "." + std::to_string(i),
                             std::ios::binary);
    // store order
    for (uint32_t j = block_data.block_start[i]; j < block_data.block_end[i];
         j++) {
      uint32_t current = block_data.read_index_genomic_record[j];
      uint32_t pair = current < num_reads / 2 ? current + num_reads / 2
                                              : current - num_reads / 2;
      // just write the min of current and pair
      uint32_t min_index = current > pair ? pair : current;
      f_order_id.write(reinterpret_cast<char*>(&min_index), sizeof(uint32_t));
    }
    f_order_id.close();
  }
  f_blocks_id.close();
}

// -----------------------------------------------------------------------------
struct PeStatistics {
  std::vector<uint32_t> count_same_rec;
  std::vector<uint32_t> count_split_same_au;
  std::vector<uint32_t> count_split_diff_au;
};

// -----------------------------------------------------------------------------
void GenerateStreamsPe(const SeData& data, const PeBlockData& block_data,
                       const uint64_t cur_block_num, PeStatistics& pest,
                       const size_t cur_thread_num, core::AccessUnit& raw_au) {
  int64_t rc_to_int[128];
  rc_to_int[static_cast<uint8_t>('d')] = 0;
  rc_to_int[static_cast<uint8_t>('r')] = 1;

  // first find the seq
  uint64_t seq_end = block_data.block_seq_end[cur_block_num];
  if (const uint64_t seq_start = block_data.block_seq_start[cur_block_num];
      seq_start != seq_end) {
    // not all unaligned
    raw_au.Get(core::gen_sub::kReadLength).Push(seq_end - seq_start - 1);
    raw_au.Get(core::gen_sub::kRtype).Push(5);
    for (uint64_t i = seq_start; i < seq_end; i++)
      raw_au.Get(core::gen_sub::kUnalignedReads)
          .Push(GetAlphabetProperties(core::AlphabetId::kAcgtn)
                    .inverse_lut[data.seq[i]]);
  }
  uint64_t prev_pos = 0;
  // Write streams
  for (uint32_t i = block_data.block_start[cur_block_num];
       i < block_data.block_end[cur_block_num]; i++) {
    uint32_t current = block_data.read_index_genomic_record[i];
    uint32_t pair = current < data.cp.num_reads / 2
                        ? current + data.cp.num_reads / 2
                        : current - data.cp.num_reads / 2;

    if (data.flag_arr[current]) {
      if (i == block_data.block_start[cur_block_num]) {
        // Note: In order non-preserving mode, if the first read of
        // the block is a singleton, then the rest are too.
        raw_au.Get(core::gen_sub::kPositionFirst).Push(0);  // pos
        prev_pos = data.pos_arr[current];
      } else {
        const uint64_t diff_pos = data.pos_arr[current] - prev_pos;
        raw_au.Get(core::gen_sub::kPositionFirst).Push(diff_pos);  // pos
        prev_pos = data.pos_arr[current];
      }
    }
    if (block_data.block_num[current] == block_data.block_num[pair] &&
        block_data.genomic_record_index[current] ==
            block_data.genomic_record_index[pair]) {
      // both reads in same record
      if (!data.flag_arr[current]) {
        // Case 1: both unaligned
        raw_au.Get(core::gen_sub::kRtype).Push(5);
        raw_au.Get(core::gen_sub::kReadLength)
            .Push(data.read_length_arr[current] + data.read_length_arr[pair] -
                  1);
        for (uint64_t j = 0; j < data.read_length_arr[current]; j++) {
          raw_au.Get(core::gen_sub::kUnalignedReads)
              .Push(GetAlphabetProperties(core::AlphabetId::kAcgtn)
                        .inverse_lut[data.unaligned_arr[data.pos_arr[current] +
                                                        j]]);
        }
        for (uint64_t j = 0; j < data.read_length_arr[pair]; j++) {
          raw_au.Get(core::gen_sub::kUnalignedReads)
              .Push(
                  GetAlphabetProperties(core::AlphabetId::kAcgtn)
                      .inverse_lut[data.unaligned_arr[data.pos_arr[pair] + j]]);
        }
        raw_au.Get(core::gen_sub::kPositionFirst).Push(seq_end - prev_pos);
        raw_au.Get(core::gen_sub::kReverseComplement).Push(0);
        raw_au.Get(core::gen_sub::kReverseComplement).Push(0);
        raw_au.Get(core::gen_sub::kReadLength)
            .Push(data.read_length_arr[current] - 1);
        raw_au.Get(core::gen_sub::kReadLength)
            .Push(data.read_length_arr[pair] - 1);
        raw_au.Get(core::gen_sub::kRtype).Push(1);
        raw_au.Get(core::gen_sub::kPairDecodingCase)
            .Push(0);  // pair decoding case same_rec
        const uint16_t delta = data.read_length_arr[current];
        raw_au.Get(core::gen_sub::kPairSameRec).Push(2 * delta);  // pair
        prev_pos = seq_end;
        seq_end = prev_pos + data.read_length_arr[current] +
                  data.read_length_arr[pair];
      } else {
        // Case 2: both aligned
        raw_au.Get(core::gen_sub::kReadLength)
            .Push(data.read_length_arr[current] - 1);
        raw_au.Get(core::gen_sub::kReadLength)
            .Push(data.read_length_arr[pair] - 1);
        raw_au.Get(core::gen_sub::kReverseComplement)
            .Push(rc_to_int[static_cast<uint8_t>(data.rc_arr[current])]);
        raw_au.Get(core::gen_sub::kReverseComplement)
            .Push(rc_to_int[static_cast<uint8_t>(data.rc_arr[pair])]);
        if (data.noise_len_arr[current] == 0 && data.noise_len_arr[pair] == 0) {
          raw_au.Get(core::gen_sub::kRtype).Push(1);  // rtype = P
        } else {
          raw_au.Get(core::gen_sub::kRtype).Push(3);  // rtype = M
          for (int k = 0; k < 2; k++) {
            const uint32_t index = k ? pair : current;
            uint16_t curr_noise_pos = 0;
            for (uint16_t j = 0; j < data.noise_len_arr[index]; j++) {
              curr_noise_pos +=
                  data.noise_pos_arr[data.pos_in_noise_arr[index] + j];
              raw_au.Get(core::gen_sub::kMismatchPosTerminator).Push(0);
              if (j == 0)
                raw_au.Get(core::gen_sub::kMismatchPosDelta)
                    .Push(data.noise_pos_arr[data.pos_in_noise_arr[index] + j]);
              else
                raw_au.Get(core::gen_sub::kMismatchPosDelta)
                    .Push(data.noise_pos_arr[data.pos_in_noise_arr[index] + j] -
                          1);
              raw_au.Get(core::gen_sub::kMismatchType).Push(0);  // Substitution
              raw_au.Get(core::gen_sub::kMismatchTypeSubstBase)
                  .Push(
                      GetAlphabetProperties(core::AlphabetId::kAcgtn)
                          .inverse_lut[data.noise_arr
                                           [data.pos_in_noise_arr[index] + j]]);
              raw_au.PushDependency(
                  core::gen_sub::kMismatchTypeSubstBase,
                  GetAlphabetProperties(core::AlphabetId::kAcgtn)
                      .inverse_lut[data.seq[data.pos_arr[index] +
                                            curr_noise_pos]]);
            }
            raw_au.Get(core::gen_sub::kMismatchPosTerminator).Push(1);
          }
        }
        const bool read_1_first = current < pair;
        const auto delta =
            static_cast<uint16_t>(data.pos_arr[pair] - data.pos_arr[current]);
        raw_au.Get(core::gen_sub::kPairDecodingCase)
            .Push(0);  // pair decoding case same_rec
        raw_au.Get(core::gen_sub::kPairSameRec)
            .Push(!read_1_first + 2 * delta);  // pair
        pest.count_same_rec[cur_thread_num]++;
      }
    } else {
      // only one read in genomic record
      if (data.flag_arr[current]) {
        raw_au.Get(core::gen_sub::kReadLength)
            .Push(data.read_length_arr[current] - 1);
        raw_au.Get(core::gen_sub::kReverseComplement)
            .Push(rc_to_int[static_cast<uint8_t>(data.rc_arr[current])]);
        if (data.noise_len_arr[current] == 0) {
          raw_au.Get(core::gen_sub::kRtype).Push(1);  // rtype = P
        } else {
          raw_au.Get(core::gen_sub::kRtype).Push(3);  // rtype = M
          uint16_t curr_noise_pos = 0;
          for (uint16_t j = 0; j < data.noise_len_arr[current]; j++) {
            curr_noise_pos +=
                data.noise_pos_arr[data.pos_in_noise_arr[current] + j];
            raw_au.Get(core::gen_sub::kMismatchPosTerminator).Push(0);
            if (j == 0)
              raw_au.Get(core::gen_sub::kMismatchPosDelta)
                  .Push(data.noise_pos_arr[data.pos_in_noise_arr[current] + j]);
            else
              raw_au.Get(core::gen_sub::kMismatchPosDelta)
                  .Push(data.noise_pos_arr[data.pos_in_noise_arr[current] + j] -
                        1);
            raw_au.Get(core::gen_sub::kMismatchType).Push(0);  // Substitution
            raw_au.Get(core::gen_sub::kMismatchTypeSubstBase)
                .Push(
                    GetAlphabetProperties(core::AlphabetId::kAcgtn)
                        .inverse_lut[data.noise_arr
                                         [data.pos_in_noise_arr[current] + j]]);
            raw_au.PushDependency(
                core::gen_sub::kMismatchTypeSubstBase,
                GetAlphabetProperties(core::AlphabetId::kAcgtn)
                    .inverse_lut[data.seq[data.pos_arr[current] +
                                          curr_noise_pos]]);
          }
          raw_au.Get(core::gen_sub::kMismatchPosTerminator).Push(1);
        }
      } else {
        raw_au.Get(core::gen_sub::kRtype).Push(5);
        raw_au.Get(core::gen_sub::kReadLength)
            .Push(data.read_length_arr[current] - 1);
        for (uint64_t j = 0; j < data.read_length_arr[current]; j++) {
          raw_au.Get(core::gen_sub::kUnalignedReads)
              .Push(GetAlphabetProperties(core::AlphabetId::kAcgtn)
                        .inverse_lut[data.unaligned_arr[data.pos_arr[current] +
                                                        j]]);
        }
        raw_au.Get(core::gen_sub::kPositionFirst).Push(seq_end - prev_pos);
        raw_au.Get(core::gen_sub::kReverseComplement).Push(0);
        raw_au.Get(core::gen_sub::kReadLength)
            .Push(data.read_length_arr[current] - 1);
        raw_au.Get(core::gen_sub::kRtype).Push(1);
        prev_pos = seq_end;
        seq_end = prev_pos + data.read_length_arr[current];
      }

      // pair subsequences
      const bool same_block =
          block_data.block_num[current] == block_data.block_num[pair];
      if (same_block)
        pest.count_split_same_au[cur_thread_num]++;
      else
        pest.count_split_diff_au[cur_thread_num]++;

      if (const bool read_1_first = current < pair;
          same_block && !read_1_first) {
        raw_au.Get(core::gen_sub::kPairDecodingCase).Push(1);  // R1_split
        raw_au.Get(core::gen_sub::kPairR1Split)
            .Push(block_data.genomic_record_index[pair]);
      } else if (same_block && read_1_first) {
        raw_au.Get(core::gen_sub::kPairDecodingCase).Push(2);  // R2_split
        raw_au.Get(core::gen_sub::kPairR2Split)
            .Push(block_data.genomic_record_index[pair]);
      } else if (!same_block && !read_1_first) {
        raw_au.Get(core::gen_sub::kPairDecodingCase)
            .Push(3);  // R1_diff_ref_seq
        raw_au.Get(core::gen_sub::kPairR1DiffSeq)
            .Push(block_data.block_num[pair]);
        raw_au.Get(core::gen_sub::kPairR1DiffPos)
            .Push(block_data.genomic_record_index[pair]);
      } else {
        raw_au.Get(core::gen_sub::kPairDecodingCase)
            .Push(4);  // R2_diff_ref_seq
        raw_au.Get(core::gen_sub::kPairR2DiffSeq)
            .Push(block_data.block_num[pair]);
        raw_au.Get(core::gen_sub::kPairR2DiffPos)
            .Push(block_data.genomic_record_index[pair]);
      }
    }
  }
}

void process_block_task(size_t cur_block_num, const PeBlockData& block_data,
                        std::vector<core::parameter::EncodingSet>& params,
                        PeStatistics& pest, const SeData& data,
                        std::vector<uint32_t>& num_reads_per_block,
                        std::vector<uint32_t>& num_records_per_block,
                        bool write_raw,
                        core::ReadEncoder::entropy_selector* entropy_encoder,
                        std::vector<core::stats::PerfStats>& stat_vec,
                        const std::string& temp_dir, size_t cur_thread_num) {
  UTILS_LOG(util::Logger::Severity::INFO,
            "-------- Processing block " + std::to_string(cur_block_num) + "/" +
                std::to_string(block_data.block_start.size()));
  params[cur_block_num] = core::parameter::EncodingSet(
      core::parameter::ParameterSet::DatasetType::kNonAligned,
      core::AlphabetId::kAcgtn, 0, true, false, 1, 0, false, false);
  params[cur_block_num].SetComputedRef(core::parameter::ComputedRef(
      core::parameter::ComputedRef::Algorithm::kGlobalAssembly));
  core::AccessUnit au(std::move(params[cur_block_num]), 0);

  GenerateStreamsPe(data, block_data, cur_block_num, pest, cur_thread_num, au);
  num_reads_per_block[cur_block_num] = static_cast<uint32_t>(
      au.Get(core::gen_sub::kReverseComplement).GetNumSymbols());
  num_records_per_block[cur_block_num] =
      block_data.block_end[cur_block_num] -
      block_data.block_start[cur_block_num];  // used later for ids
  au = core::ReadEncoder::EntropyCodeAu(entropy_encoder, std::move(au),
                                        write_raw);

  stat_vec[cur_block_num].Add(au.GetStats());

  params[cur_block_num] = std::move(au.MoveParameters());

  std::string file_to_save_streams =
      temp_dir + "/read_streams." + std::to_string(cur_block_num);
  for (const auto& d : au) {
    if (d.IsEmpty()) {
      continue;
    }
    std::ofstream out(file_to_save_streams + "." +
                          std::to_string(static_cast<uint8_t>(d.GetId())),
                      std::ios::binary);
    util::BitWriter bw(out);
    d.Write(bw);
  }
}

// Function replacing OpenMP loop
void parallel_process_blocks_dynamic(
    const PeBlockData& block_data,
    std::vector<core::parameter::EncodingSet>& params, PeStatistics& pest,
    const SeData& data, std::vector<uint32_t>& num_reads_per_block,
    std::vector<uint32_t>& num_records_per_block, const bool write_raw,
    core::ReadEncoder::entropy_selector* entropy_encoder,
    std::vector<core::stats::PerfStats>& stat_vec, const std::string& temp_dir,
    const int num_threads) {
  // Create an instance of the DynamicScheduler
  DynamicScheduler scheduler(num_threads);

  // Run the dynamic scheduler with tasks
  scheduler.run(block_data.block_start.size(), [&](const SchedulerInfo& info) {
    process_block_task(info.task_id, block_data, params, pest, data,
                       num_reads_per_block, num_records_per_block, write_raw,
                       entropy_encoder, stat_vec, temp_dir, info.thread_id);
  });
}

// -----------------------------------------------------------------------------
void GenerateReadStreamsPe(const std::string& temp_dir,
                           const CompressionParams& cp,
                           core::ReadEncoder::entropy_selector* entropy_encoder,
                           std::vector<core::parameter::EncodingSet>& params,
                           core::stats::PerfStats& stats, bool write_raw) {
  // basic approach: start looking at reads from left to right. If current is
  // aligned but pair is unaligned, pair is kept at the end current AU and
  // stored in different record. We try to keep number of records in AU =
  // num_reads_per_block (without counting the unaligned pairs above. As
  // we scan to right, if we come across a read whose pair has already been
  // seen in the same AU and the gap is < 32768 (and non-overlapping since
  // delta >= 0), then we add this to the paired read's genomic record.
  // Finally, when we come to unaligned reads whose pair is also unaligned, we
  // store them in same genomic record.

  SeData data;
  LoadPeData(cp, temp_dir, &data);

  PeBlockData block_data;
  GenerateBlocksPe(data, &block_data);
  data.order_arr.clear();

  GenerateQualityIdPaired(temp_dir, block_data, cp.num_reads);

  PeStatistics pest;
  pest.count_same_rec = std::vector<uint32_t>(cp.num_thr, 0);
  pest.count_split_same_au = std::vector<uint32_t>(cp.num_thr, 0);
  pest.count_split_diff_au = std::vector<uint32_t>(cp.num_thr, 0);

  std::vector<uint32_t> num_reads_per_block(block_data.block_start.size());
  std::vector<uint32_t> num_records_per_block(block_data.block_start.size());

  params.resize(block_data.block_start.size());

  std::vector<core::stats::PerfStats> stat_vec(block_data.block_start.size());

  // PE step 4: Now generate read streams and Compress blocks in parallel
  // this is actually number of read pairs per block for PE
  parallel_process_blocks_dynamic(block_data, params, pest, data,
                                  num_reads_per_block, num_records_per_block,
                                  write_raw, entropy_encoder, stat_vec,
                                  temp_dir, cp.num_thr);

  for (const auto& s : stat_vec) {
    stats.Add(s);
  }

  UTILS_LOG(util::Logger::Severity::INFO,
            "---- Joint records: " +
                std::to_string(std::accumulate(pest.count_same_rec.begin(),
                                               pest.count_same_rec.end(), 0u)));

  UTILS_LOG(util::Logger::Severity::INFO,
            "---- Split records, same AU: " +
                std::to_string(std::accumulate(pest.count_same_rec.begin(),
                                               pest.count_same_rec.end(), 0u)));

  UTILS_LOG(
      util::Logger::Severity::INFO,
      "---- Split records, diff AU: " +
          std::to_string(std::accumulate(pest.count_split_diff_au.begin(),
                                         pest.count_split_diff_au.end(), 0u)));

  // write num blocks, reads per block and records per block to a file
  const std::string block_info_file = temp_dir + "/block_info.bin";
  std::ofstream f_block_info(block_info_file, std::ios::binary);
  auto num_blocks = static_cast<uint32_t>(block_data.block_start.size());
  f_block_info.write(reinterpret_cast<char*>(&num_blocks), sizeof(uint32_t));
  f_block_info.write(
      reinterpret_cast<char*>(&num_reads_per_block[0]),
      static_cast<std::streamsize>(num_blocks * sizeof(uint32_t)));
  f_block_info.write(
      reinterpret_cast<char*>(&num_records_per_block[0]),
      static_cast<std::streamsize>(num_blocks * sizeof(uint32_t)));
}

// -----------------------------------------------------------------------------
void GenerateReadStreams(const std::string& temp_dir,
                         const CompressionParams& cp,
                         core::ReadEncoder::entropy_selector* entropy_encoder,
                         std::vector<core::parameter::EncodingSet>& params,
                         core::stats::PerfStats& stats, const bool write_raw) {
  if (!cp.paired_end)
    GenerateReadStreamsSe(temp_dir, cp, entropy_encoder, params, stats,
                          write_raw);
  else
    GenerateReadStreamsPe(temp_dir, cp, entropy_encoder, params, stats,
                          write_raw);
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------
