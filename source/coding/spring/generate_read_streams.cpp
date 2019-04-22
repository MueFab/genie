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
#include <map>
#include <numeric>

#include "spring/generate_read_streams.h"
#include "spring/util.h"

namespace spring {

std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> generate_read_streams(const std::string &temp_dir, const compression_params &cp) {
  if (!cp.paired_end)
    return generate_read_streams_se(temp_dir,cp);
  else
    return generate_read_streams_pe(temp_dir,cp);
}

std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> generate_read_streams_se(const std::string &temp_dir, const compression_params &cp) {

  std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> descriptorFilesPerAU;
  std::string basedir = temp_dir;

  std::vector<std::array<uint8_t,2>> subseq_indices = {
      {0,0}, // pos
      {1,0}, // rcomp
      {3,0}, // mmpos
      {3,1}, // mmpos
      {4,0}, // mmtype
      {4,1}, // mmtype
      {6,0}, // ureads
      {7,0}, // rlen
      {12,0} // rtype
  };

  std::string file_subseq_prefix = basedir + "/subseq";

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
    std::map<uint8_t, std::map<uint8_t, std::vector<int64_t>>> subseq_vector;
    for (auto arr : subseq_indices)
        subseq_vector[arr[0]][arr[1]] = std::vector<int64_t> ();

    while (!done) {
      std::map<uint8_t, std::map<uint8_t, std::string>> listDescriptorFiles;
      //clear vectors
      for (auto arr : subseq_indices)
          subseq_vector[arr[0]][arr[1]].clear();

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
        subseq_vector[7][0].push_back(seq_end - seq_start); // rlen
        subseq_vector[12][0].push_back(5); // rtype
        for (uint64_t i = seq_start; i < seq_end; i++)
          subseq_vector[6][0].push_back(char_to_int[(uint8_t)seq[i]]); // ureads
      }
      uint64_t prevpos = 0, diffpos;
      // Write streams
      for (uint64_t i = start_read_num; i < end_read_num; i++) {
        if (flag_arr[i] == true) {
          subseq_vector[7][0].push_back(read_length_arr[i]); // rlen
          subseq_vector[1][0].push_back(rc_to_int[(uint8_t)RC_arr[i]]); // rcomp
          if (i == start_read_num) {
            // Note: In order non-preserving mode, if the first read of
            // the block is a singleton, then the rest are too.
            subseq_vector[0][0].push_back(0);
            prevpos = pos_arr[i];
          } else {
            diffpos = pos_arr[i] - prevpos;
            subseq_vector[0][0].push_back(diffpos); // pos
            prevpos = pos_arr[i];
          }
          if (noise_len_arr[i] == 0)
            subseq_vector[12][0].push_back(1); // rtype = P
          else {
            subseq_vector[12][0].push_back(3); // rtype = M
            for (uint16_t j = 0; j < noise_len_arr[i]; j++) {
              subseq_vector[3][0].push_back(0); // mmpos
              if (j == 0)
                  subseq_vector[3][1].push_back(noisepos_arr[pos_in_noise_arr[i] + j]);
              else
                  subseq_vector[3][1].push_back(noisepos_arr[pos_in_noise_arr[i] + j]-1); // decoder adds +1
              subseq_vector[4][0].push_back(0); // mmtype = Substitution
              subseq_vector[4][1].push_back(char_to_int[(uint8_t)noise_arr[pos_in_noise_arr[i] + j]]);
            }
            subseq_vector[3][0].push_back(1);
          }
        } else {
          subseq_vector[12][0].push_back(5); // rtype
          subseq_vector[7][0].push_back(read_length_arr[i]); // rlen
          for (uint64_t j = 0; j < read_length_arr[i]; j++) {
            subseq_vector[6][0].push_back(char_to_int[(uint8_t)unaligned_arr[pos_arr[i] + j]]); // ureads
          }
          subseq_vector[0][0].push_back(seq_end - prevpos); // pos
          subseq_vector[1][0].push_back(0); // rcomp
          subseq_vector[7][0].push_back(read_length_arr[i]); // rlen
          subseq_vector[12][0].push_back(1); // rtype = P
          prevpos = seq_end;
          seq_end = prevpos + read_length_arr[i];
        }
      }
      // write vectors to files
      for (auto arr : subseq_indices) {
        std::string filename = file_subseq_prefix + "." + std::to_string(block_num) + "." +
            std::to_string(arr[0]) + "." + std::to_string(arr[1]);
        listDescriptorFiles[arr[0]][arr[1]] = filename;
        write_vector_to_file(subseq_vector[arr[0]][arr[1]], filename);
      }

      block_num += num_thr;
#ifdef GENIE_USE_OPENMP
#pragma omp critical
#endif
{
      descriptorFilesPerAU.push_back(listDescriptorFiles);
}
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

  return descriptorFilesPerAU;
}


std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> generate_read_streams_pe(const std::string &temp_dir,
                              const compression_params &cp) {

// basic approach: start looking at reads from left to right. If current is aligned but
// pair is unaligned, pair is kept at the end current AU and stored in different record.
// We try to keep number of records in AU = num_reads_per_block (without counting the the unaligned
// pairs above.
// As we scan to right, if we come across a read whose pair has already been seen in the same AU
// and the gap is < 32768 (and non-ovelapping since delta >= 0), then we add this to the paired read's
// genomic record. Finally when we come to unaligned reads whose pair is also unaligned, we store them
// in same genomic record.

  std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> descriptorFilesPerAU;
  std::string basedir = temp_dir;

  std::vector<std::array<uint8_t,2>> subseq_indices = {
      {0,0}, // pos
      {1,0}, // rcomp
      {3,0}, // mmpos
      {3,1}, // mmpos
      {4,0}, // mmtype
      {4,1}, // mmtype
      {6,0}, // ureads
      {7,0}, // rlen
	  {8,0}, // pair
	  {8,1}, // pair
	  {8,2}, // pair
	  {8,3}, // pair
	  {8,4}, // pair
	  {8,5}, // pair
	  {8,6}, // pair
	  {8,7}, // pair
      {12,0} // rtype
  };
  std::string file_subseq_prefix = basedir + "/subseq";

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
        if (block_num[pair] == current_block_num && (pos_arr[current] >= pos_arr[pair]) && (pos_arr[current] - pos_arr[pair] < 32768)) {
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
  // id:
  std::ofstream f_blocks_id(file_blocks_id, std::ios::binary);
  // store block start and end positions (measured in terms of records since 1 record = 1 id)
  for (uint32_t i = 0; i < block_start.size(); i++) {
    f_blocks_id.write((char*)&block_start[i], sizeof(uint32_t));
    f_blocks_id.write((char*)&block_end[i], sizeof(uint32_t));
    std::ofstream f_order_id(file_order_id + "." + std::to_string(i), std::ios::binary);
    // store order
    for (uint32_t j = block_start[i]; j < block_end[i]; j++) {
      uint32_t current = read_index_genomic_record[j];
      uint32_t pair = (current < num_reads_by_2)?(current + num_reads_by_2):(current - num_reads_by_2);
      // just write the min of current and pair
      uint32_t min_index = (current > pair)?pair:current;
      f_order_id.write((char*)&min_index, sizeof(uint32_t));
    }
    f_order_id.close();
  }
  f_blocks_id.close();

  std::vector<uint32_t> count_same_rec (num_thr, 0);
  std::vector<uint32_t> count_split_same_AU (num_thr, 0);
  std::vector<uint32_t> count_split_diff_AU (num_thr, 0);

  // PE step 4: Now generate read streams and compress blocks in parallel
#ifdef GENIE_USE_OPENMP
  omp_set_num_threads(num_thr);
#endif

#ifdef GENIE_USE_OPENMP
#pragma omp parallel
#endif
  {
#ifdef GENIE_USE_OPENMP
    uint64_t tid = omp_get_thread_num();
#else
    uint64_t tid = 0;
#endif
    uint64_t cur_block_num = tid;
    std::map<uint8_t, std::map<uint8_t, std::vector<int64_t>>> subseq_vector;
    for (auto arr : subseq_indices)
	  subseq_vector[arr[0]][arr[1]] = std::vector<int64_t> ();
    while (true) {
	  std::map<uint8_t, std::map<uint8_t, std::string>> listDescriptorFiles;
      //clear vectors
	  for (auto arr : subseq_indices)
		subseq_vector[arr[0]][arr[1]].clear();

      if (cur_block_num >= block_start.size())
        break;

      // first find the seq
      uint64_t seq_start = block_seq_start[cur_block_num], seq_end = block_seq_end[cur_block_num];
      if (seq_start != seq_end) {
        // not all unaligned
        subseq_vector[7][0].push_back(seq_end - seq_start); // rlen
        subseq_vector[12][0].push_back(5); // rtype
        for (uint64_t i = seq_start; i < seq_end; i++)
          subseq_vector[6][0].push_back(char_to_int[(uint8_t)seq[i]]); // ureads
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
            subseq_vector[0][0].push_back(0); // pos
            prevpos = pos_arr[current];
          } else {
            diffpos = pos_arr[current] - prevpos;
            subseq_vector[0][0].push_back(diffpos); // pos
            prevpos = pos_arr[current];
          }
        }
        if ((block_num[current] == block_num[pair]) && (genomic_record_index[current] == genomic_record_index[pair])) {
          // both reads in same record
          if (flag_arr[current] == false) {
            // Case 1: both unaligned
            subseq_vector[12][0].push_back(5); // rtype
            subseq_vector[7][0].push_back(read_length_arr[current] + read_length_arr[pair]); // rlen
            for (uint64_t j = 0; j < read_length_arr[current]; j++) {
              subseq_vector[6][0].push_back(char_to_int[(uint8_t)unaligned_arr[pos_arr[current] + j]]); // ureads
            }
            for (uint64_t j = 0; j < read_length_arr[pair]; j++) {
              subseq_vector[6][0].push_back(char_to_int[(uint8_t)unaligned_arr[pos_arr[pair] + j]]); // ureads
            }
            subseq_vector[0][0].push_back(seq_end - prevpos); // pos
            subseq_vector[1][0].push_back(0); // rcomp
			subseq_vector[1][0].push_back(0); // rcomp
            subseq_vector[7][0].push_back(read_length_arr[current]); // rlen
            subseq_vector[7][0].push_back(read_length_arr[pair]); // rlen
            subseq_vector[12][0].push_back(1); // rtype = P
            subseq_vector[8][0].push_back(0); // pair decoding case same_rec
            bool read_1_first = true;
            uint16_t delta = read_length_arr[current];
            subseq_vector[8][1].push_back(!(read_1_first)+2*delta); // pair
            prevpos = seq_end;
            seq_end = prevpos + read_length_arr[current] + read_length_arr[pair];
          } else {
            // Case 2: both aligned
            subseq_vector[7][0].push_back(read_length_arr[current]); // rlen
            subseq_vector[7][0].push_back(read_length_arr[pair]); // rlen
			subseq_vector[1][0].push_back(rc_to_int[(uint8_t)RC_arr[current]]); // rcomp
			subseq_vector[1][0].push_back(rc_to_int[(uint8_t)RC_arr[pair]]); // rcomp
            if (noise_len_arr[current] == 0 && noise_len_arr[pair] == 0)
              subseq_vector[12][0].push_back(1); // rtype = P
            else {
              subseq_vector[12][0].push_back(3); // rtype = M
			  for (int k = 0; k < 2; k++) {
				uint32_t ind = k?pair:current;
                for (uint16_t j = 0; j < noise_len_arr[ind]; j++) {
                  subseq_vector[3][0].push_back(0); // mmpos
				  if (j == 0)
                  	subseq_vector[3][1].push_back(noisepos_arr[pos_in_noise_arr[ind] + j]); // mmpos
				  else
					subseq_vector[3][1].push_back(noisepos_arr[pos_in_noise_arr[ind] + j]-1); // mmpos
                  subseq_vector[4][0].push_back(0); // mmtype = Substitution
                  subseq_vector[4][1].push_back(char_to_int[(uint8_t)noise_arr[pos_in_noise_arr[ind] + j]]);
                }
			 	subseq_vector[3][0].push_back(1); // mmpos
			  }
            }
            bool read_1_first = (current < pair);
            uint16_t delta = pos_arr[pair] - pos_arr[current];
            subseq_vector[8][0].push_back(0); // pair decoding case same_rec
            subseq_vector[8][1].push_back(!(read_1_first)+2*delta); // pair
            count_same_rec[tid]++;
          }
        } else {
          // only one read in genomic record
          if (flag_arr[current] == true) {
            subseq_vector[7][0].push_back(read_length_arr[current]); // rlen
            subseq_vector[1][0].push_back(rc_to_int[(uint8_t)RC_arr[current]]); // rcomp
            if (noise_len_arr[current] == 0)
              subseq_vector[12][0].push_back(1); // rtype = P
            else {
              subseq_vector[12][0].push_back(3); // rtype = M
              for (uint16_t j = 0; j < noise_len_arr[current]; j++) {
                subseq_vector[3][0].push_back(0); // mmpos
				if (j == 0)
                  subseq_vector[3][1].push_back(noisepos_arr[pos_in_noise_arr[current] + j]); // mmpos
				else
                  subseq_vector[3][1].push_back(noisepos_arr[pos_in_noise_arr[current] + j] - 1); // mmpos
                subseq_vector[4][0].push_back(0); // mmtype = Substitution
                subseq_vector[4][1].push_back(char_to_int[(uint8_t)noise_arr[pos_in_noise_arr[current] + j]]);
              }
              subseq_vector[3][0].push_back(1);
            }
          } else {
            subseq_vector[12][0].push_back(5); // rtype
            subseq_vector[7][0].push_back(read_length_arr[current]); // rlen
            for (uint64_t j = 0; j < read_length_arr[current]; j++) {
              subseq_vector[6][0].push_back(char_to_int[(uint8_t)unaligned_arr[pos_arr[current] + j]]); // ureads
            }
            subseq_vector[0][0].push_back(seq_end - prevpos); // pos
            subseq_vector[1][0].push_back(0); // rcomp
            subseq_vector[7][0].push_back(read_length_arr[current]); // rlen
            subseq_vector[12][0].push_back(1); // rtype = P
            prevpos = seq_end;
            seq_end = prevpos + read_length_arr[current];
          }

          // pair subsequences
          bool same_block = (block_num[current] == block_num[pair]);
          if (same_block)
              count_split_same_AU[tid]++;
          else
              count_split_diff_AU[tid]++;

          bool read_1_first = (current < pair);
          if (same_block && !read_1_first) {
            subseq_vector[8][0].push_back(1); // R1_split
            subseq_vector[8][2].push_back(genomic_record_index[pair]);
          } else if (same_block && read_1_first) {
            subseq_vector[8][0].push_back(2); // R2_split
            subseq_vector[8][3].push_back(genomic_record_index[pair]);
          } else if (!same_block && !read_1_first) {
            subseq_vector[8][0].push_back(3); // R1_diff_ref_seq
            subseq_vector[8][4].push_back(block_num[pair]);
            subseq_vector[8][6].push_back(genomic_record_index[pair]);
          } else {
            subseq_vector[8][0].push_back(4); // R2_diff_ref_seq
            subseq_vector[8][5].push_back(block_num[pair]);
            subseq_vector[8][7].push_back(genomic_record_index[pair]);
          }
        }
      }
      // write vectors to files
	  for (auto arr : subseq_indices) {
		std::string filename = file_subseq_prefix + "." + std::to_string(cur_block_num) + "." +
			std::to_string(arr[0]) + "." + std::to_string(arr[1]);
		listDescriptorFiles[arr[0]][arr[1]] = filename;
		write_vector_to_file(subseq_vector[arr[0]][arr[1]], filename);
	  }

      cur_block_num += num_thr;
#ifdef GENIE_USE_OPENMP
#pragma omp critical
#endif
{
      descriptorFilesPerAU.push_back(listDescriptorFiles);
}
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

  std::cout << "count_same_rec: " << std::accumulate(count_same_rec.begin(),count_same_rec.end(), 0) << "\n";
  std::cout << "count_split_same_AU: " << std::accumulate(count_split_same_AU.begin(), count_split_same_AU.end(), 0) << "\n";
  std::cout << "count_split_diff_AU: " << std::accumulate(count_split_diff_AU.begin(), count_split_diff_AU.end(), 0) << "\n";

  return descriptorFilesPerAU;
}

}  // namespace spring
