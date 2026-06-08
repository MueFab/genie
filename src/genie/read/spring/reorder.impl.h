/**
 * Copyright 2018-2024 The Genie Authors.
 * @file reorder.impl.h
 *
 * @brief Implementation of the spring reorder functionality
 * for the Genie project.
 *
 * This file contains the implementation of various functions and templates
 * used for reordering and compressing DNA read sequences in the Genie project.
 * It includes functions for reading DNA files, updating reference counts,
 * searching for matches, and writing reordered sequences to files.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_REORDER_IMPL_H_
#define SRC_GENIE_READ_SPRING_REORDER_IMPL_H_

// -----------------------------------------------------------------------------

#include <algorithm>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <string>
#include <vector>

#include "genie/read/spring/bitset_util.h"
#include "genie/util/barrier.h"
#include "genie/util/literal.h"
#include "genie/util/log.h"
#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::read::spring {

using util::operator""_u32;

// -----------------------------------------------------------------------------

template <size_t BitsetSize>
ReorderGlobal<BitsetSize>::ReorderGlobal(const int max_read_len_param) {
  base_mask =
      std::vector<std::vector<std::bitset<BitsetSize>>>(max_read_len_param);
  for (int i = 0; i < max_read_len_param; i++)
    base_mask[i] = std::vector<std::bitset<BitsetSize>>(128);
}

// -----------------------------------------------------------------------------

template <size_t BitsetSize>
void BitsetToString(std::bitset<BitsetSize> b, char* s, const uint16_t read_len,
                    const ReorderGlobal<BitsetSize>& rg) {
  // destroys bitset b
  static constexpr char rev_int_to_char[4] = {'A', 'G', 'C', 'T'};
  for (int i = 0; i < 2 * read_len / 64 + 1; i++) {
    uint64_t ull = (b & rg.mask64).to_ullong();
    b >>= 64;
    for (int j = 32 * i; j < 32 * i + 32 && j < read_len; j++) {
      s[j] = rev_int_to_char[ull % 4];
      ull /= 4;
    }
  }
  s[read_len] = '\0';
}

// -----------------------------------------------------------------------------

template <size_t BitsetSize>
void SetGlobalArrays(ReorderGlobal<BitsetSize>& rg) {
  for (int i = 0; i < 64; i++) rg.mask64[i] = 1;
  for (int i = 0; i < rg.max_read_len; i++) {
    rg.base_mask[i][static_cast<uint8_t>('A')][2 * i] = 0;
    rg.base_mask[i][static_cast<uint8_t>('A')][2 * i + 1] = 0;
    rg.base_mask[i][static_cast<uint8_t>('C')][2 * i] = 0;
    rg.base_mask[i][static_cast<uint8_t>('C')][2 * i + 1] = 1;
    rg.base_mask[i][static_cast<uint8_t>('G')][2 * i] = 1;
    rg.base_mask[i][static_cast<uint8_t>('G')][2 * i + 1] = 0;
    rg.base_mask[i][static_cast<uint8_t>('T')][2 * i] = 1;
    rg.base_mask[i][static_cast<uint8_t>('T')][2 * i + 1] = 1;
  }
}

// -----------------------------------------------------------------------------

template <size_t BitsetSize>
void UpdateRefCount(std::bitset<BitsetSize>& cur, std::bitset<BitsetSize>& ref,
                    std::bitset<BitsetSize>& rev_ref,
                    std::array<std::vector<int>, 4>& count,
                    const bool reset_count, const bool rev, const int shift,
                    const uint16_t cur_read_len, int& ref_len,
                    const ReorderGlobal<BitsetSize>& rg) {
  // for var length, shift represents shift of start positions, if read length
  // is small, may not need to shift actually
  static constexpr char int_to_char[4] = {'A', 'C', 'T', 'G'};
  auto char_to_int = [](const uint8_t a) {
    return (a & 0x06) >> 1;
  };  // inverse of above
  char s[kMaxReadLen + 1], *current;
  BitsetToString<BitsetSize>(cur, s, cur_read_len, rg);
  if (!rev) {
    current = s;
  } else {
    char s1[kMaxReadLen + 1];
    ReverseComplement(s, s1, cur_read_len);
    current = s1;
  }

  if (reset_count) {  // reset_count - unmatched read so start over
    std::fill(count[0].begin(), count[0].begin() + rg.max_read_len, 0);
    std::fill(count[1].begin(), count[1].begin() + rg.max_read_len, 0);
    std::fill(count[2].begin(), count[2].begin() + rg.max_read_len, 0);
    std::fill(count[3].begin(), count[3].begin() + rg.max_read_len, 0);
    for (int i = 0; i < cur_read_len; i++) {
      count[char_to_int(static_cast<uint8_t>(current[i]))][i] = 1;
    }
    ref_len = cur_read_len;
  } else {
    if (!rev) {
      // shift count
      for (int i = 0; i < ref_len - shift; i++) {
        for (int j = 0; j < 4; j++) count[j][i] = count[j][i + shift];
        if (i < cur_read_len)
          count[char_to_int(static_cast<uint8_t>(current[i]))][i] += 1;
      }

      // for the new positions set count to 1
      for (int i = ref_len - shift; i < cur_read_len; i++) {
        for (int j = 0; j < 4; j++) count[j][i] = 0;
        count[char_to_int(static_cast<uint8_t>(current[i]))][i] = 1;
      }
      ref_len = std::max<int>(ref_len - shift, cur_read_len);
    } else {  // reverse case is quite complicated in the variable length
              // case
      if (cur_read_len - shift >= ref_len) {
        for (int i = cur_read_len - shift - ref_len; i < cur_read_len - shift;
             i++) {
          for (int j = 0; j < 4; j++)
            count[j][i] = count[j][i - (cur_read_len - shift - ref_len)];
          count[char_to_int(static_cast<uint8_t>(current[i]))][i] += 1;
        }
        for (int i = 0; i < cur_read_len - shift - ref_len; i++) {
          for (int j = 0; j < 4; j++) count[j][i] = 0;
          count[char_to_int(static_cast<uint8_t>(current[i]))][i] = 1;
        }
        for (int i = cur_read_len - shift; i < cur_read_len; i++) {
          for (int j = 0; j < 4; j++) count[j][i] = 0;
          count[char_to_int(static_cast<uint8_t>(current[i]))][i] = 1;
        }
        ref_len = cur_read_len;
      } else if (ref_len + shift <= rg.max_read_len) {
        for (int i = ref_len - cur_read_len + shift; i < ref_len; i++)
          count[char_to_int(static_cast<uint8_t>(
              current[i - (ref_len - cur_read_len + shift)]))][i] += 1;
        for (int i = ref_len; i < ref_len + shift; i++) {
          for (int j = 0; j < 4; j++) count[j][i] = 0;
          count[char_to_int(static_cast<uint8_t>(
              current[i - (ref_len - cur_read_len + shift)]))][i] = 1;
        }
        ref_len = ref_len + shift;
      } else {
        for (int i = 0; i < rg.max_read_len - shift; i++) {
          for (int j = 0; j < 4; j++)
            count[j][i] = count[j][i + (ref_len + shift - rg.max_read_len)];
        }
        for (int i = rg.max_read_len - cur_read_len;
             i < rg.max_read_len - shift; ++i) {
          count[char_to_int(static_cast<uint8_t>(
              current[i - (rg.max_read_len - cur_read_len)]))][i] += 1;
        }
        for (int i = rg.max_read_len - shift; i < rg.max_read_len; ++i) {
          for (int j = 0; j < 4; j++) count[j][i] = 0;
          count[char_to_int(static_cast<uint8_t>(
              current[i - (rg.max_read_len - cur_read_len)]))][i] = 1;
        }
        ref_len = rg.max_read_len;
      }
    }
    // find max of each position to get ref

    for (int i = 0; i < ref_len; i++) {
      int max = 0, ind_max = 0;
      for (int j = 0; j < 4; j++)
        if (count[j][i] > max) {
          max = count[j][i];
          ind_max = j;
        }
      current[i] = int_to_char[ind_max];
    }
  }
  ref = CharToBitset<BitsetSize>(current, ref_len, rg.base_mask);
  char rev_current[kMaxReadLen + 1];
  ReverseComplement(current, rev_current, ref_len);
  rev_ref = CharToBitset<BitsetSize>(rev_current, ref_len, rg.base_mask);
}

// -----------------------------------------------------------------------------

template <size_t BitsetSize>
void ReadDnaFile(std::vector<std::bitset<BitsetSize>>& read,
                 std::vector<uint16_t>& read_lengths,
                 const ReorderGlobal<BitsetSize>& rg) {
  std::ifstream f(rg.infile[0], std::ifstream::in | std::ios::binary);
  UTILS_DIE_IF(!f, "Cannot open file to read: " + rg.infile[0]);
  for (uint32_t i = 0; i < rg.num_reads_array[0]; i++) {
    f.read(reinterpret_cast<char*>(&read_lengths[i]), sizeof(uint16_t));
    const uint16_t num_bytes_to_read =
        (static_cast<uint32_t>(read_lengths[i]) + 4 - 1) / 4;
    f.read(reinterpret_cast<char*>(&read[i]), num_bytes_to_read);
  }
  f.close();
  remove(rg.infile[0].c_str());
  if (rg.paired_end) {
    f.open(rg.infile[1], std::ifstream::in | std::ios::binary);
    UTILS_DIE_IF(!f, "Cannot open file to read: " + rg.infile[1]);
    for (uint32_t i = rg.num_reads_array[0];
         i < rg.num_reads_array[0] + rg.num_reads_array[1]; ++i) {
      f.read(reinterpret_cast<char*>(&read_lengths[i]), sizeof(uint16_t));
      const uint16_t num_bytes_to_read =
          (static_cast<uint32_t>(read_lengths[i]) + 4 - 1) / 4;
      f.read(reinterpret_cast<char*>(&read[i]), num_bytes_to_read);
    }
    f.close();
    remove(rg.infile[1].c_str());
  }
}

void display_progress(uint32_t& num_reads_remaining, float& last_progress,
                      const std::vector<uint8_t>& remaining_reads,
                      uint32_t& local_reads_used) {
  static std::mutex mutex;
  std::lock_guard lock(mutex);
  num_reads_remaining -= local_reads_used;
  local_reads_used = 0;
  const float progress = 1.0 - (static_cast<float>(num_reads_remaining) /
                                static_cast<float>(remaining_reads.size()));
  while (progress - last_progress > 0.01) {
    constexpr auto kLogModuleName = "Spring";
    UTILS_LOG(util::Logger::Severity::INFO,
              "-------- Progress: " +
                  std::to_string(static_cast<int>(last_progress * 100)) +
                  "% - " + std::to_string(num_reads_remaining) +
                  " reads remaining");
    last_progress += 0.01;
  }
}

// -----------------------------------------------------------------------------

template <size_t BitsetSize>
bool SearchMatch(
    const std::bitset<BitsetSize>& ref,
    const std::vector<std::bitset<BitsetSize>>& mask1,
                 std::vector<std::mutex>& dict_lock,
                 std::vector<std::mutex>& read_lock,
                 std::vector<std::vector<std::bitset<BitsetSize>>>& mask,
                 std::vector<uint16_t>& read_lengths,
                 std::vector<uint8_t>& remaining_reads,
                 std::vector<std::bitset<BitsetSize>>& read,
                 std::vector<BbHashDict>& dict, uint32_t& k, const bool rev,
                 const int shift, const int& ref_len,
                 const ReorderGlobal<BitsetSize>& rg,
                 uint32_t& num_reads_remaining, float& last_progress,
                 uint32_t& local_reads_used) {
  static constexpr unsigned int thresh = kThreshReorder;
  constexpr int max_search = kMaxSearchReorder;
  std::bitset<BitsetSize> b;
  // in the dict read_id array
  // index in start_pos
  bool flag = false;
  for (int l = 0; l < rg.num_dict; l++) {
    int64_t dict_index[2];
    // check if dictionary index is within bound
    if (!rev) {
      if (dict[l].end_ + shift >= ref_len) continue;
    } else {
      if (dict[l].end_ >= ref_len + shift || dict[l].start_ <= shift) continue;
    }
    b = ref & mask1[l];
    const uint64_t ull = (b >> 2 * dict[l].start_).to_ullong();
    uint64_t start_pos_idx = dict[l].boo_hash_fun_->lookup(ull);
    if (start_pos_idx >= dict[l].num_keys_)  // not found
      continue;
    // check if any other thread is modifying same dict pos
    std::lock_guard dict_lock_guard(dict_lock[ReorderLockIdx(start_pos_idx)]);
    dict[l].FindPos(dict_index, start_pos_idx);
    if (dict[l].empty_bin_[start_pos_idx]) {  // bin is empty
      continue;
    }
    const uint64_t ull1 = ((read[dict[l].read_id_[dict_index[0]]] & mask1[l]) >>
                           2 * dict[l].start_)
                              .to_ullong();
    if (ull == ull1) {  // checking if ull is actually the key for this bin
      for (int64_t i = dict_index[1] - 1;
           i >= dict_index[0] && i >= dict_index[1] - max_search; i--) {
        auto rid = dict[l].read_id_[i];
        size_t hamming;
        if (!rev) {
          hamming = ((ref ^ read[rid]) &
                     mask[0][rg.max_read_len -
                             std::min<int>(ref_len - shift, read_lengths[rid])])
                        .count();
        } else {
          hamming =
              ((ref ^ read[rid]) &
               mask[shift][rg.max_read_len -
                           std::min<int>(ref_len + shift, read_lengths[rid])])
                  .count();
        }
        if (hamming <= thresh) {
          std::lock_guard read_lock_guard(read_lock[ReorderLockIdx(rid)]);
          if (remaining_reads[rid]) {
            remaining_reads[rid] = false;
            local_reads_used++;
            if (local_reads_used > 10000) {
              display_progress(num_reads_remaining, last_progress,
                               remaining_reads, local_reads_used);
            }
            k = rid;
            flag = true;
          }
          if (flag == 1) break;
        }
      }
    }
    if (flag == 1) break;
  }
  return flag;
}

template <size_t BitsetSize>
void process_read_task(uint32_t tid, const ReorderGlobal<BitsetSize>& rg,
                       std::vector<std::bitset<BitsetSize>>& read,
                       std::vector<uint16_t>& read_lengths,
                       std::vector<uint8_t>& remaining_reads,
                       std::vector<uint32_t>& unmatched,
                       std::vector<BbHashDict>& dict,
                       std::vector<std::bitset<BitsetSize>>& mask1,
                       std::vector<std::vector<std::bitset<BitsetSize>>>& mask,
                       std::vector<std::mutex>& dict_lock,
                       std::vector<std::mutex>& read_lock, std::mutex& mutex,
                       uint32_t& first_read, util::Barrier& barrier,
                       uint32_t& num_reads_remaining, float& last_progress) {
  // Thread-specific file streams
  std::string tid_str = std::to_string(tid);

  std::ofstream file_out_reverse_comp(rg.outfile_rc + '.' + tid_str,
                                      std::ofstream::out);
  std::ofstream file_out_flags(rg.outfile_flag + '.' + tid_str,
                               std::ofstream::out);
  std::ofstream file_out_positions(rg.outfile_pos + '.' + tid_str,
                                   std::ofstream::out | std::ios::binary);
  std::ofstream file_out_order(rg.outfile_order + '.' + tid_str,
                               std::ofstream::out | std::ios::binary);
  std::ofstream file_out_order_singleton(
      rg.outfile_order + ".singleton." + tid_str,
      std::ofstream::out | std::ios::binary);
  std::ofstream file_out_lengths(rg.outfile_read_length + '.' + tid_str,
                                 std::ofstream::out | std::ios::binary);
  unmatched[tid] = 0;
  std::bitset<BitsetSize> ref, reverse_reference, b;

  uint32_t local_reads_used = 0;

  int64_t first_rid = 0;
  // first_rid represents first read of contig, used for left searching

  // variables for early stopping
  bool stop_searching = false;
  uint32_t num_reads_thr = 0;
  uint32_t num_unmatched_past_1_m_thr = 0;

  auto count = std::array<std::vector<int>, 4>();
  for (int i = 0; i < 4; i++) count[i] = std::vector<int>(rg.max_read_len);
  // in the dict read_id array
  uint64_t start_pos_index;  // index in start position
  bool flag = false, done = false, prev_unmatched = false,
       left_search_start = false, left_search = false;
  // left_search_start - true when left search is starting (to avoid
  // double deletion of first read) left_search - true during left search
  // - needed so that we know when to pick arbitrary read
  int64_t current, prev;
  uint64_t ull;
  int ref_len;
  int64_t ref_pos = 0, cur_read_pos = 0;
  // starting positions of the ref and the current read in the contig, can
  // be negative during left search or due to RC useful for sorting
  // according to starting position in the encoding stage.

  int64_t remaining_pos =
      rg.num_reads -
      1;  // used for searching next unmatched read when no match is found

  // Critical Setup ----------------------------------------------------------
  {
    std::lock_guard lock(mutex);
    // doing initial setup and first read
    current = first_read;
    // some fix below to make sure no errors occurs when we have very
    // few reads (comparable to num_threads). basically if read already
    // taken, this thread just gives up
    if (rg.num_reads == 0 || remaining_reads[current] == 0) {
      done = true;
    } else {
      remaining_reads[current] = false;
      local_reads_used++;
      if (local_reads_used > 10000) {
        display_progress(num_reads_remaining, last_progress,
                         remaining_reads, local_reads_used);
      }
      ++unmatched[tid];
    }
    first_read += rg.num_reads / rg.num_thr;  // spread out first read equally
  }

  // Finish setup with barrier ------------------------------------------------
  barrier.wait();

  if (!done) {
    UpdateRefCount<BitsetSize>(read[current], ref, reverse_reference, count,
                               true, false, 0, read_lengths[current], ref_len,
                               rg);
    cur_read_pos = 0;
    ref_pos = 0;
    first_rid = current;
    prev_unmatched = true;
    prev = current;
  }

  while (!done) {
    if (num_reads_thr % 1000000 == 0) {
      if (static_cast<float>(num_unmatched_past_1_m_thr) >
          kStopCriteriaReorder * 1000000) {
        stop_searching = true;
      }
      num_unmatched_past_1_m_thr = 0;
    }
    num_reads_thr++;

    // delete the read from the corresponding dictionary bins (unless we
    // are starting left search)
    if (!left_search_start) {
      for (int l = 0; l < rg.num_dict; l++) {
        int64_t dict_idx[2];
        if (read_lengths[current] <= dict[l].end_) continue;
        b = read[current] & mask1[l];
        ull = (b >> 2 * dict[l].start_).to_ullong();
        start_pos_index = dict[l].boo_hash_fun_->lookup(ull);
        // check if any other thread is modifying same dict position
        std::lock_guard dict_lock_guard(
            dict_lock[ReorderLockIdx(start_pos_index)]);
        dict[l].FindPos(dict_idx, start_pos_index);
        dict[l].Remove(dict_idx, start_pos_index, current);
      }
    } else {
      left_search_start = false;
    }
    flag = false;
    if (!stop_searching) {
      for (int shift = 0; shift < rg.max_shift; shift++) {
        uint32_t k;
        // find forward match
        flag = SearchMatch<BitsetSize>(
            ref, mask1, dict_lock, read_lock, mask, read_lengths,
            remaining_reads, read, dict, k, false, shift, ref_len, rg,
            num_reads_remaining, last_progress, local_reads_used);
        if (flag == 1) {
          current = k;
          int ref_len_old = ref_len;
          UpdateRefCount<BitsetSize>(read[current], ref, reverse_reference,
                                     count, false, false, shift,
                                     read_lengths[current], ref_len, rg);
          if (!left_search) {
            cur_read_pos = ref_pos + shift;
            ref_pos = cur_read_pos;
          } else {
            cur_read_pos =
                ref_pos + ref_len_old - shift - read_lengths[current];
            ref_pos = ref_pos + ref_len_old - shift - ref_len;
          }
          if (prev_unmatched ==
              true) {  // prev read not singleton, write it now
            file_out_reverse_comp << 'd';
            file_out_order.write(reinterpret_cast<char*>(&prev),
                                 sizeof(uint32_t));
            file_out_flags << 0;  // for unmatched
            int64_t zero = 0;
            file_out_positions.write(reinterpret_cast<char*>(&zero),
                                     sizeof(int64_t));
            file_out_lengths.write(reinterpret_cast<char*>(&read_lengths[prev]),
                                   sizeof(uint16_t));
          }
          file_out_reverse_comp << (left_search ? 'r' : 'd');
          file_out_order.write(reinterpret_cast<char*>(&current),
                               sizeof(uint32_t));
          file_out_flags << 1;  // for matched
          file_out_positions.write(reinterpret_cast<char*>(&cur_read_pos),
                                   sizeof(int64_t));
          file_out_lengths.write(
              reinterpret_cast<char*>(&read_lengths[current]),
              sizeof(uint16_t));

          prev_unmatched = false;
          break;
        }

        // find reverse match
        flag = SearchMatch<BitsetSize>(
            reverse_reference, mask1, dict_lock, read_lock, mask, read_lengths,
            remaining_reads, read, dict, k, true, shift, ref_len, rg,
            num_reads_remaining, last_progress, local_reads_used);

        if (flag == 1) {
          current = k;
          int ref_len_old = ref_len;
          UpdateRefCount<BitsetSize>(read[current], ref, reverse_reference,
                                     count, false, true, shift,
                                     read_lengths[current], ref_len, rg);
          if (!left_search) {
            cur_read_pos =
                ref_pos + ref_len_old + shift - read_lengths[current];
            ref_pos = ref_pos + ref_len_old + shift - ref_len;
          } else {
            cur_read_pos = ref_pos - shift;
            ref_pos = cur_read_pos;
          }
          if (prev_unmatched ==
              true) {  // prev read not singleton, write it now
            file_out_reverse_comp << 'd';
            file_out_order.write(reinterpret_cast<char*>(&prev),
                                 sizeof(uint32_t));
            file_out_flags << 0;  // for unmatched
            int64_t zero = 0;
            file_out_positions.write(reinterpret_cast<char*>(&zero),
                                     sizeof(int64_t));
            file_out_lengths.write(reinterpret_cast<char*>(&read_lengths[prev]),
                                   sizeof(uint16_t));
          }
          file_out_reverse_comp << (left_search ? 'd' : 'r');
          file_out_order.write(reinterpret_cast<char*>(&current),
                               sizeof(uint32_t));
          file_out_flags << 1;  // for matched
          file_out_positions.write(reinterpret_cast<char*>(&cur_read_pos),
                                   sizeof(int64_t));
          file_out_lengths.write(
              reinterpret_cast<char*>(&read_lengths[current]),
              sizeof(uint16_t));

          prev_unmatched = false;
          break;
        }

        reverse_reference <<= 2;
        ref >>= 2;
      }
    }
    if (flag == 0) {
      // no match found
      num_unmatched_past_1_m_thr++;
      if (!left_search) {  // start left search
        left_search = true;
        left_search_start = true;
        // update ref and count with RC of first_read
        UpdateRefCount<BitsetSize>(read[first_rid], ref, reverse_reference,
                                   count, true, true, 0,
                                   read_lengths[first_rid], ref_len, rg);
        ref_pos = 0;
        cur_read_pos = 0;
      } else {  // left search done, now pick arbitrary read and start
                // new contig
        left_search = false;
        for (int64_t j = remaining_pos; j >= 0; --j) {
          if (remaining_reads[j] == 1) {
            std::lock_guard read_lock_guard(read_lock[ReorderLockIdx(j)]);
            if (remaining_reads[j]) {  // checking again inside
                                       // critical block
              current = j;
              remaining_pos = j - 1;
              remaining_reads[j] = false;
              local_reads_used++;
              if (local_reads_used > 10000) {
                display_progress(num_reads_remaining, last_progress,
                                 remaining_reads, local_reads_used);
              }
              flag = true;
              ++unmatched[tid];
            }
            if (flag == 1) break;
          }
        }
        if (flag == 0) {
          if (prev_unmatched ==
              true) {  // last read was singleton, write it now
            file_out_order_singleton.write(reinterpret_cast<char*>(&prev),
                                           sizeof(uint32_t));
          }
          done = true;  // no reads left
        } else {
          UpdateRefCount<BitsetSize>(read[current], ref, reverse_reference,
                                     count, true, false, 0,
                                     read_lengths[current], ref_len, rg);
          ref_pos = 0;
          cur_read_pos = 0;
          if (prev_unmatched == true) {  // prev read singleton, write it now
            file_out_order_singleton.write(reinterpret_cast<char*>(&prev),
                                           sizeof(uint32_t));
          }
          prev_unmatched = true;
          first_rid = current;
          prev = current;
        }
      }
    }
  }  // while (!done) end

  file_out_reverse_comp.close();
  file_out_order.close();
  file_out_flags.close();
  file_out_positions.close();
  file_out_order_singleton.close();
  file_out_lengths.close();
}

template <size_t BitsetSize>
void parallel_process_reads(
    const ReorderGlobal<BitsetSize>& rg,
    std::vector<std::bitset<BitsetSize>>& read,
    std::vector<uint16_t>& read_lengths, std::vector<uint8_t>& remaining_reads,
    std::vector<uint32_t>& unmatched, std::vector<BbHashDict>& dict,
    std::vector<std::bitset<BitsetSize>>& mask1,
    std::vector<std::vector<std::bitset<BitsetSize>>>& mask,
    std::vector<std::mutex>& dict_lock, std::vector<std::mutex>& read_lock) {
  // Create dynamic scheduler
  util::DynamicScheduler scheduler(rg.num_thr);

  std::mutex mutex;
  uint32_t first_read = 0;
  util::Barrier barrier(rg.num_thr);
  uint32_t num_reads_remaining = rg.num_reads;
  float last_progress = 0.0;

  // Dispatch tasks dynamically
  scheduler.run(
      rg.num_thr, [&](const util::DynamicScheduler::SchedulerInfo& info) {
        process_read_task(info.task_id, rg, read, read_lengths, remaining_reads,
                          unmatched, dict, mask1, mask, dict_lock, read_lock,
                          mutex, first_read, barrier, num_reads_remaining,
                          last_progress);
      });
}

// -----------------------------------------------------------------------------

template <size_t BitsetSize>
void Reorder(std::vector<std::bitset<BitsetSize>>& read,
             std::vector<BbHashDict>& dict, std::vector<uint16_t>& read_lengths,
             const ReorderGlobal<BitsetSize>& rg) {
  constexpr uint32_t num_locks = kNumLocksReorder;  // limits on number of locks
                                                    // (power of 2 for fast mod)
  auto dict_lock = std::vector<std::mutex>(num_locks);
  auto read_lock = std::vector<std::mutex>(num_locks);
  auto mask = GenerateMasks<BitsetSize>(rg.max_read_len, 2);
  auto mask1 = std::vector<std::bitset<BitsetSize>>(rg.num_dict);
  GenerateIndexMasks<BitsetSize>(mask1, dict, rg.num_dict, 2);
  auto remaining_reads = std::vector<uint8_t>(rg.num_reads);
  std::fill(remaining_reads.begin(), remaining_reads.begin() + rg.num_reads, 1);

  // we go through remaining_reads array from behind as that speeds up deletion
  // from bin arrays

  //
  // The following parallel region shows up in the execution profile
  // as being the hottest in all genie/spring.  The workload is
  // well-balanced, and it benefits nicely from parallelization.
  //
  // A lot of other loops / regions must be executed with exactly
  // the same # of threads, as they access files created on a
  // per-thread basis (with the tid as part of the name) between
  // parallel regions.
  //
  auto unmatched = std::vector<uint32_t>(rg.num_thr);

  parallel_process_reads<BitsetSize>(rg, read, read_lengths, remaining_reads,
                                     unmatched, dict, mask1, mask, dict_lock,
                                     read_lock);

  auto num_unmatched = static_cast<int>(std::accumulate(
      unmatched.begin(), unmatched.begin() + rg.num_thr, 0_u32));

  constexpr auto kLogModuleName = "Spring";
  UTILS_LOG(util::Logger::Severity::INFO,
            "-------- Unmatched reads: " + std::to_string(num_unmatched));
}

template <size_t BitsetSize>
void process_task(uint32_t tid, const ReorderGlobal<BitsetSize>& rg,
                  const std::vector<std::bitset<BitsetSize>>& read,
                  const std::vector<uint16_t>& read_lengths,
                  std::vector<uint32_t>& num_reads_s_thr) {
  std::string tid_str = std::to_string(tid);
  std::ofstream f_out(rg.outfile + '.' + tid_str,
                      std::ofstream::out | std::ios::binary);
  std::ofstream f_out_s(rg.outfile + ".singleton." + tid_str,
                        std::ofstream::out | std::ios::binary);
  std::ifstream fin_rc(rg.outfile_rc + '.' + tid_str, std::ifstream::in);
  UTILS_DIE_IF(!fin_rc,
               "Cannot open file to read: " + rg.outfile_rc + '.' + tid_str);
  std::ifstream f_in_order(rg.outfile_order + '.' + tid_str,
                           std::ifstream::in | std::ios::binary);
  UTILS_DIE_IF(!f_in_order,
               "Cannot open file to read: " + rg.outfile_order + '.' + tid_str);
  std::ifstream f_in_order_s(rg.outfile_order + ".singleton." + tid_str,
                             std::ifstream::in | std::ios::binary);
  UTILS_DIE_IF(!f_in_order_s, "Cannot open file to read: " + rg.outfile_order +
                                  ".singleton." + tid_str);

  uint32_t current;
  char c;
  // Read character by character
  while (fin_rc >> std::noskipws >> c) {
    f_in_order.read(reinterpret_cast<char*>(&current), sizeof(uint32_t));
    if (c == 'd') {
      uint16_t num_bytes_to_write =
          (static_cast<uint32_t>(read_lengths[current]) + 4 - 1) / 4;
      f_out.write(reinterpret_cast<const char*>(&read_lengths[current]),
                  sizeof(uint16_t));
      f_out.write(reinterpret_cast<const char*>(&read[current]),
                  num_bytes_to_write);
    } else {
      char s1[kMaxReadLen + 1];
      char s[kMaxReadLen + 1];
      BitsetToString<BitsetSize>(read[current], s, read_lengths[current], rg);
      ReverseComplement(s, s1, read_lengths[current]);
      WriteDnaInBits(s1, f_out);
    }
  }

  f_in_order_s.read(reinterpret_cast<char*>(&current), sizeof(uint32_t));
  while (!f_in_order_s.eof()) {
    num_reads_s_thr[tid]++;
    uint16_t num_bytes_to_write =
        (static_cast<uint32_t>(read_lengths[current]) + 4 - 1) / 4;
    f_out_s.write(reinterpret_cast<const char*>(&read_lengths[current]),
                  sizeof(uint16_t));
    f_out_s.write(reinterpret_cast<const char*>(&read[current]),
                  num_bytes_to_write);
    f_in_order_s.read(reinterpret_cast<char*>(&current), sizeof(uint32_t));
  }

  // Close all files
  f_out.close();
  f_out_s.close();
  fin_rc.close();
  f_in_order.close();
  f_in_order_s.close();
}

template <size_t BitsetSize>
void process_all_tasks(const ReorderGlobal<BitsetSize>& rg,
                       const std::vector<std::bitset<BitsetSize>>& read,
                       const std::vector<uint16_t>& read_lengths,
                       std::vector<uint32_t>& num_reads_s_thr) {
  // Create a DynamicScheduler instance
  util::DynamicScheduler scheduler(rg.num_thr);

  // Run the scheduler to process tasks dynamically
  scheduler.run(
      rg.num_thr, [&](const util::DynamicScheduler::SchedulerInfo& info) {
        process_task(info.task_id, rg, read, read_lengths, num_reads_s_thr);
      });
}

// -----------------------------------------------------------------------------

template <size_t BitsetSize>
void WriteToFile(std::vector<std::bitset<BitsetSize>>& read,
                 std::vector<uint16_t>& read_lengths,
                 ReorderGlobal<BitsetSize>& rg) {
  //
  // This loop must be executed in parallel with the same #threads
  // that was used for the hot loop in reorder(), for correctness.
  // It also shows up in the execution profile, and is worth
  // parallelizing anyway.
  std::vector<uint32_t> num_reads_s_thr(rg.num_thr, 0);
  process_all_tasks(rg, read, read_lengths, num_reads_s_thr);

  uint32_t num_reads_s = 0;
  for (int i = 0; i < rg.num_thr; i++) num_reads_s += num_reads_s_thr[i];
  // write num_reads_s to a file
  std::ofstream f_out_s_count(rg.outfile + ".singleton" + ".count",
                              std::ofstream::out | std::ios::binary);
  f_out_s_count.write(reinterpret_cast<char*>(&num_reads_s), sizeof(uint32_t));
  f_out_s_count.close();

  // Now combine the num_thr order files
  std::ofstream file_out_singleton(rg.outfile + ".singleton",
                                   std::ofstream::out | std::ios::binary);
  std::ofstream file_out_order_singleton(rg.outfile_order + ".singleton",
                                         std::ofstream::out | std::ios::binary);
  for (int tid = 0; tid < rg.num_thr; tid++) {
    std::string tid_str = std::to_string(tid);
    std::ifstream fin_s(rg.outfile + ".singleton." + tid_str,
                        std::ifstream::in | std::ios::binary);
    std::ifstream file_input_order_singleton(
        rg.outfile_order + ".singleton." + tid_str,
        std::ifstream::in | std::ios::binary);

    file_out_singleton << fin_s.rdbuf();  // write entire file
    file_out_order_singleton << file_input_order_singleton.rdbuf();

    // clearStreamState error flags which occur on read buffing empty file
    file_out_singleton.clear();
    file_out_order_singleton.clear();

    fin_s.close();
    file_input_order_singleton.close();

    remove((rg.outfile + ".singleton." + tid_str).c_str());
    remove((rg.outfile_order + ".singleton." + tid_str).c_str());
  }
  file_out_singleton.close();
  file_out_order_singleton.close();
}

// -----------------------------------------------------------------------------

template <size_t BitsetSize>
void ReorderMain(const std::string& temp_dir, const CompressionParams& cp) {
  auto rg_pointer =
      std::make_unique<ReorderGlobal<BitsetSize>>(cp.max_read_len);
  ReorderGlobal<BitsetSize>& rg = *rg_pointer;
  rg.basedir = temp_dir;
  rg.infile[0] = rg.basedir + "/input_clean_1.dna";
  rg.infile[1] = rg.basedir + "/input_clean_2.dna";
  rg.outfile = rg.basedir + "/temp.dna";
  rg.outfile_rc = rg.basedir + "/read_rev.txt";
  rg.outfile_flag = rg.basedir + "/temp_flag.txt";
  rg.outfile_pos = rg.basedir + "/temp_pos.txt";
  rg.outfile_order = rg.basedir + "/read_order.bin";
  rg.outfile_read_length = rg.basedir + "/read_lengths.bin";

  rg.max_read_len = cp.max_read_len;
  rg.num_thr = cp.num_thr;
  rg.paired_end = cp.paired_end;
  rg.max_shift = rg.max_read_len / 2;

  rg.num_reads = cp.num_reads_clean[0] + cp.num_reads_clean[1];
  rg.num_reads_array[0] = cp.num_reads_clean[0];
  rg.num_reads_array[1] = cp.num_reads_clean[1];

  SetGlobalArrays(rg);
  auto read = std::vector<std::bitset<BitsetSize>>(rg.num_reads);
  auto read_lengths = std::vector<uint16_t>(rg.num_reads);
  constexpr auto kLogModuleName = "Spring";
  UTILS_LOG(util::Logger::Severity::INFO, "---- Reading DNA file");
  ReadDnaFile<BitsetSize>(read, read_lengths, rg);
  UTILS_LOG(util::Logger::Severity::INFO, "---- Constructing dictionaries");
  DictSizes dict_sizes{};
  if (rg.max_read_len > 100) {
    dict_sizes = {static_cast<uint32_t>(rg.max_read_len / 2 - 32),
                  static_cast<uint32_t>(rg.max_read_len / 2 - 1),
                  static_cast<uint32_t>(rg.max_read_len / 2),
                  static_cast<uint32_t>(rg.max_read_len / 2 - 1 + 32)};
  } else {
    dict_sizes = {
        static_cast<uint32_t>(rg.max_read_len / 2 - rg.max_read_len * 32 / 100),
        static_cast<uint32_t>(rg.max_read_len / 2 - 1),
        static_cast<uint32_t>(rg.max_read_len / 2),
        static_cast<uint32_t>(rg.max_read_len / 2 - 1 +
                              rg.max_read_len * 32 / 100)};
  }
  auto dict = ConstructDictionary<BitsetSize>(read, read_lengths, rg.num_dict,
                                              rg.num_reads, 2, rg.basedir,
                                              rg.num_thr, dict_sizes);
  UTILS_LOG(util::Logger::Severity::INFO, "---- Reordering reads");
  Reorder<BitsetSize>(read, dict, read_lengths, rg);
  UTILS_LOG(util::Logger::Severity::INFO, "---- Writing to file");
  WriteToFile<BitsetSize>(read, read_lengths, rg);
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_REORDER_IMPL_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
