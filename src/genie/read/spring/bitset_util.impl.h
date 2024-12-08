/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_BITSET_UTIL_IMPL_H_
#define SRC_GENIE_READ_SPRING_BITSET_UTIL_IMPL_H_

// -----------------------------------------------------------------------------

#ifdef GENIE_USE_OPENMP
#include <omp.h>
#endif

#include <genie/util/runtime_exception.h>

#include <algorithm>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

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

    //
    // It is not worth putting parallel region here.
    // This loop barely shows up on the execution profile.
    //
#if 0 && GENIE_USE_OPENMP
#pragma omp parallel num_threads(num_threads)
#endif
    {
      std::bitset<BitsetSize> b;
#if 0 && GENIE_USE_OPENMP
            int tid = omp_get_thread_num();
                int num_thr = omp_get_num_threads();
#else
      int tid = 0;
      int num_thr = 1;
#endif
      uint64_t i, stop;
      i = static_cast<uint64_t>(tid) * num_reads / num_thr;
      stop = static_cast<uint64_t>(tid + 1) * num_reads / num_thr;
      if (tid == num_thr - 1) stop = num_reads;
      // compute keys and store in ull
      for (; i < stop; i++) {
        b = read[i] & mask[j];
        ull[i] = (b >> bpb * dict[j].start_).to_ullong();
      }
    }  // parallel end

    // remove keys corresponding to reads shorter than dict_end[j]
    dict[j].dict_num_reads_ = 0;
    for (uint32_t i = 0; i < num_reads; i++) {
      if (read_lengths[i] > dict[j].end_) {
        ull[dict[j].dict_num_reads_] = ull[i];
        dict[j].dict_num_reads_++;
      }
    }

    //
    // write ull to file
    //
    // This region must be executed in parallel with num_threads
    // threads, for correctness.
    //
    // FIXME - make this a parallel for loop that can easily have
    // the OpenMP directive commented out && parallelism disabled.
    //
#if GENIE_USE_OPENMP
#pragma omp parallel num_threads(num_threads) default(none) \
    shared(ull, dict, j, basedir)
#endif
    {
#ifdef GENIE_USE_OPENMP
      int tid = omp_get_thread_num();
      int num_thr = omp_get_num_threads();
#else
      int tid = 0;
      int num_thr = 1;
#endif
      std::ofstream file_out_key(
          basedir + std::string("/keys.bin.") + std::to_string(tid),
          std::ios::binary);
      uint64_t i, stop;
      i = static_cast<uint64_t>(tid) * dict[j].dict_num_reads_ / num_thr;
      stop = static_cast<uint64_t>(tid + 1) * dict[j].dict_num_reads_ / num_thr;
      if (tid == num_thr - 1) stop = dict[j].dict_num_reads_;
      for (; i < stop; i++)
        file_out_key.write(reinterpret_cast<char*>(&ull[i]), sizeof(uint64_t));
      file_out_key.close();
    }  // parallel end

    // deduplicating ull
    std::sort(ull.begin(), ull.begin() + dict[j].dict_num_reads_);
    uint32_t k = 0;
    for (uint32_t i = 1; i < dict[j].dict_num_reads_; i++)
      if (ull[i] != ull[k]) ull[++k] = ull[i];
    dict[j].num_keys_ = k + 1;
    // construct boo hash
    auto data_iterator = boomphf::range(
        static_cast<const uint64_t*>(ull.data()),
        static_cast<const uint64_t*>(ull.data() + dict[j].num_keys_));
    double gamma_factor = 5.0;  // balance between speed and memory
    dict[j].boo_hash_fun_ = std::make_unique<BooHashFunType>(
        dict[j].num_keys_, data_iterator, /*num_thr*/ 1, gamma_factor, true,
        false);

    //
    // compute hashes for all reads
    //
    // This region must be executed in parallel with num_threads
    // threads, for correctness.
    //
    // FIXME - make this a parallel for loop that can easily have
    // the OpenMP directive commented out && parallelism disabled.
    //
    // The boo lookup() call shows up in the execution profile,
    // so there might be a small benefit from it being executed
    // in parallel on large problem sizes.
    //
#ifdef GENIE_USE_OPENMP
#pragma omp parallel num_threads(num_threads) default(none) \
    shared(dict, j, basedir, num_threads, num_dict)
