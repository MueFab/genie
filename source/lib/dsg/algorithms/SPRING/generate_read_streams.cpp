#include <omp.h>
#include <cmath>  // abs
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "algorithms/SPRING/generate_read_streams.h"
#include "algorithms/SPRING/util.h"

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
  uint32_t num_reads_by_2 = num_reads / 2;
  int num_thr = cp.num_thr;
  bool paired_end = cp.paired_end;
  if(paired_end)
    throw std::runtime_error("PE: NOT IMPLEMENTED");
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
  if (paired_end) f_order.open(file_order, std::ios::binary);
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
    if (paired_end)
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
    if (!(paired_end)) order++;
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
    if (paired_end)
      f_order.read((char *)&order, sizeof(uint32_t));
    f_readlength.read((char *)&read_length, sizeof(uint16_t));
    read_length_arr[order] = read_length;
    pos_arr[order] = current_pos_in_unaligned_arr;
    current_pos_in_unaligned_arr += read_length;
    flag_arr[order] = false;  // unaligned
    if (!(paired_end)) order++;
  }
  if (paired_end) f_order.close();
  f_readlength.close();

  // delete old streams
  remove(file_noise.c_str());
  remove(file_noisepos.c_str());
  remove(file_RC.c_str());
  remove(file_order.c_str());
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
      if (!paired_end) {
        if (start_read_num >= num_reads) break;
        if (end_read_num >= num_reads) {
          done = true;
          end_read_num = num_reads;
        }
      } else {
        if (start_read_num >= num_reads_by_2) break;
        if (end_read_num >= num_reads_by_2) {
          done = true;
          end_read_num = num_reads_by_2;
        }
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
        subseq_12_0[tid].push_back(5); // rtype
        for (uint64_t i = seq_start; i < seq_end; i++)
          subseq_6_0[tid].push_back(char_to_int[(uint8_t)seq[i]]); // ureads
      }
      uint64_t prevpos = 0, diffpos;
      // Write streams
      for (uint64_t i = start_read_num; i < end_read_num; i++) {
        if (!paired_end) {
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
              subseq_12_0[tid].push_back(3); // rtype = P
              for (uint16_t j = 0; j < noise_len_arr[i]; j++) {
                subseq_3_0[tid].push_back(0); // mmpos
                subseq_3_1[tid].push_back(noisepos_arr[pos_in_noise_arr[i] + j]);
                subseq_4_0[tid].push_back(0); // mmtype = Substitution
                subseq_4_1[tid].push_back(char_to_int[(uint8_t)noise_arr[pos_in_noise_arr[i] + j]]);
              }
              subseq_3_0[tid].push_back(1);
            }
          } else {
            subseq_12_0[tid].push_back(5); // rtype
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
/*	else {
          uint64_t i_p = num_reads_by_2 + i;  // i_pair
          f_readlength.write((char *)&read_length_arr[i], sizeof(uint16_t));
          f_readlength.write((char *)&read_length_arr[i_p], sizeof(uint16_t));
          int64_t pos_pair = (int64_t)pos_arr[i_p] - (int64_t)pos_arr[i];
          int flag;
          if (flag_arr[i] && flag_arr[i_p] && std::abs(pos_pair) < 32767)
            flag = 0;
          else if (flag_arr[i] && flag_arr[i_p])
            flag = 1;
          else if (!flag_arr[i] && !flag_arr[i_p])
            flag = 2;
          else if (flag_arr[i] && !flag_arr[i_p])
            flag = 3;
          else if (!flag_arr[i] && flag_arr[i_p])
            flag = 4;
          f_flag << flag;
          if (flag == 0 && paired_end) {
            int16_t pos_pair_16 = (int16_t)pos_pair;
            f_pos_pair.write((char *)&pos_pair_16, sizeof(int16_t));
            if (RC_arr[i] != RC_arr[i_p])
              f_RC_pair << '0';
            else
              f_RC_pair << '1';
          }
          if (flag == 0 || flag == 1 || flag == 3) {
            // read 1 is aligned
            if (i == start_read_num) {
              // Note: In order non-preserving mode, if read 1 of
              // first pair the block is a singleton, then the rest are too.
              f_pos.write((char *)&pos_arr[i], sizeof(uint64_t));
              prevpos = pos_arr[i];
            } else {
              diffpos = pos_arr[i] - prevpos;
              if (diffpos < 65535) {
                diffpos_16 = (uint16_t)diffpos;
                f_pos.write((char *)&diffpos_16, sizeof(uint16_t));
              } else {
                diffpos_16 = 65535;
                f_pos.write((char *)&diffpos_16, sizeof(uint16_t));
                f_pos.write((char *)&pos_arr[i], sizeof(uint64_t));
              }
              prevpos = pos_arr[i];
            }
            
            for (uint16_t j = 0; j < noise_len_arr[i]; j++) {
              f_noise << noise_arr[pos_in_noise_arr[i] + j];
              f_noisepos.write((char *)&noisepos_arr[pos_in_noise_arr[i] + j],
                               sizeof(uint16_t));
            }
            f_noise << "\n";
            f_RC << RC_arr[i];
          } else {
            // read 1 is unaligned
            f_unaligned.write(unaligned_arr + pos_arr[i], read_length_arr[i]);
          }

          if (flag == 0 || flag == 1 || flag == 4) {
            // read 2 is aligned
            for (uint16_t j = 0; j < noise_len_arr[i_p]; j++) {
              f_noise << noise_arr[pos_in_noise_arr[i_p] + j];
              f_noisepos.write((char *)&noisepos_arr[pos_in_noise_arr[i_p] + j],
                               sizeof(uint16_t));
            }
            f_noise << "\n";
            if (flag == 1 || flag == 4) {
              // read 2 is aligned but not paired properly
              f_pos.write((char *)&pos_arr[i_p], sizeof(uint64_t));
              f_RC << RC_arr[i_p];
            }
          } else {
            // read 2 is unaligned
            f_unaligned.write(unaligned_arr + pos_arr[i_p],
                              read_length_arr[i_p]);
          }
        }
*/
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
