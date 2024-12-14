/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/spring/spring_encoding.h"

#include <array>
#include <bitset>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------

constexpr auto kLogModuleName = "Spring";

namespace genie::read::spring {

// -----------------------------------------------------------------------------
std::string BuildContig(std::list<ContigReads>& current_contig,
                        const uint32_t& list_size) {
  static constexpr char long_to_char[5] = {'A', 'C', 'G', 'T', 'N'};
  static const int64_t char_to_long[128] = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 3, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  };
  if (list_size == 1) return current_contig.front().read;
  auto current_contig_it = current_contig.begin();
  int64_t current_pos = 0, current_size = 0, to_insert;
  std::vector<std::array<int64_t, 4>> count;
  for (; current_contig_it != current_contig.end(); ++current_contig_it) {
    if (current_contig_it == current_contig.begin()) {  // first read
      to_insert = current_contig_it->read_length;
    } else {
      current_pos = current_contig_it->pos;
      if (current_pos + current_contig_it->read_length > current_size)
        to_insert = current_pos + current_contig_it->read_length - current_size;
      else
        to_insert = 0;
    }
    count.insert(count.end(), to_insert, {0, 0, 0, 0});
    current_size = current_size + to_insert;
    for (int64_t i = 0; i < current_contig_it->read_length; i++)
      count[current_pos + i]
           [char_to_long[static_cast<uint8_t>(current_contig_it->read[i])]] +=
          1;
  }
  std::string ref(count.size(), 'A');
  for (size_t i = 0; i < count.size(); i++) {
    int64_t max = 0, index_max = 0;
    for (int64_t j = 0; j < 4; j++)
      if (count[i][j] > max) {
        max = count[i][j];
        index_max = j;
      }
    ref[i] = long_to_char[index_max];
  }
  return ref;
}

// -----------------------------------------------------------------------------
void WriteContig(const std::string& ref, std::list<ContigReads>& current_contig,
                 std::ofstream& f_seq, std::ofstream& f_pos,
                 std::ofstream& f_noise, std::ofstream& f_noise_pos,
                 std::ofstream& f_order, std::ofstream& f_rc,
                 std::ofstream& f_read_length, uint64_t& abs_pos) {
  f_seq << ref;
  uint16_t pos_var;
  int64_t prev_j = 0;
  auto current_contig_it = current_contig.begin();
  uint64_t abs_current_pos;
  for (; current_contig_it != current_contig.end(); ++current_contig_it) {
    const int64_t current_position = current_contig_it->pos;
    prev_j = 0;
    for (int64_t j = 0; j < current_contig_it->read_length; j++)
      if (current_contig_it->read[j] != ref[current_position + j]) {
        f_noise << current_contig_it->read[j];
        pos_var = static_cast<uint16_t>(j - prev_j);
        f_noise_pos.write(reinterpret_cast<char*>(&pos_var), sizeof(uint16_t));
        prev_j = j;
      }
    f_noise << "\n";
    abs_current_pos = abs_pos + current_position;
    f_pos.write(reinterpret_cast<char*>(&abs_current_pos), sizeof(uint64_t));
    f_order.write(reinterpret_cast<char*>(&current_contig_it->order),
                  sizeof(uint32_t));
    f_read_length.write(
        reinterpret_cast<char*>(&current_contig_it->read_length),
        sizeof(uint16_t));
    f_rc << current_contig_it->rc;
  }
  abs_pos += ref.size();
}

// -----------------------------------------------------------------------------
void GetDataParams(EncoderGlobal& eg, const CompressionParams& cp) {
  const uint32_t num_reads_clean =
      cp.num_reads_clean[0] + cp.num_reads_clean[1];
  const uint32_t num_reads_total = cp.num_reads;

  std::ifstream my_file_s_count(eg.infile + ".singleton" + ".count",
                                std::ifstream::in | std::ios::binary);
  UTILS_DIE_IF(!my_file_s_count, "Cannot open file to read: " + eg.infile +
                                     ".singleton" + ".count");
  my_file_s_count.read(reinterpret_cast<char*>(&eg.num_reads_s),
                       sizeof(uint32_t));
  my_file_s_count.close();
  const std::string file_s_count = eg.infile + ".singleton" + ".count";
  remove(file_s_count.c_str());

  eg.num_reads = num_reads_clean - eg.num_reads_s;
  eg.num_reads_n = num_reads_total - num_reads_clean;

  GENIE_LOG(util::Logger::Severity::INFO,
            "---- Maximum Read length: " + std::to_string(cp.max_read_len));
  GENIE_LOG(
      util::Logger::Severity::INFO,
      "---- Number of non-singleton reads: " + std::to_string(eg.num_reads));
  GENIE_LOG(util::Logger::Severity::INFO, "---- Number of singleton reads: " +
                                              std::to_string(eg.num_reads_s));
  GENIE_LOG(util::Logger::Severity::INFO,
            "---- Number of reads with N: " + std::to_string(eg.num_reads_n));
}

// -----------------------------------------------------------------------------
void CorrectOrder(std::vector<uint32_t>& order_s, const EncoderGlobal& eg) {
  uint32_t num_reads_total = eg.num_reads + eg.num_reads_s + eg.num_reads_n;
  auto read_flag_n = std::vector<uint8_t>(num_reads_total);
  // bool array indicating N reads
  for (uint32_t i = 0; i < eg.num_reads_n; i++) {
    read_flag_n[order_s[eg.num_reads_s + i]] = true;
  }

  auto cumulative_n_reads =
      std::vector<uint32_t>(eg.num_reads + eg.num_reads_s);
  // number of reads occurring before pos in clean reads
  uint32_t pos_in_clean = 0, num_n_reads_till_now = 0;
  for (uint32_t i = 0; i < num_reads_total; i++) {
    if (read_flag_n[i] == true)
      num_n_reads_till_now++;
    else
      cumulative_n_reads[pos_in_clean++] = num_n_reads_till_now;
  }

  // First correct the order for singletons
  for (uint32_t i = 0; i < eg.num_reads_s; i++)
    order_s[i] += cumulative_n_reads[order_s[i]];

  // Now correct for clean reads (this is stored on file)
  for (int tid = 0; tid < eg.num_thr; tid++) {
    std::ifstream fin_order(eg.infile_order + '.' + std::to_string(tid),
                            std::ios::binary);
    UTILS_DIE_IF(!fin_order, "Cannot open file to read: " + eg.infile_order +
                                 '.' + std::to_string(tid));
    std::ofstream f_out_order(
        eg.infile_order + '.' + std::to_string(tid) + ".tmp", std::ios::binary);
    uint32_t pos;
    fin_order.read(reinterpret_cast<char*>(&pos), sizeof(uint32_t));
    while (!fin_order.eof()) {
      pos += cumulative_n_reads[pos];
      f_out_order.write(reinterpret_cast<char*>(&pos), sizeof(uint32_t));
      fin_order.read(reinterpret_cast<char*>(&pos), sizeof(uint32_t));
    }
    fin_order.close();
    f_out_order.close();
    remove((eg.infile_order + '.' + std::to_string(tid)).c_str());
    rename((eg.infile_order + '.' + std::to_string(tid) + ".tmp").c_str(),
           (eg.infile_order + '.' + std::to_string(tid)).c_str());
  }
  remove(eg.infile_order_n.c_str());
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
