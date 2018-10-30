#include <omp.h>
#include <cmath>  // abs
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "algorithms/SPRING/generate_read_streams.h"
#include "algorithms/SPRING/decode_read_streams.h"
#include "algorithms/SPRING/util.h"

namespace spring {

void generate_read_streams_se(const std::string &temp_dir,
                              const compression_params &cp) {

  std::string basedir = temp_dir;

  std::string file_subseq_0_0 = basedir + "/subseq_0_0"; // pos
  std::string file_subseq_1_0 = basedir + "/subseq_1_0"; // rcomp
  std::string file_subseq_3_0 = basedir + "/subseq_3_0"; // mmpos
  std::string file_subseq_3_1 = basedir + "/subseq_3_1"; // mmpos
  std::string file_subseq_4_0 = basedir + "/subseq_4_0"; // mmtype
  std::string file_subseq_4_1 = basedir + "/subseq_4_1"; // mmtype
  std::string file_subseq_6_0 = basedir + "/subseq_6_0"; // ureads
  std::string file_subseq_7_0 = basedir + "/subseq_7_0"; // rlen
  std::string file_subseq_12_0 = basedir + "/subseq_12_0"; // rtype

  std::string file_seq = basedir + "/read_seq.txt";
  std::string file_pos = basedir + "/read_pos.bin";
  std::string file_RC = basedir + "/read_rev.txt";
  std::string file_readlength = basedir + "/read_lengths.bin";
  std::string file_unaligned = basedir + "/read_unaligned.txt";
  std::string file_noise = basedir + "/read_noise.txt";
  std::string file_noisepos = basedir + "/read_noisepos.bin";
  std::string file_order = basedir + "/read_order.bin";

  // char_to_int
  int64_t char_to_int[128];
  char_to_int[(uint8_t)'A'] = 0;
  char_to_int[(uint8_t)'C'] = 1;
  char_to_int[(uint8_t)'G'] = 2;
  char_to_int[(uint8_t)'T'] = 3;
  char_to_int[(uint8_t)'N'] = 4;

  int64_t rc_to_int[128];
  rc_to_int[(uint8_t)'d'] = 0;
  rc_to_int[(uint8_t)'r'] = 1;

  // load some params
  uint32_t num_reads = cp.num_reads, num_reads_aligned = 0, num_reads_unaligned;
  int num_thr = cp.num_thr;
  char *RC_arr = new char[num_reads];
  uint16_t *read_length_arr = new uint16_t[num_reads];
  bool *flag_arr = new bool[num_reads];
  uint64_t *pos_in_noise_arr = new uint64_t[num_reads];
  uint64_t *pos_arr = new uint64_t[num_reads];
  uint16_t *noise_len_arr = new uint16_t[num_reads];

  std::vector<int64_t> subseq_0_0[num_thr];
  std::vector<int64_t> subseq_1_0[num_thr];
  std::vector<int64_t> subseq_3_0[num_thr];
  std::vector<int64_t> subseq_3_1[num_thr];
  std::vector<int64_t> subseq_4_0[num_thr];
  std::vector<int64_t> subseq_4_1[num_thr];
  std::vector<int64_t> subseq_6_0[num_thr];
  std::vector<int64_t> subseq_7_0[num_thr];
  std::vector<int64_t> subseq_12_0[num_thr];

  // read streams for aligned reads
  std::string seq;
  std::ifstream f_seq(file_seq);
  f_seq.seekg(0,f_seq.end);
  uint64_t seq_len = f_seq.tellg();
  seq.resize(seq_len);
  f_seq.seekg(0);
  f_seq.read(&seq[0], seq_len);
  f_seq.close();
  std::ifstream f_order;
  std::ifstream f_RC(file_RC);
  std::ifstream f_readlength(file_readlength, std::ios::binary);
  std::ifstream f_noise(file_noise);
  std::ifstream f_noisepos(file_noisepos, std::ios::binary);
  std::ifstream f_pos(file_pos, std::ios::binary);
  f_noisepos.seekg(0, f_noisepos.end);
  uint64_t noise_array_size = f_noisepos.tellg() / 2;
  f_noisepos.seekg(0, f_noisepos.beg);
  // divide by 2 because we have 2 bytes per noise
  char *noise_arr = new char[noise_array_size];
  uint16_t *noisepos_arr = new uint16_t[noise_array_size];
  char rc, noise_char;
  uint32_t order = 0;
  uint64_t current_pos_noise_arr = 0;
  uint64_t current_pos_noisepos_arr = 0;
  uint64_t pos;
  uint16_t num_noise_in_curr_read;
  uint16_t read_length, noisepos;

  while (f_RC.get(rc)) {
    f_readlength.read((char *)&read_length, sizeof(uint16_t));
    f_pos.read((char *)&pos, sizeof(uint64_t));
    RC_arr[order] = rc;
    read_length_arr[order] = read_length;
    flag_arr[order] = true;  // aligned
    pos_arr[order] = pos;
    pos_in_noise_arr[order] = current_pos_noise_arr;
    num_noise_in_curr_read = 0;
    f_noise.get(noise_char);
    while (noise_char != '\n') {
      noise_arr[current_pos_noise_arr++] = noise_char;
      num_noise_in_curr_read++;
      f_noise.get(noise_char);
    }
    for (uint16_t i = 0; i < num_noise_in_curr_read; i++) {
      f_noisepos.read((char *)&noisepos, sizeof(uint16_t));
      noisepos_arr[current_pos_noisepos_arr] = noisepos;
      current_pos_noisepos_arr++;
    }
    noise_len_arr[order] = num_noise_in_curr_read;
    num_reads_aligned++;
    order++;
  }
  f_noise.close();
  f_noisepos.close();
  f_RC.close();
  f_pos.close();

  // Now start with unaligned reads
  num_reads_unaligned = num_reads - num_reads_aligned;
  std::ifstream f_unaligned(file_unaligned);
  f_unaligned.seekg(0, f_unaligned.end);
  uint64_t unaligned_array_size = f_unaligned.tellg();
  f_unaligned.seekg(0, f_unaligned.beg);
  char *unaligned_arr = new char[unaligned_array_size];
  f_unaligned.read(unaligned_arr, unaligned_array_size);
  f_unaligned.close();
  uint64_t current_pos_in_unaligned_arr = 0;
  for (uint32_t i = 0; i < num_reads_unaligned; i++) {
    f_readlength.read((char *)&read_length, sizeof(uint16_t));
    read_length_arr[order] = read_length;
    pos_arr[order] = current_pos_in_unaligned_arr;
    current_pos_in_unaligned_arr += read_length;
    flag_arr[order] = false;  // unaligned
    order++;
  }
  f_readlength.close();

  // delete old streams
  remove(file_noise.c_str());
  remove(file_noisepos.c_str());
  remove(file_RC.c_str());
  remove(file_readlength.c_str());
  remove(file_unaligned.c_str());
  remove(file_pos.c_str());
  remove(file_seq.c_str());

  // Now generate new streams and compress blocks in parallel
  omp_set_num_threads(num_thr);
  uint32_t num_reads_per_block = cp.num_reads_per_block;
// this is actually number of read pairs per block for PE
#pragma omp parallel
  {
    uint64_t tid = omp_get_thread_num();
    uint64_t block_num = tid;
    bool done = false;
    while (!done) {
      //clear vectors
      subseq_0_0[tid].clear();
      subseq_1_0[tid].clear();
      subseq_3_0[tid].clear();
      subseq_3_1[tid].clear();
      subseq_4_0[tid].clear();
      subseq_4_1[tid].clear();
      subseq_6_0[tid].clear();
      subseq_7_0[tid].clear();
      subseq_12_0[tid].clear();

      uint64_t start_read_num = block_num * num_reads_per_block;
      uint64_t end_read_num = (block_num + 1) * num_reads_per_block;

      if (start_read_num >= num_reads) break;
      if (end_read_num >= num_reads) {
        done = true;
        end_read_num = num_reads;
      }

      // first find the seq
      uint64_t seq_start, seq_end;
      if (flag_arr[start_read_num] == false)
        seq_start = seq_end = 0; // all reads unaligned
      else {
        seq_start = pos_arr[start_read_num];
        // find last read in AU that's aligned
        uint64_t i = start_read_num;
        for (; i < end_read_num; i++)
          if (flag_arr[i] == false)
            break;
        seq_end = pos_arr[i-1] + read_length_arr[i-1];
      }
      if (seq_start != seq_end) {
        // not all unaligned
        subseq_7_0[tid].push_back(seq_end - seq_start); // rlen
        subseq_12_0[tid].push_back(6); // rtype
        for (uint64_t i = seq_start; i < seq_end; i++)
          subseq_6_0[tid].push_back(char_to_int[(uint8_t)seq[i]]); // ureads
      }
      uint64_t prevpos = 0, diffpos;
      // Write streams
      for (uint64_t i = start_read_num; i < end_read_num; i++) {
        if (flag_arr[i] == true) {
          subseq_7_0[tid].push_back(read_length_arr[i]); // rlen
          subseq_1_0[tid].push_back(rc_to_int[(uint8_t)RC_arr[i]]); // rcomp
          if (i == start_read_num) {
            // Note: In order non-preserving mode, if the first read of
            // the block is a singleton, then the rest are too.
            subseq_0_0[tid].push_back(0);
            prevpos = pos_arr[i];
          } else {
            diffpos = pos_arr[i] - prevpos;
            subseq_0_0[tid].push_back(diffpos); // pos
            prevpos = pos_arr[i];
          }
          if (noise_len_arr[i] == 0)
            subseq_12_0[tid].push_back(1); // rtype = P
          else {
            subseq_12_0[tid].push_back(3); // rtype = M
            for (uint16_t j = 0; j < noise_len_arr[i]; j++) {
              subseq_3_0[tid].push_back(0); // mmpos
              subseq_3_1[tid].push_back(noisepos_arr[pos_in_noise_arr[i] + j]);
              subseq_4_0[tid].push_back(0); // mmtype = Substitution
              subseq_4_1[tid].push_back(char_to_int[(uint8_t)noise_arr[pos_in_noise_arr[i] + j]]);
            }
            subseq_3_0[tid].push_back(1);
          }
        } else {
          subseq_12_0[tid].push_back(6); // rtype
          subseq_7_0[tid].push_back(read_length_arr[i]); // rlen
          for (uint64_t j = 0; j < read_length_arr[i]; j++) {
            subseq_6_0[tid].push_back(char_to_int[(uint8_t)unaligned_arr[pos_arr[i] + j]]); // ureads
          }
          subseq_0_0[tid].push_back(seq_end - prevpos); // pos
          subseq_1_0[tid].push_back(0); // rcomp
          subseq_7_0[tid].push_back(read_length_arr[i]); // rlen
          subseq_12_0[tid].push_back(1); // rtype = P
          prevpos = seq_end;
          seq_end = prevpos + read_length_arr[i];
        }
      }
      // write vectors to files
      write_vector_to_file(subseq_0_0[tid], file_subseq_0_0 + '.' + std::to_string(block_num));
      write_vector_to_file(subseq_1_0[tid], file_subseq_1_0 + '.' + std::to_string(block_num));
      write_vector_to_file(subseq_3_0[tid], file_subseq_3_0 + '.' + std::to_string(block_num));
      write_vector_to_file(subseq_3_1[tid], file_subseq_3_1 + '.' + std::to_string(block_num));
      write_vector_to_file(subseq_4_0[tid], file_subseq_4_0 + '.' + std::to_string(block_num));
      write_vector_to_file(subseq_4_1[tid], file_subseq_4_1 + '.' + std::to_string(block_num));
      write_vector_to_file(subseq_6_0[tid], file_subseq_6_0 + '.' + std::to_string(block_num));
      write_vector_to_file(subseq_7_0[tid], file_subseq_7_0 + '.' + std::to_string(block_num));
      write_vector_to_file(subseq_12_0[tid], file_subseq_12_0 + '.' + std::to_string(block_num));

      block_num += num_thr;
    }
  }  // end omp parallel

  // decode and write the reads to a file (for testing purposes)
  uint32_t num_blocks = 1 + (num_reads-1)/num_reads_per_block;
  decompress_se_reads(temp_dir, num_blocks);

  // deallocate
  delete[] RC_arr;
  delete[] read_length_arr;
  delete[] flag_arr;
  delete[] pos_in_noise_arr;
  delete[] pos_arr;
  delete[] noise_len_arr;
  delete[] noise_arr;
  delete[] noisepos_arr;
  delete[] unaligned_arr;

  return;
}

void generate_read_streams_pe(const std::string &temp_dir,
                              const compression_params &cp) {

// basic approach: start looking at reads from left to right. If current is aligned but
// pair is unaligned, pair is kept at the end current AU and stored in different record.
// We try to keep number of records in AU = num_reads_per_block (without counting the the unaligned
// pairs above.
// As we scan to right, if we come across a read whose pair has already been seen in the same AU
// and the gap is < 32768 (and non-ovelapping since delta >= 0), then we add this to the paired read's
// genomic record. Finally when we come to unaligned reads whose pair is also unaligned, we store them
// in same genomic record.

  std::string basedir = temp_dir;

  std::string file_subseq_0_0 = basedir + "/subseq_0_0"; // pos
  std::string file_subseq_1_0 = basedir + "/subseq_1_0"; // rcomp
  std::string file_subseq_3_0 = basedir + "/subseq_3_0"; // mmpos
  std::string file_subseq_3_1 = basedir + "/subseq_3_1"; // mmpos
  std::string file_subseq_4_0 = basedir + "/subseq_4_0"; // mmtype
  std::string file_subseq_4_1 = basedir + "/subseq_4_1"; // mmtype
  std::string file_subseq_6_0 = basedir + "/subseq_6_0"; // ureads
  std::string file_subseq_7_0 = basedir + "/subseq_7_0"; // rlen
  std::string file_subseq_8_0 = basedir + "/subseq_8_0"; // pair
  std::string file_subseq_8_1 = basedir + "/subseq_8_1"; // pair
  std::string file_subseq_8_2 = basedir + "/subseq_8_2"; // pair
  std::string file_subseq_8_3 = basedir + "/subseq_8_3"; // pair
  std::string file_subseq_8_4 = basedir + "/subseq_8_4"; // pair
  std::string file_subseq_8_5 = basedir + "/subseq_8_5"; // pair
  std::string file_subseq_8_7 = basedir + "/subseq_8_7"; // pair
  std::string file_subseq_8_8 = basedir + "/subseq_8_8"; // pair
  std::string file_subseq_12_0 = basedir + "/subseq_12_0"; // rtype

  std::string file_seq = basedir + "/read_seq.txt";
  std::string file_pos = basedir + "/read_pos.bin";
  std::string file_RC = basedir + "/read_rev.txt";
  std::string file_readlength = basedir + "/read_lengths.bin";
  std::string file_unaligned = basedir + "/read_unaligned.txt";
  std::string file_noise = basedir + "/read_noise.txt";
  std::string file_noisepos = basedir + "/read_noisepos.bin";
  std::string file_order = basedir + "/read_order.bin";
  std::string file_order_quality = basedir + "/order_quality.bin";
  std::string file_order_id = basedir + "/order_id.bin";
  std::string file_blocks_quality = basedir + "/blocks_quality.bin";
  std::string file_blocks_id = basedir + "/blocks_id.bin";

  // char_to_int
  int64_t char_to_int[128];
  char_to_int[(uint8_t)'A'] = 0;
  char_to_int[(uint8_t)'C'] = 1;
  char_to_int[(uint8_t)'G'] = 2;
  char_to_int[(uint8_t)'T'] = 3;
  char_to_int[(uint8_t)'N'] = 4;

  int64_t rc_to_int[128];
  rc_to_int[(uint8_t)'d'] = 0;
  rc_to_int[(uint8_t)'r'] = 1;

  // load some params
  uint32_t num_reads = cp.num_reads, num_reads_aligned = 0, num_reads_unaligned;
  uint32_t num_reads_by_2 = num_reads / 2;
  int num_thr = cp.num_thr;
  char *RC_arr = new char[num_reads];
  uint16_t *read_length_arr = new uint16_t[num_reads];
  bool *flag_arr = new bool[num_reads];
  uint64_t *pos_in_noise_arr = new uint64_t[num_reads];
  uint64_t *pos_arr = new uint64_t[num_reads];
  uint16_t *noise_len_arr = new uint16_t[num_reads];

  uint32_t num_reads_per_block = cp.num_reads_per_block;

  std::vector<uint32_t> order_arr(num_reads);
  std::vector<uint32_t> block_num(num_reads);

  std::vector<uint32_t> genomic_record_index(num_reads);

  std::vector<uint32_t> block_start, block_end; // block start and end positions wrt
                                                // read_index_genomic_record vector
  std::vector<uint32_t> read_index_genomic_record; // read numbers in genomic records
  std::vector<uint32_t> block_seq_start, block_seq_end; // seq start and end positions for this block

  std::vector<int64_t> subseq_0_0[num_thr];
  std::vector<int64_t> subseq_1_0[num_thr];
  std::vector<int64_t> subseq_3_0[num_thr];
  std::vector<int64_t> subseq_3_1[num_thr];
  std::vector<int64_t> subseq_4_0[num_thr];
  std::vector<int64_t> subseq_4_1[num_thr];
  std::vector<int64_t> subseq_6_0[num_thr];
  std::vector<int64_t> subseq_7_0[num_thr];
  std::vector<int64_t> subseq_8_0[num_thr];
  std::vector<int64_t> subseq_8_1[num_thr];
  std::vector<int64_t> subseq_8_2[num_thr];
  std::vector<int64_t> subseq_8_3[num_thr];
  std::vector<int64_t> subseq_8_4[num_thr];
  std::vector<int64_t> subseq_8_5[num_thr];
  std::vector<int64_t> subseq_8_7[num_thr];
  std::vector<int64_t> subseq_8_8[num_thr];
  std::vector<int64_t> subseq_12_0[num_thr];


  // PE step 1: read all streams indexed by original position in FASTQ, also read order array

  // read streams for aligned reads
  std::string seq;
  std::ifstream f_seq(file_seq);
  f_seq.seekg(0,f_seq.end);
  uint64_t seq_len = f_seq.tellg();
  seq.resize(seq_len);
  f_seq.seekg(0);
  f_seq.read(&seq[0], seq_len);
  f_seq.close();
  std::ifstream f_order;
  f_order.open(file_order, std::ios::binary);
  std::ifstream f_RC(file_RC);
  std::ifstream f_readlength(file_readlength, std::ios::binary);
  std::ifstream f_noise(file_noise);
  std::ifstream f_noisepos(file_noisepos, std::ios::binary);
  std::ifstream f_pos(file_pos, std::ios::binary);
  f_noisepos.seekg(0, f_noisepos.end);
  uint64_t noise_array_size = f_noisepos.tellg() / 2;
  f_noisepos.seekg(0, f_noisepos.beg);
  // divide by 2 because we have 2 bytes per noise
  char *noise_arr = new char[noise_array_size];
  uint16_t *noisepos_arr = new uint16_t[noise_array_size];
  char rc, noise_char;
  uint32_t order = 0;
  uint64_t current_pos_noise_arr = 0;
  uint64_t current_pos_noisepos_arr = 0;
  uint64_t pos;
  uint16_t num_noise_in_curr_read;
  uint16_t read_length, noisepos;
  uint32_t ind = 0;
  while (f_RC.get(rc)) {
    f_order.read((char *)&order, sizeof(uint32_t));
    order_arr[ind++] = order;
    f_readlength.read((char *)&read_length, sizeof(uint16_t));
    f_pos.read((char *)&pos, sizeof(uint64_t));
    RC_arr[order] = rc;
    read_length_arr[order] = read_length;
    flag_arr[order] = true;  // aligned
    pos_arr[order] = pos;
    pos_in_noise_arr[order] = current_pos_noise_arr;
    num_noise_in_curr_read = 0;
    f_noise.get(noise_char);
    while (noise_char != '\n') {
      noise_arr[current_pos_noise_arr++] = noise_char;
      num_noise_in_curr_read++;
      f_noise.get(noise_char);
    }
    for (uint16_t i = 0; i < num_noise_in_curr_read; i++) {
      f_noisepos.read((char *)&noisepos, sizeof(uint16_t));
      noisepos_arr[current_pos_noisepos_arr] = noisepos;
      current_pos_noisepos_arr++;
    }
    noise_len_arr[order] = num_noise_in_curr_read;
    num_reads_aligned++;
  }
  f_noise.close();
  f_noisepos.close();
  f_RC.close();
  f_pos.close();

  // Now start with unaligned reads
  num_reads_unaligned = num_reads - num_reads_aligned;
  std::ifstream f_unaligned(file_unaligned);
  f_unaligned.seekg(0, f_unaligned.end);
  uint64_t unaligned_array_size = f_unaligned.tellg();
  f_unaligned.seekg(0, f_unaligned.beg);
  char *unaligned_arr = new char[unaligned_array_size];
  f_unaligned.read(unaligned_arr, unaligned_array_size);
  f_unaligned.close();
  uint64_t current_pos_in_unaligned_arr = 0;
  for (uint32_t i = 0; i < num_reads_unaligned; i++) {
    f_order.read((char *)&order, sizeof(uint32_t));
    order_arr[ind++] = order;
    f_readlength.read((char *)&read_length, sizeof(uint16_t));
    read_length_arr[order] = read_length;
    pos_arr[order] = current_pos_in_unaligned_arr;
    current_pos_in_unaligned_arr += read_length;
    flag_arr[order] = false;  // unaligned
  }
  f_order.close();
  f_readlength.close();

  // delete old streams
  remove(file_noise.c_str());
  remove(file_noisepos.c_str());
  remove(file_RC.c_str());
  remove(file_readlength.c_str());
  remove(file_unaligned.c_str());
  remove(file_pos.c_str());
  remove(file_seq.c_str());

  // PE step 2: decide on the blocks for the reads
  uint32_t num_records_current_block = 0;
  uint32_t current_block_num = 0;
  uint32_t current_block_seq_end = 0;
  block_start.push_back(0);

  std::vector<bool> already_seen(num_reads, false);
  std::vector<uint32_t> to_push_at_end_of_block;

  for (uint32_t i = 0; i < num_reads; i++) {
    uint32_t current = order_arr[i];
    if (num_records_current_block == 0) {
      // first read of block
      if (!flag_arr[current])
        block_seq_start.push_back(0);
      else
        block_seq_start.push_back(pos_arr[current]);
    }
    if (!already_seen[current]) {
      // current is already seen when current is unaligned and its pair has already
      // appeared before - in such cases we don't need to handle it now.
      already_seen[current] = true;
      uint32_t pair = (current < num_reads_by_2)?(current + num_reads_by_2):(current - num_reads_by_2);
      if (already_seen[pair]) {
        if (block_num[pair] == current_block_num && (pos_arr[current] >= pos_arr[pair] + read_length_arr[pair]) && (pos_arr[current] - pos_arr[pair] - read_length_arr[pair] < 32768)) {
          // put in same record
          block_num[current] = current_block_num;
          genomic_record_index[current] = genomic_record_index[pair];
        } else {
          // new genomic record
          block_num[current] = current_block_num;
          genomic_record_index[current] = num_records_current_block++;
          read_index_genomic_record.push_back(current);
        }
      } else {
        // add new genomic record
        block_num[current] = current_block_num;
        genomic_record_index[current] = num_records_current_block++;

        if (!flag_arr[pair]) {
          already_seen[pair] = true;
          if (!flag_arr[current]) {
            // both unaligned - put in same record
            block_num[pair] = current_block_num;
            genomic_record_index[pair] = num_records_current_block - 1;
            read_index_genomic_record.push_back(std::min(current,pair)); // always put read 1 first in this case
          } else {
            // pair is unaligned, put in same block at end (not in same record)
            // for now, put in to_push_at_end_of_block vector (processed at end of block)
            read_index_genomic_record.push_back(current);
            to_push_at_end_of_block.push_back(pair);
          }
        } else {
          read_index_genomic_record.push_back(current);
        }
      }
      if (flag_arr[current]) {
        // update current_block_seq_end
        if (current_block_seq_end < pos_arr[current] + read_length_arr[current])
          current_block_seq_end = pos_arr[current] + read_length_arr[current];
      }
    }
    if (num_records_current_block == num_reads_per_block || i == num_reads - 1) {
      // block done
      // put in the reads in to_push_at_end_of_block vector and clear the vector
      for (uint32_t j: to_push_at_end_of_block) {
        block_num[j] = current_block_num;
        genomic_record_index[j] = num_records_current_block++;
        read_index_genomic_record.push_back(j);
      }
      to_push_at_end_of_block.clear();
      block_end.push_back(block_start.back() + num_records_current_block);
      block_seq_end.push_back(current_block_seq_end);

      if (i != num_reads - 1) {
        // start new block
        num_records_current_block = 0;
        current_block_num++;
        current_block_seq_end = 0;
        block_start.push_back(read_index_genomic_record.size());
      }
    }
  }
  already_seen.clear();
  order_arr.clear();
  // PE step 3: generate index for ids and quality

  // quality:
  std::ofstream f_order_quality(file_order_quality, std::ios::binary);
  // store order (as usual in uint32_t)
  std::ofstream f_blocks_quality(file_blocks_quality, std::ios::binary);
  // store block start and end positions (differs from the block_start and end because here we measure
  // in terms of quality values rather than records
  uint32_t quality_block_pos = 0;
  for (uint32_t i = 0; i < block_start.size(); i++) {
    f_blocks_quality.write((char*)&quality_block_pos, sizeof(uint32_t));
    for (uint32_t j = block_start[i]; j < block_end[i]; j++) {
      uint32_t current = read_index_genomic_record[j];
      uint32_t pair = (current < num_reads_by_2)?(current + num_reads_by_2):(current - num_reads_by_2);
      if ((block_num[current] == block_num[pair]) && (genomic_record_index[pair] == genomic_record_index[current])) {
        // pair in genomic record
        f_order_quality.write((char*)&current, sizeof(uint32_t));
        quality_block_pos++;
        f_order_quality.write((char*)&pair, sizeof(uint32_t));
        quality_block_pos++;
      } else {
        // only single read in genomic record
        f_order_quality.write((char*)&current, sizeof(uint32_t));
        quality_block_pos++;
      }
    }
    f_blocks_quality.write((char*)&quality_block_pos, sizeof(uint32_t));
  }
  f_order_quality.close();
  f_blocks_quality.close();
  std::cout << quality_block_pos << "\n";
  // id:
  std::ofstream f_blocks_id(file_blocks_id, std::ios::binary);
  // store block start and end positions (measured in terms of records since 1 record = 1 id)
  for (uint32_t i = 0; i < block_start.size(); i++) {
    f_blocks_id.write((char*)&block_start[i], sizeof(uint32_t));
    f_blocks_id.write((char*)&block_end[i], sizeof(uint32_t));
    std::ofstream f_order_id(file_order_id + "." + std::to_string(i), std::ios::binary);
    // store order (uint64_t - < 2^32 means index while greater means DUP)
    for (uint32_t j = block_start[i]; j < block_end[i]; j++) {
      uint32_t current = read_index_genomic_record[j];
      uint32_t pair = (current < num_reads_by_2)?(current + num_reads_by_2):(current - num_reads_by_2);
      if ((block_num[current] == block_num[pair]) && (genomic_record_index[pair] < genomic_record_index[current])) {
        // DUP
        uint64_t gap = genomic_record_index[current] - genomic_record_index[pair];
        uint64_t min_index = (current > pair)?pair:current;
        uint64_t val_to_write = (gap<<32)|min_index;
        f_order_id.write((char*)&val_to_write, sizeof(uint64_t));
      } else {
        // just write the min of current and pair
        uint64_t min_index = (current > pair)?pair:current;
        f_order_id.write((char*)&min_index, sizeof(uint64_t));
      }
    }
    f_order_id.close();
  }
  f_blocks_id.close();

  // PE step 4: Now generate read streams and compress blocks in parallel
  omp_set_num_threads(num_thr);
#pragma omp parallel
  {
    uint64_t tid = omp_get_thread_num();
    uint64_t cur_block_num = tid;
    while (true) {
      //clear vectors
      subseq_0_0[tid].clear();
      subseq_1_0[tid].clear();
      subseq_3_0[tid].clear();
      subseq_3_1[tid].clear();
      subseq_4_0[tid].clear();
      subseq_4_1[tid].clear();
      subseq_6_0[tid].clear();
      subseq_7_0[tid].clear();
      subseq_8_0[tid].clear();
      subseq_8_1[tid].clear();
      subseq_8_2[tid].clear();
      subseq_8_3[tid].clear();
      subseq_8_4[tid].clear();
      subseq_8_5[tid].clear();
      subseq_8_7[tid].clear();
      subseq_8_8[tid].clear();
      subseq_12_0[tid].clear();

      if (cur_block_num >= block_start.size())
        break;

      // first find the seq
      uint64_t seq_start = block_seq_start[cur_block_num], seq_end = block_seq_end[cur_block_num];
      if (seq_start != seq_end) {
        // not all unaligned
        subseq_7_0[tid].push_back(seq_end - seq_start); // rlen
        subseq_12_0[tid].push_back(6); // rtype
        for (uint64_t i = seq_start; i < seq_end; i++)
          subseq_6_0[tid].push_back(char_to_int[(uint8_t)seq[i]]); // ureads
      }
      uint64_t prevpos = 0, diffpos;
      // Write streams
      for (uint32_t i = block_start[cur_block_num]; i < block_end[cur_block_num]; i++) {
        uint32_t current = read_index_genomic_record[i];
        uint32_t pair = (current < num_reads_by_2)?(current + num_reads_by_2):(current - num_reads_by_2);

        if (flag_arr[current] == true) {
          if (i == block_start[cur_block_num]) {
            // Note: In order non-preserving mode, if the first read of
            // the block is a singleton, then the rest are too.
            subseq_0_0[tid].push_back(0); // pos
            prevpos = pos_arr[current];
          } else {
            diffpos = pos_arr[current] - prevpos;
            subseq_0_0[tid].push_back(diffpos); // pos
            prevpos = pos_arr[current];
          }
        }
        if ((block_num[current] == block_num[pair]) && (genomic_record_index[current] == genomic_record_index[pair])) {
          // both reads in same record
          if (flag_arr[current] == false) {
            // Case 1: both unaligned
            subseq_12_0[tid].push_back(6); // rtype
            subseq_7_0[tid].push_back(read_length_arr[current] + read_length_arr[pair]); // rlen
            for (uint64_t j = 0; j < read_length_arr[current]; j++) {
              subseq_6_0[tid].push_back(char_to_int[(uint8_t)unaligned_arr[pos_arr[current] + j]]); // ureads
            }
            for (uint64_t j = 0; j < read_length_arr[pair]; j++) {
              subseq_6_0[tid].push_back(char_to_int[(uint8_t)unaligned_arr[pos_arr[pair] + j]]); // ureads
            }
            subseq_0_0[tid].push_back(seq_end - prevpos); // pos
            subseq_1_0[tid].push_back(0+2*0); // rcomp
            subseq_7_0[tid].push_back(read_length_arr[current]); // rlen
            subseq_7_0[tid].push_back(read_length_arr[pair]); // rlen
            subseq_12_0[tid].push_back(1); // rtype = P
            subseq_8_0[tid].push_back(0); // pair decoding case same_rec
            bool read_1_first = true;
            uint16_t delta = 0;
            subseq_8_1[tid].push_back(!(read_1_first)+2*delta); // pair
            prevpos = seq_end;
            seq_end = prevpos + read_length_arr[current] + read_length_arr[pair];
          } else {
            // Case 2: both aligned
            subseq_7_0[tid].push_back(read_length_arr[current]); // rlen
            subseq_7_0[tid].push_back(read_length_arr[pair]); // rlen
            subseq_1_0[tid].push_back(rc_to_int[(uint8_t)RC_arr[current]]+2*rc_to_int[(uint8_t)RC_arr[pair]]); // rcomp
            if (noise_len_arr[current] == 0 && noise_len_arr[pair] == 0)
              subseq_12_0[tid].push_back(1); // rtype = P
            else {
              subseq_12_0[tid].push_back(3); // rtype = M
              uint16_t abs_noise_pos = 0;
              for (uint16_t j = 0; j < noise_len_arr[current]; j++) {
                subseq_3_0[tid].push_back(0); // mmpos
                subseq_3_1[tid].push_back(noisepos_arr[pos_in_noise_arr[current] + j]); // mmpos
                abs_noise_pos += noisepos_arr[pos_in_noise_arr[current] + j];
                subseq_4_0[tid].push_back(0); // mmtype = Substitution
                subseq_4_1[tid].push_back(char_to_int[(uint8_t)noise_arr[pos_in_noise_arr[current] + j]]);
              }
              for (uint16_t j = 0; j < noise_len_arr[pair]; j++) {
                subseq_3_0[tid].push_back(0); // mmpos
                if (j == 0)
                  subseq_3_1[tid].push_back(read_length_arr[current] - abs_noise_pos + noisepos_arr[pos_in_noise_arr[pair] + j]); // mmpos
                else
                  subseq_3_1[tid].push_back(noisepos_arr[pos_in_noise_arr[pair] + j]); // mmpos
                subseq_4_0[tid].push_back(0); // mmtype = Substitution
                subseq_4_1[tid].push_back(char_to_int[(uint8_t)noise_arr[pos_in_noise_arr[pair] + j]]);
              }
              subseq_3_0[tid].push_back(1);
            }
            bool read_1_first = (current < pair);
            uint16_t delta = pos_arr[pair] - pos_arr[current] - read_length_arr[current];
            subseq_8_0[tid].push_back(0); // pair decoding case same_rec
            subseq_8_1[tid].push_back(!(read_1_first)+2*delta); // pair
          }
        } else {
          // only one read in genomic record
          if (flag_arr[current] == true) {
            subseq_7_0[tid].push_back(read_length_arr[current]); // rlen
            subseq_1_0[tid].push_back(rc_to_int[(uint8_t)RC_arr[current]]); // rcomp
            if (noise_len_arr[current] == 0)
              subseq_12_0[tid].push_back(1); // rtype = P
            else {
              subseq_12_0[tid].push_back(3); // rtype = M
              for (uint16_t j = 0; j < noise_len_arr[current]; j++) {
                subseq_3_0[tid].push_back(0); // mmpos
                subseq_3_1[tid].push_back(noisepos_arr[pos_in_noise_arr[current] + j]);
                subseq_4_0[tid].push_back(0); // mmtype = Substitution
                subseq_4_1[tid].push_back(char_to_int[(uint8_t)noise_arr[pos_in_noise_arr[current] + j]]);
              }
              subseq_3_0[tid].push_back(1);
            }
          } else {
            subseq_12_0[tid].push_back(6); // rtype
            subseq_7_0[tid].push_back(read_length_arr[current]); // rlen
            for (uint64_t j = 0; j < read_length_arr[current]; j++) {
              subseq_6_0[tid].push_back(char_to_int[(uint8_t)unaligned_arr[pos_arr[current] + j]]); // ureads
            }
            subseq_0_0[tid].push_back(seq_end - prevpos); // pos
            subseq_1_0[tid].push_back(0); // rcomp
            subseq_7_0[tid].push_back(read_length_arr[current]); // rlen
            subseq_12_0[tid].push_back(1); // rtype = P
            prevpos = seq_end;
            seq_end = prevpos + read_length_arr[current];
          }

          // pair subsequences
          bool same_block = (block_num[current] == block_num[pair]);
          bool read_1_first = (current < pair);
          if (same_block && !read_1_first) {
            subseq_8_0[tid].push_back(1); // R1_split
            subseq_8_2[tid].push_back(genomic_record_index[pair]);
          } else if (same_block && read_1_first) {
            subseq_8_0[tid].push_back(2); // R2_split
            subseq_8_3[tid].push_back(genomic_record_index[pair]);
          } else if (!same_block && !read_1_first) {
            subseq_8_0[tid].push_back(3); // R1_diff_ref_seq
            subseq_8_4[tid].push_back(block_num[pair]);
            subseq_8_7[tid].push_back(genomic_record_index[pair]);
          } else {
            subseq_8_0[tid].push_back(4); // R2_diff_ref_seq
            subseq_8_5[tid].push_back(block_num[pair]);
            subseq_8_8[tid].push_back(genomic_record_index[pair]);
          }
        }
      }
      // write vectors to files
      write_vector_to_file(subseq_0_0[tid], file_subseq_0_0 + '.' + std::to_string(cur_block_num));
      write_vector_to_file(subseq_1_0[tid], file_subseq_1_0 + '.' + std::to_string(cur_block_num));
      write_vector_to_file(subseq_3_0[tid], file_subseq_3_0 + '.' + std::to_string(cur_block_num));
      write_vector_to_file(subseq_3_1[tid], file_subseq_3_1 + '.' + std::to_string(cur_block_num));
      write_vector_to_file(subseq_4_0[tid], file_subseq_4_0 + '.' + std::to_string(cur_block_num));
      write_vector_to_file(subseq_4_1[tid], file_subseq_4_1 + '.' + std::to_string(cur_block_num));
      write_vector_to_file(subseq_6_0[tid], file_subseq_6_0 + '.' + std::to_string(cur_block_num));
      write_vector_to_file(subseq_7_0[tid], file_subseq_7_0 + '.' + std::to_string(cur_block_num));
      write_vector_to_file(subseq_8_0[tid], file_subseq_8_0 + '.' + std::to_string(cur_block_num));
      write_vector_to_file(subseq_8_1[tid], file_subseq_8_1 + '.' + std::to_string(cur_block_num));
      write_vector_to_file(subseq_8_2[tid], file_subseq_8_2 + '.' + std::to_string(cur_block_num));
      write_vector_to_file(subseq_8_3[tid], file_subseq_8_3 + '.' + std::to_string(cur_block_num));
      write_vector_to_file(subseq_8_4[tid], file_subseq_8_4 + '.' + std::to_string(cur_block_num));
      write_vector_to_file(subseq_8_5[tid], file_subseq_8_5 + '.' + std::to_string(cur_block_num));
      write_vector_to_file(subseq_8_7[tid], file_subseq_8_7 + '.' + std::to_string(cur_block_num));
      write_vector_to_file(subseq_8_8[tid], file_subseq_8_8 + '.' + std::to_string(cur_block_num));
      write_vector_to_file(subseq_12_0[tid], file_subseq_12_0 + '.' + std::to_string(cur_block_num));

      cur_block_num += num_thr;
    }
  }  // end omp parallel

  // decode and write the reads to a file (for testing purposes)
  uint32_t num_blocks = block_start.size();
  decompress_pe_reads(temp_dir, num_blocks);

  // deallocate
  delete[] RC_arr;
  delete[] read_length_arr;
  delete[] flag_arr;
  delete[] pos_in_noise_arr;
  delete[] pos_arr;
  delete[] noise_len_arr;
  delete[] noise_arr;
  delete[] noisepos_arr;
  delete[] unaligned_arr;

  return;
}
}  // namespace spring
