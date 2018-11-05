#ifdef GENIE_USE_OPENMP
    #include <omp.h>
#endif
#include <cmath>  // abs
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "descriptors/spring/generate_read_streams_eru.h"
#include "descriptors/spring/util.h"

namespace spring {

void generate_read_streams(const std::string &temp_dir,
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
  std::string file_subseq_8_0 = basedir + "/subseq_8_0"; // pair
  std::string file_subseq_8_10 = basedir + "/subseq_8_10"; // pair
  std::string file_subseq_8_11 = basedir + "/subseq_8_11"; // pair
  std::string file_subseq_8_12 = basedir + "/subseq_8_12"; // pair
  std::string file_subseq_12_0 = basedir + "/subseq_12_0"; // rtype

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
  bool preserve_order = cp.preserve_order;
  bool paired_end = cp.paired_end;
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
  std::vector<int64_t> subseq_8_0[num_thr];
  std::vector<int64_t> subseq_8_10[num_thr];
  std::vector<int64_t> subseq_8_11[num_thr];
  std::vector<int64_t> subseq_8_12[num_thr];
  std::vector<int64_t> subseq_12_0[num_thr];

  // read streams for aligned reads
  std::ifstream f_order;
  if (preserve_order || paired_end) f_order.open(file_order, std::ios::binary);
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
    if (preserve_order || paired_end)
      f_order.read((char *)&order, sizeof(uint32_t)); 
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
    if (!preserve_order && !paired_end)
      order++;
  }
  f_noise.close();
  f_noisepos.close();
  f_RC.close();
  f_pos.close();

  // Now start with unaligned reads
  num_reads_unaligned = num_reads - num_reads_aligned;
  std::string unaligned_arr = read_file_as_string(file_unaligned);
  uint64_t current_pos_in_unaligned_arr = 0;
  for (uint32_t i = 0; i < num_reads_unaligned; i++) {
    if (preserve_order || paired_end)
      f_order.read((char *)&order, sizeof(uint32_t)); 
    f_readlength.read((char *)&read_length, sizeof(uint16_t));
    read_length_arr[order] = read_length;
    pos_arr[order] = current_pos_in_unaligned_arr;
    current_pos_in_unaligned_arr += read_length;
    flag_arr[order] = false;  // unaligned
    if (!preserve_order && !paired_end)
      order++;
  }
  if (preserve_order || paired_end)
    f_order.close();
  f_readlength.close();

  // delete old streams
  remove(file_noise.c_str());
  remove(file_noisepos.c_str());
  remove(file_RC.c_str());
  remove(file_readlength.c_str());
  remove(file_unaligned.c_str());
  remove(file_pos.c_str());

