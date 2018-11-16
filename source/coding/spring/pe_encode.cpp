#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "spring/pe_encode.h"
#include "spring/util.h"

namespace spring {

void pe_encode(const std::string &temp_dir, const compression_params &cp) {
  // Read some parameters
  uint32_t numreads = cp.num_reads;

  std::string basedir = temp_dir;
  std::string file_order = basedir + "/read_order.bin";
  std::string file_order_single = basedir + "/read_order_single.bin";
  // this file is used for reordering ids and quality
  // this will just store numreads/2 values as the reordering of both files is same
  // also used for generate new fastq

  uint32_t *order_array = new uint32_t[numreads];
  // stores index mapping position in reordered file to
  // position in original file
  // later stores index generated in this step - maps position
  // in reordered file to position in intended decompressed file

  uint32_t *inverse_order_array = new uint32_t[numreads];
  // stores index mapping position in original file to
  // position in reordered file

  std::ifstream fin_order(file_order, std::ios::binary);
  uint32_t order;
  for (uint32_t i = 0; i < numreads; i++) {
    fin_order.read((char *)&order, sizeof(uint32_t));
    order_array[i] = order;
    inverse_order_array[order] = i;
  }
  fin_order.close();

  // Find new order array by going from left to right and picking reads whose pair hasn't been picked
  bool *already_seen_array = new bool [numreads]();
  uint32_t *new_order_array = new uint32_t [numreads];
  uint32_t pos_in_new_order = 0;
  std::ofstream fout_order_single(file_order_single, std::ios::binary);
  for (uint32_t i = 0; i < numreads; i++) {
    uint32_t current = order_array[i];
    uint32_t pair = (current>=numreads/2)?(current-numreads/2):(current+numreads/2);
    already_seen_array[current] = true;
    if (!already_seen_array[pair]) {
      // current read is to the left of pair
      uint32_t pos_of_pair_after_reordering = inverse_order_array[pair];
      if (pos_of_pair_after_reordering < i)
        throw std::runtime_error("Error in pe encode index");
      if (current < pair) {
        new_order_array[i] = pos_in_new_order;
        new_order_array[pos_of_pair_after_reordering] = pos_in_new_order + numreads/2;
        fout_order_single.write((char*)&current, sizeof(uint32_t));
      } else {
        new_order_array[i] = pos_in_new_order + numreads/2;
        new_order_array[pos_of_pair_after_reordering] = pos_in_new_order;
        fout_order_single.write((char*)&pair, sizeof(uint32_t));
      }
      pos_in_new_order++;
    }
  }
  fout_order_single.close();
  // Write to file
  std::ofstream fout_order(file_order, std::ios::binary);
  for (uint32_t i = 0; i < numreads; i++) {
    fout_order.write((char *)&new_order_array[i], sizeof(uint32_t));
  }
  fout_order.close();

  delete[] order_array;
  delete[] inverse_order_array;
  delete[] already_seen_array;
  delete[] new_order_array;
  return;
}

}  // namespace spring
