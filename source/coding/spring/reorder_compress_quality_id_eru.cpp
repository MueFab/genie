#ifdef GENIE_USE_OPENMP
#include <omp.h>
#endif
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "spring/reorder_compress_quality_id_eru.h"
#include "spring/id_tokenization.h"
#include "spring/util.h"

namespace spring {

void reorder_compress_quality_id(const std::string &temp_dir,
                                 const compression_params &cp) {
  // Read some parameters
  uint32_t numreads = cp.num_reads;
  int num_thr = cp.num_thr;
  bool preserve_id = cp.preserve_id;
  bool preserve_quality = cp.preserve_quality;
  bool paired_end = cp.paired_end;
  uint32_t num_reads_per_block = cp.num_reads_per_block;
  bool paired_id_match = cp.paired_id_match;
  bool preserve_order = cp.preserve_order;

  std::string basedir = temp_dir;

  std::string file_order = basedir + "/read_order.bin";
  if (paired_end) 
    file_order = basedir + "/read_order_single.bin";
  std::string file_id[2];
  std::string file_quality[2];
  file_id[0] = basedir + "/id_1";
  file_id[1] = basedir + "/id_2";
  file_quality[0] = basedir + "/quality_1";
  file_quality[1] = basedir + "/quality_2";
  std::string outfile_quality  = basedir + "/quality";
  std::string outfile_id = basedir + "/id";

#ifdef GENIE_USE_OPENMP
  omp_set_num_threads(num_thr);
#endif
  uint32_t numreads_modified = numreads;
  if (paired_end)
    numreads_modified = numreads/2; // to make things simpler below
  uint32_t *order_array;
  // array containing index mapping position in original fastq to
  // position after reordering
  order_array = new uint32_t [numreads_modified];
  if (!preserve_order) {
    generate_order_array(file_order, order_array, numreads_modified);
  } else {
    for (uint32_t i = 0; i < numreads_modified; i++)
      order_array[i] = i; // TODO: this is NOT the most efficient method for order preserving
  }

  if (preserve_quality) {
    std::cout << "Compressing qualities\n";
    uint32_t num_reads_per_file = numreads;
    if (!paired_end) {
      uint32_t str_array_size;
      str_array_size = (1 + (numreads / 4 - 1) / num_reads_per_block) * num_reads_per_block;
      // smallest multiple of num_reads_per_block bigger than numreads/4
      // numreads/4 chosen so that these many qualities/ids can be stored in
      // memory without exceeding the RAM consumption of reordering stage
      std::string *str_array = new std::string[str_array_size];
      // array to load ids and/or qualities into
      reorder_compress(file_quality[0], outfile_quality, num_reads_per_file, num_thr,
                       num_reads_per_block, str_array, str_array_size,
                       order_array, "quality");
      delete[] str_array;
    }
    else {
      uint32_t num_reads_by_2 = numreads/2; 
      uint32_t str_array_size;
      str_array_size = (1 + (num_reads_by_2 / 4 - 1) / num_reads_per_block) * num_reads_per_block;
      // smallest multiple of num_reads_per_block bigger than numreads/4
      // numreads/4 chosen so that these many qualities/ids can be stored in
      // memory without exceeding the RAM consumption of reordering stage
      std::string *str_array_1 = new std::string[str_array_size];
      std::string *str_array_2 = new std::string[str_array_size];
      // array to load ids and/or qualities into
      reorder_compress_quality_pe(file_quality, outfile_quality, num_reads_by_2, num_thr, num_reads_per_block, str_array_1, str_array_2, str_array_size, order_array);
      delete[] str_array_1;
      delete[] str_array_2;
    }
    remove(file_quality[0].c_str());
    if (paired_end) remove(file_quality[1].c_str());
  }
  if (preserve_id) {
    uint32_t str_array_size;
    str_array_size = (1 + (numreads / 4 - 1) / num_reads_per_block) * num_reads_per_block;
    // smallest multiple of num_reads_per_block bigger than numreads/4
    // numreads/4 chosen so that these many qualities/ids can be stored in
    // memory without exceeding the RAM consumption of reordering stage
    std::string *str_array = new std::string[str_array_size];
    // array to load ids and/or qualities into
    std::cout << "Compressing ids\n";
    uint32_t num_reads_per_file = numreads_modified;
    reorder_compress(file_id[0], outfile_id, num_reads_per_file, num_thr,
                     num_reads_per_block, str_array, str_array_size,
                     order_array, "id"); // treat id as single end always
    remove(file_id[0].c_str());
    if (paired_end && !paired_id_match) remove(file_id[1].c_str());
    delete[] str_array;
  }
  delete[] order_array;
  return;
}

void reorder_compress(const std::string &file_name, const std::string &outfilenameprefix,
                      const uint32_t &num_reads_per_file, const int &num_thr,
                      const uint32_t &num_reads_per_block,
                      std::string *str_array, const uint32_t &str_array_size,
                      uint32_t *order_array, const std::string &mode) {
  for (uint32_t i = 0; i <= num_reads_per_file / str_array_size; i++) {
    uint32_t num_reads_bin = str_array_size;
    if (i == num_reads_per_file / str_array_size)
      num_reads_bin = num_reads_per_file % str_array_size;
    if (num_reads_bin == 0) break;
    uint32_t start_read_bin = i * str_array_size;
    uint32_t end_read_bin = i * str_array_size + num_reads_bin;
    // Read the file and pick up lines corresponding to this bin
    std::ifstream f_in(file_name);
    std::string temp_str;
    for (uint32_t i = 0; i < num_reads_per_file; i++) {
      std::getline(f_in, temp_str);
      if (order_array[i] >= start_read_bin && order_array[i] < end_read_bin)
        str_array[order_array[i] - start_read_bin] = temp_str;
    }
    f_in.close();
#ifdef GENIE_USE_OPENMP
#pragma omp parallel
#endif
    {
#ifdef GENIE_USE_OPENMP
      uint64_t tid = omp_get_thread_num();
#else
      uint64_t tid = 0;
#endif
      uint64_t block_num_offset = start_read_bin / num_reads_per_block;
      uint64_t block_num = tid;
      bool done = false;
      while (!done) {
        uint64_t start_read_num = block_num * num_reads_per_block;
        uint64_t end_read_num = (block_num + 1) * num_reads_per_block;
        if (start_read_num >= num_reads_bin) break;
        if (end_read_num >= num_reads_bin) {
          done = true;
          end_read_num = num_reads_bin;
        }
        uint32_t num_reads_block = (uint32_t)(end_read_num - start_read_num);
        std::string outfilename =
            outfilenameprefix + "." + std::to_string(block_num_offset + block_num);

        if (mode == "id") {
          std::vector<int64_t> tokens[128][8];
          generate_read_id_tokens(str_array + start_read_num, num_reads_block, tokens);
          write_read_id_tokens_to_file(outfilename.c_str(), tokens);
        } else {
          std::ofstream fout_quality(outfilename);
	  for(uint32_t i = start_read_num; i < start_read_num + num_reads_block; i++)
	    fout_quality << str_array[i];
          fout_quality.close();
        }
        block_num += num_thr;
      }
    }  // omp parallel
  }
}

void reorder_compress_quality_pe(const std::string file_name[2], const std::string &outfilenameprefix,
                      const uint32_t &num_reads, const int &num_thr,
                      const uint32_t &num_reads_per_block,
                      std::string *str_array_1, std::string *str_array_2, const uint32_t &str_array_size,
                      uint32_t *order_array) {
  for (uint32_t i = 0; i <= num_reads / str_array_size; i++) {
    uint32_t num_reads_bin = str_array_size;
    if (i == num_reads / str_array_size)
      num_reads_bin = num_reads % str_array_size;
    if (num_reads_bin == 0) break;
    uint32_t start_read_bin = i * str_array_size;
    uint32_t end_read_bin = i * str_array_size + num_reads_bin;
    // Read the file and pick up lines corresponding to this bin
    std::ifstream f_in_1(file_name[0]);
    std::ifstream f_in_2(file_name[1]);
    std::string temp_str_1, temp_str_2;
    for (uint32_t i = 0; i < num_reads; i++) {
      std::getline(f_in_1, temp_str_1);
      std::getline(f_in_2, temp_str_2);
      if (order_array[i] >= start_read_bin && order_array[i] < end_read_bin) {
        str_array_1[order_array[i] - start_read_bin] = temp_str_1;
        str_array_2[order_array[i] - start_read_bin] = temp_str_2;
      }
    }
    f_in_1.close();
    f_in_2.close();
#ifdef GENIE_USE_OPENMP
#pragma omp parallel
#endif
    {
#ifdef GENIE_USE_OPENMP
      uint64_t tid = omp_get_thread_num();
#else
      uint64_t tid = 0;
#endif
      uint64_t block_num_offset = start_read_bin / num_reads_per_block;
      uint64_t block_num = tid;
      bool done = false;
      while (!done) {
        uint64_t start_read_num = block_num * num_reads_per_block;
        uint64_t end_read_num = (block_num + 1) * num_reads_per_block;
        if (start_read_num >= num_reads_bin) break;
        if (end_read_num >= num_reads_bin) {
          done = true;
          end_read_num = num_reads_bin;
        }
        uint32_t num_reads_block = (uint32_t)(end_read_num - start_read_num);
        std::string outfilename =
            outfilenameprefix + "." + std::to_string(block_num_offset + block_num);

        std::ofstream fout_quality(outfilename);
	for(uint32_t i = start_read_num; i < start_read_num + num_reads_block; i++) {
	  fout_quality << str_array_1[i];
	  fout_quality << str_array_2[i];
        }
        fout_quality.close();
        block_num += num_thr;
      }
    }  // omp parallel
  }
}
void generate_read_id_tokens(std::string *id_array, const uint32_t &num_ids, std::vector<int64_t> tokens[128][8]) {
  char prev_ID[1024] = {0};
  uint32_t prev_tokens_ptr[1024] = {0};
  for (uint32_t id_num = 0; id_num < num_ids; id_num++) {
    tokens[0][0].push_back(1); // DIFF
    if (id_num == 0)
      tokens[0][1].push_back(0); // DIFF 0 for first id
    else
      tokens[0][1].push_back(1); // DIFF 1 for rest of ids
    generate_id_tokens(prev_ID, prev_tokens_ptr, id_array[id_num], tokens);
  }
}

}  // namespace spring
