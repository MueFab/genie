#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include "algorithms/SPRING/util.h"
#include "algorithms/SPRING/decode_read_streams.h"

namespace spring {

std::vector<std::string> decode_read_streams_se(const subsequences_se_t &subseq) {
  std::vector<std::string> decoded_reads;
  std::string refBuf;

  // int_to_char
  char int_to_char[5] = {'A','C','G','T','N'};

  // intialize iterators for subsequences
  auto subseq_0_0_it = subseq.subseq_0_0.begin();
  auto subseq_1_0_it = subseq.subseq_1_0.begin();
  auto subseq_3_0_it = subseq.subseq_3_0.begin();
  auto subseq_3_1_it = subseq.subseq_3_1.begin();
  auto subseq_4_0_it = subseq.subseq_4_0.begin();
  auto subseq_4_1_it = subseq.subseq_4_1.begin();
  auto subseq_6_0_it = subseq.subseq_6_0.begin();
  auto subseq_7_0_it = subseq.subseq_7_0.begin();

  // some state variables
  uint64_t abs_pos = 0;

  for (auto rtype: subseq.subseq_12_0) {
    if (rtype == 6) {
      // put in refBuf
      uint32_t rlen = (uint32_t)(*(subseq_7_0_it++)); // rlen
      for (uint32_t i = 0; i < rlen; i++) {
        refBuf.push_back(int_to_char[*(subseq_6_0_it++)]); // ureads
      }
    }
    else {
      // rtype can be 1 (P) or 3 (M)
      uint32_t rlen = (uint32_t)(*(subseq_7_0_it++)); // rlen
      int64_t rcomp = *(subseq_1_0_it++); // rcomp
      uint32_t pos = *(subseq_0_0_it++); // pos
      abs_pos += pos;
      std::string cur_read = refBuf.substr(abs_pos, rlen);

      if (rtype == 3) {
        // type M, so some mismatches
        uint32_t abs_mmpos = 0;
        while (true) {
          bool mmpos_flag = (bool)(*(subseq_3_0_it++));
          if (mmpos_flag == 1)
            break;
          uint32_t mmpos = (uint32_t)(*(subseq_3_1_it++));
          abs_mmpos += mmpos;
          uint32_t mmtype_0 = (uint32_t)(*(subseq_4_0_it++));
          if (mmtype_0 != 0) // i.e., not substitution
            throw std::runtime_error("Non zero mmtype encountered.");
          uint32_t mmtype_1 = (uint32_t)(*(subseq_4_1_it++));
          cur_read[abs_mmpos] = int_to_char[mmtype_1];
        }
      }
      // finally, reverse complement if needed
      if(rcomp == 1)
        cur_read = reverse_complement(cur_read, rlen);
      decoded_reads.push_back(cur_read);
    }
  }
  return decoded_reads;
}

void decompress_se_reads(const std::string &temp_dir, uint32_t num_blocks) {
  std::string basedir = temp_dir;
  std::string file_subseq_0_0 = basedir + "/subseq_0_0";
  std::string file_subseq_1_0 = basedir + "/subseq_1_0";
  std::string file_subseq_3_0 = basedir + "/subseq_3_0";
  std::string file_subseq_3_1 = basedir + "/subseq_3_1";
  std::string file_subseq_4_0 = basedir + "/subseq_4_0";
  std::string file_subseq_4_1 = basedir + "/subseq_4_1";
  std::string file_subseq_6_0 = basedir + "/subseq_6_0";
  std::string file_subseq_7_0 = basedir + "/subseq_7_0";
  std::string file_subseq_12_0 = basedir + "/subseq_12_0";
  std::string file_decompressed_reads = basedir + "/decompressed.reads";
  std::ofstream fout(file_decompressed_reads);
  subsequences_se_t subseq;
  for (uint32_t i = 0; i < num_blocks; i++) {
    subseq.subseq_0_0 = read_vector_from_file(file_subseq_0_0 + '.' + std::to_string(i));
    subseq.subseq_1_0 = read_vector_from_file(file_subseq_1_0 + '.' + std::to_string(i));
    subseq.subseq_3_0 = read_vector_from_file(file_subseq_3_0 + '.' + std::to_string(i));
    subseq.subseq_3_1 = read_vector_from_file(file_subseq_3_1 + '.' + std::to_string(i));
    subseq.subseq_4_0 = read_vector_from_file(file_subseq_4_0 + '.' + std::to_string(i));
    subseq.subseq_4_1 = read_vector_from_file(file_subseq_4_1 + '.' + std::to_string(i));
    subseq.subseq_6_0 = read_vector_from_file(file_subseq_6_0 + '.' + std::to_string(i));
    subseq.subseq_7_0 = read_vector_from_file(file_subseq_7_0 + '.' + std::to_string(i));
    subseq.subseq_12_0 = read_vector_from_file(file_subseq_12_0 + '.' + std::to_string(i));
    std::vector<std::string> decoded_reads = decode_read_streams_se(subseq);
    for (auto s: decoded_reads)
      fout << s << "\n";
  }
}

}  // namespace spring
