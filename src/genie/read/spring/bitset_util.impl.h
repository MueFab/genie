/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_BITSET_UTIL_IMPL_H_
#define SRC_GENIE_READ_SPRING_BITSET_UTIL_IMPL_H_

// -----------------------------------------------------------------------------

#include <algorithm>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "genie/read/spring/dynamic_scheduler.h"

// -----------------------------------------------------------------------------

namespace genie::read::spring {

// -----------------------------------------------------------------------------
template <size_t BitsetSize>
void StringToBitset(
    const std::string& s, const uint16_t read_length,
    std::bitset<BitsetSize>& b,
    const std::vector<std::vector<std::bitset<BitsetSize>>>& base_mask) {
  for (int i = 0; i < read_length; i++)
    b |= base_mask[i][static_cast<uint8_t>(s[i])];
}

// -----------------------------------------------------------------------------
template <size_t BitsetSize>
void GenerateIndexMasks(std::vector<std::bitset<BitsetSize>>& mask1,
                        std::vector<BbHashDict>& dict, const int num_dict,
                        const int bpb) {
  for (int j = 0; j < num_dict; j++) mask1[j].reset();
  for (int j = 0; j < num_dict; j++)
    for (int i = bpb * dict[j].start_; i < bpb * (dict[j].end_ + 1); i++)
      mask1[j][i] = 1;
}

// -----------------------------------------------------------------------------

// Function to write keys for a specific task (dynamic scheduling replacement
// for OpenMP loop)
inline void write_keys_task_dynamic(const size_t task_id,
                                    const std::vector<uint64_t>& ull,
                                    const BbHashDict& dict,
                                    const std::string& basedir,
                                    const size_t num_threads) {
  const uint64_t num_reads = dict.dict_num_reads_;
  const uint64_t start =
      static_cast<uint64_t>(task_id) * num_reads / num_threads;
  uint64_t stop = (task_id + 1) * num_reads / num_threads;
  if (task_id == num_threads - 1) {
    stop = num_reads;
  }

  std::ofstream file_out_key(basedir + "/keys.bin." + std::to_string(task_id),
                             std::ios::binary);
  for (uint64_t i = start; i < stop; ++i) {
    file_out_key.write(reinterpret_cast<const char*>(&ull[i]),
                       sizeof(uint64_t));
  }
  file_out_key.close();
}

// -----------------------------------------------------------------------------

// Function replacing OpenMP loop
inline void parallel_write_keys_dynamic(const std::vector<uint64_t>& ull,
                                        const BbHashDict& dict,
                                        const std::string& basedir,
                                        const size_t num_threads) {
  // Create an instance of the DynamicScheduler
  DynamicScheduler scheduler(num_threads);

  // Run the dynamic scheduler with tasks
  scheduler.run(num_threads, [&](const SchedulerInfo& info) {
    write_keys_task_dynamic(info.task_id, ull, dict, basedir, num_threads);
  });
}

// -----------------------------------------------------------------------------

// Task to process keys and write hashes
inline void process_keys_task(size_t task_id,
                              const std::vector<BbHashDict>& dict,
                              const std::string& basedir, size_t num_threads,
                              int j) {
  const uint64_t num_reads = dict[j].dict_num_reads_;
  const uint64_t start = task_id * num_reads / num_threads;
  uint64_t stop = (task_id + 1) * num_reads / num_threads;
  if (task_id == num_threads - 1) stop = num_reads;

  std::ifstream file_in_key(basedir + "/keys.bin." + std::to_string(task_id),
                            std::ios::binary);
  if (!file_in_key.is_open()) {
    throw std::runtime_error("Cannot open file to read: " + basedir +
                             "/keys.bin." + std::to_string(task_id));
  }

  std::ofstream file_out_hash(basedir + "/hash.bin." + std::to_string(task_id) +
                                  '.' + std::to_string(j),
                              std::ios::binary);

  uint64_t current_key, current_hash;
  for (uint64_t i = start; i < stop; ++i) {
    file_in_key.read(reinterpret_cast<char*>(&current_key), sizeof(uint64_t));
    current_hash = dict[j].boo_hash_fun_->lookup(current_key);
    file_out_hash.write(reinterpret_cast<char*>(&current_hash),
                        sizeof(uint64_t));
  }

  file_in_key.close();
  std::remove((basedir + "/keys.bin." + std::to_string(task_id)).c_str());
  file_out_hash.close();
}

// -----------------------------------------------------------------------------

inline void parallel_process_keys_dynamic(const std::vector<BbHashDict>& dict,
                                          const std::string& basedir,
                                          const int num_threads, const int j) {
  // Create an instance of the DynamicScheduler
  DynamicScheduler scheduler(num_threads);

  // Run the dynamic scheduler with tasks
  scheduler.run(num_threads, [&](const SchedulerInfo& info) {
    process_keys_task(info.task_id, dict, basedir, num_threads, j);
  });
}

// -----------------------------------------------------------------------------

inline void deduplicate_and_construct_hash(std::vector<uint64_t>& ull,
                                           BbHashDict& dict) {
  // Deduplicate ull
  std::sort(ull.begin(), ull.begin() + dict.dict_num_reads_);
  uint32_t k = 0;
  for (uint32_t i = 1; i < dict.dict_num_reads_; i++) {
    if (ull[i] != ull[k]) {
      ull[++k] = ull[i];
    }
  }
  dict.num_keys_ = k + 1;

  // Construct BooHash
  auto data_iterator =
      boomphf::range(static_cast<const uint64_t*>(ull.data()),
                     static_cast<const uint64_t*>(ull.data() + dict.num_keys_));
  double gamma_factor = 5.0;  // Balance between speed and memory
  dict.boo_hash_fun_ = std::make_unique<BooHashFunType>(
      dict.num_keys_, data_iterator, /*num_thr*/ 1, gamma_factor, true, false);
}

// -----------------------------------------------------------------------------

inline void filter_keys_by_read_length(
    const std::vector<uint16_t>& read_lengths, std::vector<uint64_t>& ull,
    BbHashDict& dict, const uint32_t num_reads) {
  dict.dict_num_reads_ = 0;  // Reset the count of reads for the dictionary
  for (uint32_t i = 0; i < num_reads; i++) {
    if (read_lengths[i] > dict.end_) {
      ull[dict.dict_num_reads_] = ull[i];  // Retain the key
      dict.dict_num_reads_++;
    }
  }
}

// -----------------------------------------------------------------------------

template <size_t BitsetSize>
void compute_keys(const std::vector<std::bitset<BitsetSize>>& read,
                  const std::bitset<BitsetSize>& mask,
                  std::vector<uint64_t>& ull, const BbHashDict& dict,
                  const size_t num_reads, const int bpb) {
  std::bitset<BitsetSize> b;
  // Compute keys and store in ull
  for (uint64_t i = 0; i < num_reads; i++) {
    b = read[i] & mask;
    ull[i] = (b >> (bpb * dict.start_)).to_ullong();  // NOLINT
  }
}

// -----------------------------------------------------------------------------

inline void process_dict_task(size_t task_id, std::vector<BbHashDict>& dict,
                              const std::string& basedir,
                              const std::vector<uint16_t>& read_lengths,
                              int num_threads) {
  int j = static_cast<int>(task_id);

  // Step 1: Fill start_pos_ by first storing numbers and then doing cumulative
  // sum
  dict[j].start_pos_ = std::vector<uint32_t>(dict[j].num_keys_ + 1);
  uint64_t current_hash;
  for (int tid = 0; tid < num_threads; tid++) {
    std::ifstream fin_hash(
        basedir + "/hash.bin." + std::to_string(tid) + '.' + std::to_string(j),
        std::ios::binary);
    if (!fin_hash.is_open()) {
      throw std::runtime_error("Cannot open file to read: " + basedir +
                               "/hash.bin." + std::to_string(tid) + '.' +
                               std::to_string(j));
    }
    while (fin_hash.read(reinterpret_cast<char*>(&current_hash),
                         sizeof(uint64_t))) {
      dict[j].start_pos_[current_hash + 1]++;
    }
    fin_hash.close();
  }

  dict[j].empty_bin_ = std::vector<uint8_t>(dict[j].num_keys_);
  for (uint32_t i = 1; i < dict[j].num_keys_; i++) {
    dict[j].start_pos_[i] += dict[j].start_pos_[i - 1];
  }

  // Step 2: Insert elements in the dict array
  dict[j].read_id_ = std::vector<uint32_t>(dict[j].dict_num_reads_);
  uint32_t i = 0;
  for (int tid = 0; tid < num_threads; tid++) {
    std::ifstream fin_hash(
        basedir + "/hash.bin." + std::to_string(tid) + '.' + std::to_string(j),
        std::ios::binary);
    if (!fin_hash.is_open()) {
      throw std::runtime_error("Cannot open file to read: " + basedir +
                               "/hash.bin." + std::to_string(tid) + '.' +
                               std::to_string(j));
    }
    while (fin_hash.read(reinterpret_cast<char*>(&current_hash),
                         sizeof(uint64_t))) {
      while (read_lengths[i] <= dict[j].end_) i++;
      dict[j].read_id_[dict[j].start_pos_[current_hash]++] = i;
      i++;
    }
    fin_hash.close();
    std::remove(
        (basedir + "/hash.bin." + std::to_string(tid) + '.' + std::to_string(j))
            .c_str());
  }

  // Step 3: Correcting start_pos array modified during insertion
  for (int64_t key_num = dict[j].num_keys_; key_num >= 1; key_num--) {
    dict[j].start_pos_[key_num] = dict[j].start_pos_[key_num - 1];
  }
  dict[j].start_pos_[0] = 0;
}

// -----------------------------------------------------------------------------

inline void parallel_process_dicts_dynamic(
    std::vector<BbHashDict>& dict, const std::string& basedir,
    const std::vector<uint16_t>& read_lengths, const int num_threads,
    const int num_dict) {
  // Create an instance of the DynamicScheduler
  DynamicScheduler scheduler(std::min(num_dict, num_threads));

  // Run the dynamic scheduler with tasks
  scheduler.run(num_dict, [&](const SchedulerInfo& info) {
    process_dict_task(info.task_id, dict, basedir, read_lengths, num_threads);
  });
}

// -----------------------------------------------------------------------------

template <size_t BitsetSize>
void ConstructDictionary(const std::vector<std::bitset<BitsetSize>>& read,
                         std::vector<BbHashDict>& dict,
                         const std::vector<uint16_t>& read_lengths,
                         const int num_dict, const uint32_t& num_reads,
                         const int bpb, const std::string& basedir,
                         const int& num_threads) {
  auto mask = std::vector<std::bitset<BitsetSize>>(num_dict);
  GenerateIndexMasks<BitsetSize>(mask, dict, num_dict, bpb);
  for (int j = 0; j < num_dict; j++) {
    auto ull = std::vector<uint64_t>(num_reads);
    compute_keys(read, mask[j], ull, dict[j], num_reads, bpb);
    filter_keys_by_read_length(read_lengths, ull, dict[j], num_reads);
    parallel_write_keys_dynamic(ull, dict[j], basedir, num_threads);
    deduplicate_and_construct_hash(ull, dict[j]);
    parallel_process_keys_dynamic(dict, basedir, num_threads, j);
  }
  parallel_process_dicts_dynamic(dict, basedir, read_lengths, num_threads,
                                 num_dict);
}

// -----------------------------------------------------------------------------
template <size_t BitsetSize>
void GenerateMasks(std::vector<std::vector<std::bitset<BitsetSize>>>& mask,
                   const int max_read_len, const int bpb) {
  // mask for zeroing the end bits (needed while reordering to compute Hamming
  // distance between shifted reads)
  for (int i = 0; i < max_read_len; i++) {
    for (int j = 0; j < max_read_len; j++) {
      mask[i][j].reset();
      for (int k = bpb * i; k < bpb * max_read_len - bpb * j; k++)
        mask[i][j][k] = 1;
    }
  }
}

// -----------------------------------------------------------------------------

template <size_t BitsetSize>
void CharToBitset(
    const char* s, const int read_len, std::bitset<BitsetSize>& b,
    const std::vector<std::vector<std::bitset<BitsetSize>>>& base_mask) {
  b.reset();
  for (int i = 0; i < read_len; i++)
    b |= base_mask[i][static_cast<uint8_t>(s[i])];
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_BITSET_UTIL_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