  // Now generate new streams and compress blocks in parallel
#ifdef GENIE_USE_OPENMP
  omp_set_num_threads(num_thr);
#endif
  uint32_t num_reads_per_block = cp.num_reads_per_block;
// this is actually number of read pairs per block for PE
#ifdef GENIE_USE_OPENMP
#pragma omp parallel
#endif
  {
#ifdef GENIE_USE_OPENMP
    uint64_t tid = omp_get_thread_num();
#else
    uint64_t tid = 0;
#endif
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
      subseq_8_0[tid].clear();
      subseq_8_10[tid].clear();
      subseq_8_11[tid].clear();
      subseq_8_12[tid].clear();
      subseq_12_0[tid].clear();

      uint64_t start_read_num = block_num * num_reads_per_block;
      uint64_t end_read_num = (block_num + 1) * num_reads_per_block;
      if (!paired_end) {
        if (start_read_num >= num_reads) break;
        if (end_read_num >= num_reads) {
          done = true;
          end_read_num = num_reads;
        }
      } else {
        if (start_read_num >= num_reads/2) break;
        if (end_read_num >= num_reads/2) {
          done = true;
          end_read_num = num_reads/2;
        }
      }

      uint64_t prevpos = 0, diffpos;
      // Write streams
      for (uint64_t i = start_read_num; i < end_read_num; i++) {
        if(!paired_end) {
          if (flag_arr[i] == true) {
            subseq_7_0[tid].push_back(read_length_arr[i]); // rlen
            subseq_1_0[tid].push_back(rc_to_int[(uint8_t)RC_arr[i]]); // rcomp
            if (preserve_order) {
              subseq_0_0[tid].push_back(pos_arr[i]); // pos
            } else {
              if (i == start_read_num) {
                // Note: In order non-preserving mode, if the first read of
                // the block is a singleton, then the rest are too.
                subseq_0_0[tid].push_back(pos_arr[i]); // pos
                prevpos = pos_arr[i];
              } else {
                diffpos = pos_arr[i] - prevpos;
                subseq_0_0[tid].push_back(diffpos); // pos
                prevpos = pos_arr[i];
              }
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
            subseq_12_0[tid].push_back(6); // rtype = U
            subseq_7_0[tid].push_back(read_length_arr[i]); // rlen
            for (uint64_t j = 0; j < read_length_arr[i]; j++) {
              subseq_6_0[tid].push_back(char_to_int[(uint8_t)unaligned_arr[pos_arr[i] + j]]); // ureads
            }
          }
        } else {
          uint32_t i_p = i + num_reads/2;
          if (flag_arr[i] == false && flag_arr[i_p] == false) {
            // both reads unaligned (type U)
            subseq_12_0[tid].push_back(6); // rtype = U
            subseq_7_0[tid].push_back(read_length_arr[i]); // rlen
            subseq_7_0[tid].push_back(read_length_arr[i_p]); // rlen
            for (uint64_t j = 0; j < read_length_arr[i]; j++)
              subseq_6_0[tid].push_back(char_to_int[(uint8_t)unaligned_arr[pos_arr[i] + j]]); // ureads
            for (uint64_t j = 0; j < read_length_arr[i_p]; j++)
              subseq_6_0[tid].push_back(char_to_int[(uint8_t)unaligned_arr[pos_arr[i_p] + j]]); // ureads
          } else {
            if (flag_arr[i] == false || flag_arr[i_p] == false) {
              // HM
              subseq_12_0[tid].push_back(5); // rtype = HM
              bool swap_sequences = (flag_arr[i] == false); // swap if first is unaligned
              uint32_t i_aligned = swap_sequences?i_p:i;
              uint32_t i_unaligned = swap_sequences?i:i_p;
              subseq_7_0[tid].push_back(read_length_arr[i_aligned]); // rlen
              subseq_7_0[tid].push_back(read_length_arr[i_unaligned]); // rlen
              for (uint64_t j = 0; j < read_length_arr[i_unaligned]; j++)
                subseq_6_0[tid].push_back(char_to_int[(uint8_t)unaligned_arr[pos_arr[i_unaligned] + j]]); // ureads
              subseq_1_0[tid].push_back(rc_to_int[(uint8_t)RC_arr[i_aligned]]); // rcomp
              if (preserve_order) {
                subseq_0_0[tid].push_back(pos_arr[i_aligned]); // pos
              } else {
                if (i == start_read_num) {
                  // Note: In order non-preserving mode, if the first read of
                  // the block is a singleton, then the rest are too.
                  subseq_0_0[tid].push_back(pos_arr[i_aligned]); // pos
                  prevpos = pos_arr[i_aligned];
                } else {
                  diffpos = pos_arr[i_aligned] - prevpos;
                  subseq_0_0[tid].push_back(diffpos); // pos
                  prevpos = pos_arr[i_aligned];
                }
              }
              // for HM, always encode noise even if there is none
              for (uint16_t j = 0; j < noise_len_arr[i_aligned]; j++) {
                subseq_3_0[tid].push_back(0); // mmpos
                subseq_3_1[tid].push_back(noisepos_arr[pos_in_noise_arr[i_aligned] + j]);
                subseq_4_0[tid].push_back(0); // mmtype = Substitution
                subseq_4_1[tid].push_back(char_to_int[(uint8_t)noise_arr[pos_in_noise_arr[i_aligned] + j]]);
              }
              subseq_3_0[tid].push_back(1);
              
              // finally pair descriptors
              subseq_8_10[tid].push_back((int64_t)swap_sequences);
            } else {
              // both aligned - so type P or M
              bool swap_sequences = (pos_arr[i] > pos_arr[i_p]);
              uint32_t i_left = swap_sequences?i_p:i; 
              uint32_t i_right = swap_sequences?i:i_p; 
              subseq_7_0[tid].push_back(read_length_arr[i_left]); // rlen
              subseq_7_0[tid].push_back(read_length_arr[i_right]); // rlen
              subseq_1_0[tid].push_back(2*rc_to_int[(uint8_t)RC_arr[i_left]] + rc_to_int[(uint8_t)RC_arr[i_right]]); // rcomp
              if (preserve_order) {
                subseq_0_0[tid].push_back(pos_arr[i_left]); // pos
              } else {
                if (i == start_read_num) {
                  // Note: In order non-preserving mode, if the first read of
                  // the block is a singleton, then the rest are too.
                  subseq_0_0[tid].push_back(pos_arr[i_left]); // pos
                  prevpos = pos_arr[i_left];
                } else {
                  diffpos = pos_arr[i_left] - prevpos;
                  subseq_0_0[tid].push_back(diffpos); // pos
                  prevpos = pos_arr[i_left];
                }
              }
              if (noise_len_arr[i_left] == 0 && noise_len_arr[i_right] == 0)
                subseq_12_0[tid].push_back(1); // rtype = P 
              else {
                subseq_12_0[tid].push_back(3); // rtype = M               
                uint16_t abs_noise_pos = 0; 
                for (uint16_t j = 0; j < noise_len_arr[i_left]; j++) {
                  subseq_3_0[tid].push_back(0); // mmpos
                  subseq_3_1[tid].push_back(noisepos_arr[pos_in_noise_arr[i_left] + j]);
                  subseq_4_0[tid].push_back(0); // mmtype = Substitution
                  subseq_4_1[tid].push_back(char_to_int[(uint8_t)noise_arr[pos_in_noise_arr[i_left] + j]]);
                  abs_noise_pos += noisepos_arr[pos_in_noise_arr[i_left] + j];
                }
                for (uint16_t j = 0; j < noise_len_arr[i_right]; j++) {
                  subseq_3_0[tid].push_back(0); // mmpos
                  if (j == 0)
                    subseq_3_1[tid].push_back(read_length_arr[i_left] - abs_noise_pos + noisepos_arr[pos_in_noise_arr[i_right] + j]); // mmpos
                  else
                    subseq_3_1[tid].push_back(noisepos_arr[pos_in_noise_arr[i_right] + j]); //mmpos
                  subseq_4_0[tid].push_back(0); // mmtype = Substitution 
                  subseq_4_1[tid].push_back(char_to_int[(uint8_t)noise_arr[pos_in_noise_arr[i_right] + j]]);
                } 
                subseq_3_0[tid].push_back(1);
              }
              
              // finally pair descriptors
              int64_t delta = pos_arr[i_right] - pos_arr[i_left] - read_length_arr[i_left];
              if (delta <= 65535 && delta >= 0) {
                subseq_8_0[tid].push_back(8); // eru_delta
                subseq_8_11[tid].push_back(delta); // delta
              } else {
                subseq_8_0[tid].push_back(9); // eru_abs_pos 
                subseq_8_12[tid].push_back(pos_arr[i_right]); // abs_pos
              }
              subseq_8_10[tid].push_back((int64_t)swap_sequences);
            }
          }
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
      if (paired_end) {
        write_vector_to_file(subseq_8_0[tid], file_subseq_8_0 + '.' + std::to_string(block_num));
        write_vector_to_file(subseq_8_10[tid], file_subseq_8_10 + '.' + std::to_string(block_num));
        write_vector_to_file(subseq_8_11[tid], file_subseq_8_11 + '.' + std::to_string(block_num));
        write_vector_to_file(subseq_8_12[tid], file_subseq_8_12 + '.' + std::to_string(block_num));
      }
      write_vector_to_file(subseq_12_0[tid], file_subseq_12_0 + '.' + std::to_string(block_num));

      block_num += num_thr;
    }
  }  // end omp parallel


  // deallocate
  delete[] RC_arr;
  delete[] read_length_arr;
  delete[] flag_arr;
  delete[] pos_in_noise_arr;
  delete[] pos_arr;
  delete[] noise_len_arr;
  delete[] noise_arr;
  delete[] noisepos_arr;

  return;
}

}  // namespace spring