#endif
    {
#ifdef GENIE_USE_OPENMP
      int tid = omp_get_thread_num();
      int num_thr = omp_get_num_threads();
#else
      int tid = 0;
      int num_thr = 1;
#endif
      std::ifstream file_in_key(
          basedir + std::string("/keys.bin.") + std::to_string(tid),
          std::ios::binary);
      UTILS_DIE_IF(!file_in_key, "Cannot open file to read: " + basedir +
                                     std::string("/keys.bin.") +
                                     std::to_string(tid));
      std::ofstream file_out_hash(basedir + std::string("/hash.bin.") +
                                      std::to_string(tid) + '.' +
                                      std::to_string(j),
                                  std::ios::binary);
      uint64_t current_key, current_hash;
      uint64_t i, stop;
      i = static_cast<uint64_t>(tid) * dict[j].dict_num_reads_ / num_thr;
      stop = static_cast<uint64_t>(tid + 1) * dict[j].dict_num_reads_ / num_thr;
      if (tid == num_thr - 1) stop = dict[j].dict_num_reads_;
      for (; i < stop; i++) {
        file_in_key.read(reinterpret_cast<char*>(&current_key),
                         sizeof(uint64_t));
        //
        // the following line shows up on the execution profile
        //
        current_hash = dict[j].boo_hash_fun_->lookup(current_key);
        file_out_hash.write(reinterpret_cast<char*>(&current_hash),
                            sizeof(uint64_t));
      }
      file_in_key.close();
      remove(
          (basedir + std::string("/keys.bin.") + std::to_string(tid)).c_str());
      file_out_hash.close();
    }  // parallel end
  }

  //
  // for rest of the function, use num_dict threads to parallelize
  //
  // This loops must be executed in parallel with num_threads
  // threads, for correctness.
  //
  // When this region shows up in the execution profile, it is always
  // because of a file I/O operation.
  //
  {
#ifdef GENIE_USE_OPENMP
#pragma omp parallel for num_threads(std::min(num_dict, num_threads)) default( \
        none) shared(dict, basedir, read_lengths, num_threads, num_dict)
#endif
    for (int j = 0; j < num_dict; j++) {
      // fill start_pos by first storing numbers and then doing cumulative
      // sum
      dict[j].start_pos_ = std::vector<uint32_t>(
          dict[j].num_keys_ + 1);  // 1 extra to store end pos of last key
      uint64_t current_hash;
      for (int tid = 0; tid < num_threads; tid++) {
        std::ifstream fin_hash(basedir + std::string("/hash.bin.") +
                                   std::to_string(tid) + '.' +
                                   std::to_string(j),
                               std::ios::binary);
        UTILS_DIE_IF(!fin_hash, "Cannot open file to read: " + basedir +
                                    std::string("/hash.bin.") +
                                    std::to_string(tid) + '.' +
                                    std::to_string(j));
        fin_hash.read(reinterpret_cast<char*>(&current_hash), sizeof(uint64_t));
        while (!fin_hash.eof()) {
          dict[j].start_pos_[current_hash + 1]++;
          fin_hash.read(reinterpret_cast<char*>(&current_hash),
                        sizeof(uint64_t));
        }
        fin_hash.close();
      }

      dict[j].empty_bin_ = std::vector<uint8_t>(dict[j].num_keys_);
      for (uint32_t i = 1; i < dict[j].num_keys_; i++)
        dict[j].start_pos_[i] =
            dict[j].start_pos_[i] + dict[j].start_pos_[i - 1];

      // insert elements in the dict array
      dict[j].read_id_ = std::vector<uint32_t>(dict[j].dict_num_reads_);
      uint32_t i = 0;
      for (int tid = 0; tid < num_threads; tid++) {
        std::ifstream fin_hash(basedir + std::string("/hash.bin.") +
                                   std::to_string(tid) + '.' +
                                   std::to_string(j),
                               std::ios::binary);
        UTILS_DIE_IF(!fin_hash, "Cannot open file to read: " + basedir +
                                    std::string("/hash.bin.") +
                                    std::to_string(tid) + '.' +
                                    std::to_string(j));
        fin_hash.read(reinterpret_cast<char*>(&current_hash), sizeof(uint64_t));
        while (!fin_hash.eof()) {
          while (read_lengths[i] <= dict[j].end_) i++;
          dict[j].read_id_[dict[j].start_pos_[current_hash]++] = i;
          i++;
          fin_hash.read(reinterpret_cast<char*>(&current_hash),
                        sizeof(uint64_t));
        }
        fin_hash.close();
        remove((basedir + std::string("/hash.bin.") + std::to_string(tid) +
                '.' + std::to_string(j))
                   .c_str());
      }

      // correcting start_pos array modified during insertion
      for (int64_t key_num = dict[j].num_keys_; key_num >= 1; key_num--)
        dict[j].start_pos_[key_num] = dict[j].start_pos_[key_num - 1];
      dict[j].start_pos_[0] = 0;
    }  // parallel for end
  }
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
