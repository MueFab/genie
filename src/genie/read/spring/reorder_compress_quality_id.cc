/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */


// -----------------------------------------------------------------------------

#include "genie/read/spring/reorder_compress_quality_id.h"

#include <cmath>
#include <cstdio>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "genie/core/access_unit.h"
#include "genie/core/read_encoder.h"
#include "genie/core/record/chunk.h"
#include "genie/core/record/segment.h"
#include "genie/read/spring/dynamic_scheduler.h"
#include "genie/read/spring/util.h"

// -----------------------------------------------------------------------------

namespace genie::read::spring {

// -----------------------------------------------------------------------------
void ReorderCompressQualityId(const std::string& temp_dir,
                              const CompressionParams& cp,
                              core::ReadEncoder::qv_selector* qv_coder,
                              core::ReadEncoder::name_selector* name_coder,
                              core::ReadEncoder::entropy_selector* entropy,
                              std::vector<core::parameter::EncodingSet>& params,
                              core::stats::PerfStats& stats, bool write_raw) {
  // Read some parameters
  uint32_t num_reads = cp.num_reads;
  int num_thr = cp.num_thr;
  bool preserve_id = cp.preserve_id;
  bool preserve_quality = cp.preserve_quality;
  bool paired_end = cp.paired_end;
  uint32_t num_reads_per_block = cp.num_reads_per_block;

  const std::string& basedir = temp_dir;

  std::string file_order = basedir + "/read_order.bin";
  std::string file_id = basedir + "/id_1";
  std::string file_quality[2];
  file_quality[0] = basedir + "/quality_1";
  file_quality[1] = basedir + "/quality_2";

  if (!paired_end) {
    // array containing index mapping position in original fastq to
    // position after reordering
    auto order_array = std::vector<uint32_t>(num_reads);
    GenerateOrder(file_order, order_array, num_reads);

    uint32_t str_array_size =
        (1 + (num_reads / 4 - 1) / num_reads_per_block) * num_reads_per_block;
    // smallest multiple of num_reads_per_block bigger than num_reads/4
    // num_reads/4 chosen so that these many qualities/ids can be stored in
    // memory without exceeding the RAM consumption of reordering stage
    auto str_array = std::vector<std::string>(str_array_size);
    // array to load ids and/or qualities into

    if (preserve_quality) {
      std::cerr << "Compressing qualities\n";
      uint32_t num_reads_per_file = num_reads;
      ReorderCompress(file_quality[0], temp_dir, num_reads_per_file, num_thr,
                      num_reads_per_block, str_array, str_array_size,
                      order_array, "quality", qv_coder, name_coder, entropy,
                      params, stats, write_raw);
      remove(file_quality[0].c_str());
    }
    if (preserve_id) {
      std::cerr << "Compressing ids\n";
      uint32_t num_reads_per_file = num_reads;
      ReorderCompress(file_id, temp_dir, num_reads_per_file, num_thr,
                      num_reads_per_block, str_array, str_array_size,
                      order_array, "id", qv_coder, name_coder, entropy, params,
                      stats, write_raw);
      remove(file_id.c_str());
    }

  } else {
    // paired end
    std::string file_order_quality = basedir + "/order_quality.bin";
    std::string file_order_id = basedir + "/order_id.bin";
    std::string file_blocks_quality = basedir + "/blocks_quality.bin";
    std::string file_blocks_id = basedir + "/blocks_id.bin";
    std::vector<uint32_t> block_start, block_end;
    if (preserve_quality) {
      // read block start and end into vector
      ReadBlockStartEnd(file_blocks_quality, block_start, block_end);
      // read order into order_array
      auto order_array = std::vector<uint32_t>(num_reads);
      GenerateOrder(file_order_quality, order_array, num_reads);
      uint64_t quality_array_size = num_reads / 4 + 3 * num_reads_per_block;
      auto quality_array = std::vector<std::string>(quality_array_size);
      // num_reads/4 so that memory consumption isn't too high
      // 3*num_reads_per_block added to ensure that we are done in 4
      // passes (needed because block sizes are not exactly equal to
      // num_reads_per_block
      ReorderCompressQualityPe(file_quality, temp_dir, quality_array,
                               quality_array_size, order_array, block_start,
                               block_end, cp, qv_coder, entropy, params, stats,
                               write_raw);
      remove(file_quality[0].c_str());
      remove(file_quality[1].c_str());
      block_start.clear();
      block_end.clear();
    }
    if (preserve_id) {
      ReadBlockStartEnd(file_blocks_id, block_start, block_end);
      auto id_array = std::vector<std::string>(num_reads / 2);
      std::ifstream f_id(file_id);
      UTILS_DIE_IF(!f_id, "Cannot open file to read: " + file_id);
      for (uint32_t i = 0; i < num_reads / 2; i++)
        std::getline(f_id, id_array[i]);
      ReorderCompressIdPe(id_array, temp_dir, file_order_id, block_start,
                          block_end, cp, name_coder, entropy, params, stats,
                          write_raw);
      for (uint32_t i = 0; i < block_start.size(); i++)
        remove((file_order_id + "." + std::to_string(i)).c_str());
      remove(file_id.c_str());
      block_start.clear();
      block_end.clear();
    }
    remove(file_order_quality.c_str());
    remove(file_blocks_quality.c_str());
    remove(file_blocks_quality.c_str());
  }
}

// -----------------------------------------------------------------------------
void ReadBlockStartEnd(const std::string& file_blocks,
                       std::vector<uint32_t>& block_start,
                       std::vector<uint32_t>& block_end) {
  std::ifstream f_blocks(file_blocks, std::ios::binary);
  UTILS_DIE_IF(!f_blocks, "Cannot open file to read: " + file_blocks);
  uint32_t block_pos_temp;
  f_blocks.read(reinterpret_cast<char*>(&block_pos_temp), sizeof(uint32_t));
  while (!f_blocks.eof()) {
    block_start.push_back(block_pos_temp);
    f_blocks.read(reinterpret_cast<char*>(&block_pos_temp), sizeof(uint32_t));
    block_end.push_back(block_pos_temp);
    f_blocks.read(reinterpret_cast<char*>(&block_pos_temp), sizeof(uint32_t));
  }
  f_blocks.close();
}

// -----------------------------------------------------------------------------
void GenerateOrder(const std::string& file_order,
                   std::vector<uint32_t>& order_array,
                   const uint32_t& num_reads) {
  std::ifstream fin_order(file_order, std::ios::binary);
  UTILS_DIE_IF(!fin_order, "Cannot open file to read: " + file_order);
  uint32_t order;
  for (uint32_t i = 0; i < num_reads; i++) {
    fin_order.read(reinterpret_cast<char*>(&order), sizeof(uint32_t));
    order_array[order] = i;
  }
  fin_order.close();
}

// Task to process a single block
void process_block_task(size_t block_num,
                        const std::vector<uint32_t>& block_start,
                        const std::vector<uint32_t>& block_end,
                        const std::vector<std::string>& id_array,
                        const std::string& file_order_id,
                        core::ReadEncoder::name_selector* name_coder,
                        core::ReadEncoder::entropy_selector* entropy,
                        std::vector<core::parameter::EncodingSet>& params,
                        std::vector<core::stats::PerfStats>& stat_vec,
                        bool write_raw, const std::string& id_desc_prefix) {
  std::ifstream f_order_id(file_order_id + "." + std::to_string(block_num),
                           std::ios::binary);
  if (!f_order_id.is_open()) {
    throw std::runtime_error("Cannot open file to read: " + file_order_id +
                             "." + std::to_string(block_num));
  }

  auto id_array_block =
      std::vector<std::string>(block_end[block_num] - block_start[block_num]);
  uint32_t index;
  for (uint32_t j = block_start[block_num]; j < block_end[block_num]; j++) {
    f_order_id.read(reinterpret_cast<char*>(&index), sizeof(uint32_t));
    id_array_block[j - block_start[block_num]] = id_array[index];
  }

  core::record::Chunk chunk;
  for (size_t i = 0; i < block_end[block_num] - block_start[block_num]; ++i) {
    chunk.GetData().emplace_back(
        static_cast<uint8_t>(1), core::record::ClassType::kClassU,
        std::move(id_array_block[i]), "", static_cast<uint8_t>(0));
    chunk.GetData().back().AddSegment(core::record::Segment("N"));
  }

  auto raw_desc = name_coder->Process(chunk);
  stat_vec[block_num].Add(std::get<1>(raw_desc));
  chunk.GetData().clear();

  if (write_raw && block_num < 10) {
    for (uint16_t i = 0;
         i < static_cast<uint16_t>(std::get<0>(raw_desc).GetSize()); ++i) {
      if (std::get<0>(raw_desc).Get(i).IsEmpty()) {
        continue;
      }
      std::ofstream out_file_stream(
          "raw_stream_" + std::to_string(block_num) + "_" +
          std::to_string(static_cast<uint8_t>(core::GenDesc::kReadName)) + "_" +
          std::to_string(static_cast<uint8_t>(i)));
      out_file_stream.write(
          static_cast<char*>(std::get<0>(raw_desc).Get(i).GetData().GetData()),
          static_cast<std::streamsize>(
              std::get<0>(raw_desc).Get(i).GetData().GetRawSize()));
    }
  }

  auto encoded = entropy->Process(std::get<0>(raw_desc));
  stat_vec[block_num].Add(std::get<2>(encoded));
  params[block_num].SetDescriptor(core::GenDesc::kReadName,
                                  std::move(std::get<0>(encoded)));

  std::string file_to_save_streams = id_desc_prefix + std::to_string(block_num);
  std::ofstream outfile(file_to_save_streams, std::ios::binary);
  util::BitWriter bw(outfile);
  std::get<1>(encoded).Write(bw);

  f_order_id.close();
}

void parallel_process_blocks_dynamic(
    const std::vector<uint32_t>& block_start,
    const std::vector<uint32_t>& block_end,
    const std::vector<std::string>& id_array, const std::string& file_order_id,
    core::ReadEncoder::name_selector* name_coder,
    core::ReadEncoder::entropy_selector* entropy,
    std::vector<core::parameter::EncodingSet>& params,
    std::vector<core::stats::PerfStats>& stat_vec, const bool write_raw,
    const std::string& id_desc_prefix, const int num_threads) {
  // Create an instance of the DynamicScheduler
  DynamicScheduler scheduler(num_threads);

  // Run the dynamic scheduler with tasks
  scheduler.run(block_start.size(), [&](const SchedulerInfo& info) {
    process_block_task(info.task_id, block_start, block_end, id_array,
                       file_order_id, name_coder, entropy, params, stat_vec,
                       write_raw, id_desc_prefix);
  });
}

// -----------------------------------------------------------------------------
void ReorderCompressIdPe(const std::vector<std::string>& id_array,
                         const std::string& temp_dir,
                         const std::string& file_order_id,
                         const std::vector<uint32_t>& block_start,
                         const std::vector<uint32_t>& block_end,
                         const CompressionParams& cp,
                         core::ReadEncoder::name_selector* name_coder,
                         core::ReadEncoder::entropy_selector* entropy,
                         std::vector<core::parameter::EncodingSet>& params,
                         core::stats::PerfStats& stats, const bool write_raw) {
  const std::string id_desc_prefix = temp_dir + "/id_streams.";
  (void)cp;

  std::vector<core::stats::PerfStats> stat_vec(block_start.size());

  parallel_process_blocks_dynamic(
      block_start, block_end, id_array, file_order_id, name_coder, entropy,
      params, stat_vec, write_raw, id_desc_prefix, cp.num_thr);

  for (const auto& s : stat_vec) {
    stats.Add(s);
  }
}

// Task to process a single block
void process_quality_block_task(
    size_t block_num, const std::vector<uint32_t>& block_start,
    const std::vector<uint32_t>& block_end,
    std::vector<std::string>& quality_array,
    core::ReadEncoder::qv_selector* qv_coder,
    core::ReadEncoder::entropy_selector* entropy,
    std::vector<core::parameter::EncodingSet>& params,
    std::vector<core::stats::PerfStats>& stat_vec, bool write_raw,
    const std::string& quality_desc_prefix, size_t start_block_num) {
  core::record::Chunk chunk;
  for (size_t i = block_start[block_num]; i < block_end[block_num]; i++) {
    chunk.GetData().emplace_back(static_cast<uint8_t>(2),
                                 core::record::ClassType::kClassU, "", "",
                                 static_cast<uint8_t>(0));
    core::record::Segment s(std::string(
        quality_array[i - block_start[start_block_num]].size(), 'N'));
    s.AddQualities(std::move(quality_array[i - block_start[start_block_num]]));
    chunk.GetData().back().AddSegment(std::move(s));
  }

  auto raw_desc = qv_coder->Process(chunk);
  params[block_num].SetQvDepth(std::get<1>(raw_desc).IsEmpty() ? 0 : 1);
  stat_vec[block_num - start_block_num].Add(std::get<2>(raw_desc));
  chunk.GetData().clear();

  if (write_raw && block_num < 10) {
    for (uint16_t i = 0;
         i < static_cast<uint16_t>(std::get<1>(raw_desc).GetSize()); ++i) {
      if (std::get<1>(raw_desc).Get(i).IsEmpty()) {
        continue;
      }
      std::ofstream out_file_stream(
          "raw_stream_" + std::to_string(block_num) + "_" +
          std::to_string(static_cast<uint8_t>(core::GenDesc::kQv)) + "_" +
          std::to_string(static_cast<uint8_t>(i)));
      out_file_stream.write(
          static_cast<char*>(std::get<1>(raw_desc).Get(i).GetData().GetData()),
          static_cast<std::streamsize>(
              std::get<1>(raw_desc).Get(i).GetData().GetRawSize()));
    }
  }

  auto encoded = entropy->Process(std::get<1>(raw_desc));
  stat_vec[block_num - start_block_num].Add(std::get<2>(encoded));
  params[block_num].AddClass(core::record::ClassType::kClassU,
                             std::move(std::get<0>(raw_desc)));
  params[block_num].SetDescriptor(core::GenDesc::kQv,
                                  std::move(std::get<0>(encoded)));
  std::string file_to_save_streams =
      quality_desc_prefix + std::to_string(block_num);
  std::ofstream out(file_to_save_streams, std::ios::binary);
  util::BitWriter bw(out);
  std::get<1>(encoded).Write(bw);
}

void parallel_process_quality_blocks_dynamic(
    const std::vector<uint32_t>& block_start,
    const std::vector<uint32_t>& block_end,
    std::vector<std::string>& quality_array,
    core::ReadEncoder::qv_selector* qv_coder,
    core::ReadEncoder::entropy_selector* entropy,
    std::vector<core::parameter::EncodingSet>& params,
    std::vector<core::stats::PerfStats>& stat_vec, const bool write_raw,
    const std::string& quality_desc_prefix, const size_t start_block_num,
    const size_t end_block_num, const int num_threads) {
  // Create an instance of the DynamicScheduler
  DynamicScheduler scheduler(num_threads);

  // Run the dynamic scheduler with tasks
  scheduler.run(
      end_block_num - start_block_num, [&](const SchedulerInfo& info) {
        const size_t block_num = start_block_num + info.task_id;
        process_quality_block_task(
            block_num, block_start, block_end, quality_array, qv_coder, entropy,
            params, stat_vec, write_raw, quality_desc_prefix, start_block_num);
      });
}

// -----------------------------------------------------------------------------
void ReorderCompressQualityPe(std::string file_quality[2],
                              const std::string& temp_dir,
                              std::vector<std::string>& quality_array,
                              const uint64_t& quality_array_size,
                              const std::vector<uint32_t>& order_array,
                              const std::vector<uint32_t>& block_start,
                              const std::vector<uint32_t>& block_end,
                              const CompressionParams& cp,
                              core::ReadEncoder::qv_selector* qv_coder,
                              core::ReadEncoder::entropy_selector* entropy,
                              std::vector<core::parameter::EncodingSet>& params,
                              core::stats::PerfStats& stats, bool write_raw) {
  const std::string quality_desc_prefix = temp_dir + "/quality_streams.";
  uint32_t start_block_num = 0;
  uint32_t end_block_num = 0;
  while (true) {
    // first find blocks to read from file
    if (start_block_num >= block_start.size()) break;
    end_block_num = start_block_num;
    while (end_block_num < block_end.size()) {
      if (block_end[end_block_num] - block_start[start_block_num] >
          quality_array_size)
        break;
      end_block_num++;
    }
    std::string temp_str;
    for (int j = 0; j < 2; j++) {
      std::ifstream f_in(file_quality[j]);
      UTILS_DIE_IF(!f_in, "Cannot open file to read: " + file_quality[j]);
      uint32_t num_reads_offset = j * (cp.num_reads / 2);
      for (uint32_t i = 0; i < cp.num_reads / 2; i++) {
        std::getline(f_in, temp_str);
        if (order_array[i + num_reads_offset] >= block_start[start_block_num] &&
            order_array[i + num_reads_offset] < block_end[end_block_num - 1])
          quality_array[order_array[i + num_reads_offset] -
                        block_start[start_block_num]] = temp_str;
      }
    }

    std::vector<core::stats::PerfStats> stat_vec(end_block_num -
                                                 start_block_num);

    parallel_process_quality_blocks_dynamic(
        block_start, block_end, quality_array, qv_coder, entropy, params,
        stat_vec, write_raw, quality_desc_prefix, start_block_num,
        end_block_num, cp.num_thr);
    start_block_num = end_block_num;

    for (const auto& s : stat_vec) {
      stats.Add(s);
    }
  }
}

// Task to process a single block
void process_block_task(size_t block_num,
                        uint64_t num_reads_per_block, uint64_t num_reads_bin,
                        uint64_t start_read_bin, const std::string& mode,
                        std::vector<std::string>& str_array,
                        core::ReadEncoder::name_selector* name_coder,
                        core::ReadEncoder::entropy_selector* entropy,
                        std::vector<core::parameter::EncodingSet>& params,
                        std::vector<core::stats::PerfStats>& stat_vec,
                        bool write_raw, const std::string& id_desc_prefix,
                        const std::string& quality_desc_prefix,
                        core::ReadEncoder::qv_selector* qv_coder) {
  uint64_t block_num_offset = start_read_bin / num_reads_per_block;
  uint64_t start_read_num = block_num * num_reads_per_block;
  uint64_t end_read_num = (block_num + 1) * num_reads_per_block;
  if (end_read_num >= num_reads_bin) {
    end_read_num = num_reads_bin;
  }
  auto num_reads_block = static_cast<uint32_t>(end_read_num - start_read_num);

  if (mode == "id") {
    core::record::Chunk chunk;
    for (size_t i = 0; i < num_reads_block; i++) {
      chunk.GetData().emplace_back(static_cast<uint8_t>(1),
                                   core::record::ClassType::kClassU,
                                   std::move(str_array[start_read_num + i]), "",
                                   static_cast<uint8_t>(0));
      core::record::Segment s("N");
      chunk.GetData().back().AddSegment(std::move(s));
    }

    auto name_raw = name_coder->Process(chunk);
    stat_vec[block_num].Add(std::get<1>(name_raw));

    if (write_raw && block_num_offset + block_num < 10) {
      for (uint16_t i = 0;
           i < static_cast<uint16_t>(std::get<0>(name_raw).GetSize()); ++i) {
        if (std::get<0>(name_raw).Get(i).IsEmpty()) {
          continue;
        }
        std::ofstream out_file_stream(
            "raw_stream_" + std::to_string(block_num_offset + block_num) + "_" +
            std::to_string(static_cast<uint8_t>(core::GenDesc::kReadName)) +
            "_" + std::to_string(static_cast<uint8_t>(i)));
        out_file_stream.write(
            static_cast<char*>(
                std::get<0>(name_raw).Get(i).GetData().GetData()),
            static_cast<std::streamsize>(
                std::get<0>(name_raw).Get(i).GetData().GetRawSize()));
      }
    }

    auto encoded = entropy->Process(std::get<0>(name_raw));
    stat_vec[block_num].Add(std::get<2>(encoded));
    params[block_num_offset + block_num].SetDescriptor(
        core::GenDesc::kReadName, std::move(std::get<0>(encoded)));
    std::string file_to_save_streams =
        id_desc_prefix + std::to_string(block_num_offset + block_num);
    std::ofstream out(file_to_save_streams, std::ios::binary);
    util::BitWriter bw(out);
    std::get<1>(encoded).Write(bw);
  } else /* mode == "quality" */ {
    core::record::Chunk chunk;
    for (auto i = static_cast<uint32_t>(start_read_num);
         i < start_read_num + num_reads_block; i++) {
      chunk.GetData().emplace_back(static_cast<uint8_t>(1),
                                   core::record::ClassType::kClassU, "", "",
                                   static_cast<uint8_t>(0));
      core::record::Segment s(std::string(str_array[i].size(), 'N'));
      s.AddQualities(std::move(str_array[i]));
      chunk.GetData().back().AddSegment(std::move(s));
    }
    auto qv_str = qv_coder->Process(chunk);
    stat_vec[block_num].Add(std::get<2>(qv_str));
    params[block_num_offset + block_num].SetQvDepth(
        std::get<1>(qv_str).IsEmpty() ? 0 : 1);

    if (write_raw && block_num_offset + block_num < 10) {
      for (uint16_t i = 0;
           i < static_cast<uint16_t>(std::get<1>(qv_str).GetSize()); ++i) {
        if (std::get<1>(qv_str).Get(i).IsEmpty()) {
          continue;
        }
        std::ofstream out_file_stream(
            "raw_stream_" + std::to_string(block_num_offset + block_num) + "_" +
            std::to_string(static_cast<uint8_t>(core::GenDesc::kQv)) + "_" +
            std::to_string(static_cast<uint8_t>(i)));
        out_file_stream.write(
            static_cast<char*>(std::get<1>(qv_str).Get(i).GetData().GetData()),
            static_cast<std::streamsize>(
                std::get<1>(qv_str).Get(i).GetData().GetRawSize()));
      }
    }

    auto encoded = entropy->Process(std::get<1>(qv_str));
    stat_vec[block_num].Add(std::get<2>(encoded));
    params[block_num_offset + block_num].AddClass(
        core::record::ClassType::kClassU, std::move(std::get<0>(qv_str)));
    params[block_num_offset + block_num].SetDescriptor(
        core::GenDesc::kQv, std::move(std::get<0>(encoded)));
    std::string file_to_save_streams =
        quality_desc_prefix + std::to_string(block_num_offset + block_num);
    std::ofstream out(file_to_save_streams, std::ios::binary);
    util::BitWriter bw(out);
    std::get<1>(encoded).Write(bw);
  }
}

// Function replacing OpenMP loop
void parallel_process_blocks_dynamic(
    const uint64_t blocks, const uint64_t num_reads_per_block,
    const uint64_t num_reads_bin, const uint64_t start_read_bin,
    const std::string& mode, std::vector<std::string>& str_array,
    core::ReadEncoder::name_selector* name_coder,
    core::ReadEncoder::entropy_selector* entropy,
    std::vector<core::parameter::EncodingSet>& params,
    std::vector<core::stats::PerfStats>& stat_vec, const bool write_raw,
    const std::string& id_desc_prefix, const std::string& quality_desc_prefix,
    core::ReadEncoder::qv_selector* qv_coder, const int num_threads) {
  // Create an instance of the DynamicScheduler
  DynamicScheduler scheduler(num_threads);

  // Run the dynamic scheduler with tasks
  scheduler.run(blocks, [&](const SchedulerInfo& info) {
    process_block_task(info.task_id, num_reads_per_block, num_reads_bin,
                       start_read_bin, mode, str_array, name_coder, entropy,
                       params, stat_vec, write_raw, id_desc_prefix,
                       quality_desc_prefix, qv_coder);
  });
}

// -----------------------------------------------------------------------------
void ReorderCompress(const std::string& file_name, const std::string& temp_dir,
                     const uint32_t& num_reads_per_file, const int& num_thr,
                     const uint32_t& num_reads_per_block,
                     std::vector<std::string>& str_array,
                     const uint32_t& str_array_size,
                     const std::vector<uint32_t>& order_array,
                     const std::string& mode,
                     core::ReadEncoder::qv_selector* qv_coder,
                     core::ReadEncoder::name_selector* name_coder,
                     core::ReadEncoder::entropy_selector* entropy,
                     std::vector<core::parameter::EncodingSet>& params,
                     core::stats::PerfStats& stats, bool write_raw) {
  const std::string id_desc_prefix = temp_dir + "/id_streams.";
  const std::string quality_desc_prefix = temp_dir + "/quality_streams.";
  for (uint32_t index = 0; index <= num_reads_per_file / str_array_size;
       index++) {
    uint32_t num_reads_bin = str_array_size;
    if (index == num_reads_per_file / str_array_size)
      num_reads_bin = num_reads_per_file % str_array_size;
    if (num_reads_bin == 0) break;
    uint32_t start_read_bin = index * str_array_size;
    uint32_t end_read_bin = index * str_array_size + num_reads_bin;
    // Read the file and pick up the lines corresponding to this bin
    std::ifstream f_in(file_name);
    std::string temp_str;
    for (uint32_t i = 0; i < num_reads_per_file; i++) {
      std::getline(f_in, temp_str);
      if (order_array[i] >= start_read_bin && order_array[i] < end_read_bin)
        str_array[order_array[i] - start_read_bin] = temp_str;
    }
    f_in.close();
    auto blocks = static_cast<uint64_t>(
        std::ceil(static_cast<float>(num_reads_bin) /
                  static_cast<float>(num_reads_per_block)));

    //
    // According to the execution profile, this is the 2nd hottest
    // parallel region in genie when compressing a normal fastq
    // file (not an analysis run).
    //
    // It is important that you specify a chunk Size of 1 (which
    // becomes the default when schedule=dynamic).  Without it,
    // libgomp effectively serializes a loop with ordered sections.
    //
    // Right now, on a 4 thread run, the 3 worker threads spend
    // ~1/3 of their time idling, waiting on the ordered section.
    // That figure will get worse as we increase the #threads,
    // but better with larger problem sizes.
    //
    // There might be a better way to parallelize the loop.
    //

    std::vector<core::stats::PerfStats> stat_vec(blocks);
    (void)num_thr;
    parallel_process_blocks_dynamic(
        blocks, num_reads_per_block, num_reads_bin, start_read_bin, mode,
        str_array, name_coder, entropy, params, stat_vec, write_raw,
        id_desc_prefix, quality_desc_prefix, qv_coder, num_thr);

    for (const auto& s : stat_vec) {
      stats.Add(s);
    }
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------
