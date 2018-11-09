#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include "spring/util.h"
#include "spring/decode_eru.h"
#include "spring/id_tokenization.h"
#include "fileio/fastq_record.h"

namespace spring {

std::vector<dsg::input::fastq::FastqRecord> decode_streams(const subsequences_t &subseq, const std::string &ref, bool eru_abs_flag, bool paired_end) {
  std::vector<dsg::input::fastq::FastqRecord> decoded_records;
  std::string cur_read[2];
  std::string cur_quality[2];
  std::string cur_ID;
  dsg::input::fastq::FastqRecord cur_record;
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
  auto subseq_8_0_it = subseq.subseq_8_0.begin();
  auto subseq_8_10_it = subseq.subseq_8_10.begin();
  auto subseq_8_11_it = subseq.subseq_8_11.begin();
  auto subseq_8_12_it = subseq.subseq_8_12.begin();
  uint32_t pos_in_tokens_array[128][8];
  for (int i = 0; i < 128; i++)
    for (int j = 0; j < 8; j++)
      pos_in_tokens_array[i][j] = 0;
  uint64_t pos_in_quality_arr = 0;
  std::string prev_ID;
  uint32_t prev_tokens_ptr[256] = {0};
  uint32_t prev_tokens_len[256] = {0};

  // some state variables
  uint64_t abs_pos = 0;

  int number_of_record_segments = paired_end?2:1;

  for (auto rtype: subseq.subseq_12_0) {
    uint32_t rlen[2];
    for (int i = 0; i < number_of_record_segments; i++) {
      rlen[i] = (uint32_t)(*(subseq_7_0_it++)); // rlen
    }
    
    //get ID
    cur_ID = decode_id_tokens(prev_ID, prev_tokens_ptr, prev_tokens_len, subseq.tokens, pos_in_tokens_array);

    if (rtype == 6) {
      // class U
      for (int i = 0; i < number_of_record_segments; i++)
        cur_read[i] = decode_type_U(subseq_6_0_it, rlen[i], int_to_char);
    } else {
      // rtype can be 1 (P) or 3 (M) or 5 (HM)  
      bool swap_sequences = false;
      int64_t pairing_decoding_case;
      uint16_t pair_delta;
      uint64_t pair_abs_pos;

      int64_t rcomp = *(subseq_1_0_it++); // rcomp
      uint64_t pos = *(subseq_0_0_it++); // pos
      if (!eru_abs_flag)
        abs_pos += pos;
      else 
        abs_pos = pos;
      cur_read[0] = ref.substr(abs_pos, rlen[0]);
      if (paired_end) {
        if (rtype == 5) // HM
          pairing_decoding_case = 10; // eru_HM
        else
          pairing_decoding_case = *(subseq_8_0_it++);
        if (pairing_decoding_case == 8) {
          // eru_delta
          pair_delta = (uint16_t)(*subseq_8_11_it++);
          swap_sequences = (bool)(*subseq_8_10_it++);
          cur_read[1] = ref.substr(abs_pos + rlen[0] + pair_delta, rlen[1]);
        } else if (pairing_decoding_case == 9) {
          // eru_abs_pos
          pair_abs_pos = (uint64_t)(*subseq_8_12_it++);
          swap_sequences = (bool)(*subseq_8_10_it++);
          cur_read[1] = ref.substr(pair_abs_pos, rlen[1]);
        } else if (pairing_decoding_case == 10) {
          // eru_HM
          swap_sequences = (bool)(*subseq_8_10_it++);
        }
      }
      int number_of_mapped_segments = number_of_record_segments;
      bool rev_comp[2] = {0, 0};
      if (rtype == 5) {
        // HM
        if (!paired_end)
          throw std::runtime_error("Error: HM type for unpaired data");
        number_of_mapped_segments = 1;
        cur_read[1] =  decode_type_U(subseq_6_0_it, rlen[1], int_to_char);
        rtype = 3; // type M for the mapped read (I not supported)
      }
      for (int i = 0; i < number_of_mapped_segments; i++)
        rev_comp[i] = (rcomp >> (number_of_mapped_segments - i - 1)) & 1;

      if (rtype == 3) {
        // type M, so some mismatches
        int i = 0; // which read we are working with (first/second)
        uint32_t abs_mmpos = 0;
        while (true) {
          bool mmpos_flag = (bool)(*(subseq_3_0_it++));
          if (mmpos_flag == 1)
            break;
          uint32_t mmpos = (uint32_t)(*(subseq_3_1_it++));
          abs_mmpos += mmpos;
          if (i == 0 && abs_mmpos >= rlen[0]) {
            if (number_of_mapped_segments != 2)
              throw std::runtime_error("Error in mmpos");
            i = 1;
            abs_mmpos -= rlen[0];
          }
          uint32_t mmtype_0 = (uint32_t)(*(subseq_4_0_it++));
          if (mmtype_0 != 0) // i.e., not substitution
            throw std::runtime_error("Non zero mmtype encountered.");
          uint32_t mmtype_1 = (uint32_t)(*(subseq_4_1_it++));
          cur_read[i][abs_mmpos] = int_to_char[mmtype_1];
        }
      }
      // finally, reverse complement if needed
      for (int i = 0; i < number_of_record_segments; i++) {
        if (rev_comp[i] == 1)
          cur_read[i] = reverse_complement(cur_read[i], rlen[i]);
      }
      if (paired_end && swap_sequences) {
        cur_read[0].swap(cur_read[1]);
        std::swap(rlen[0], rlen[1]);
      }
    }
    for (int i = 0; i < number_of_record_segments; i++) {
      // get quality
      cur_quality[i] = subseq.quality_arr.substr(pos_in_quality_arr, rlen[i]);
      pos_in_quality_arr += rlen[i];
    }

    for (int i = 0; i < number_of_record_segments; i++) {
      cur_record.title = cur_ID;
      cur_record.sequence = cur_read[i];
      cur_record.qualityScores = cur_quality[i];
      decoded_records.push_back(cur_record);
    }
  }
  return decoded_records;
}

std::string decode_type_U(std::vector<int64_t>::const_iterator &subseq_6_0_it, uint32_t rlen, char int_to_char[]) {
  std::string cur_read;
  cur_read.resize(rlen);
  for (uint32_t i = 0; i < rlen; i++) {
    cur_read[i] = int_to_char[*(subseq_6_0_it++)]; // ureads
  }
  return cur_read;
}

std::string decode_ref_AU (const std::vector<int64_t> &subseq_6_0, const std::vector<int64_t> &subseq_7_0) {
  // int_to_char
  char int_to_char[5] = {'A','C','G','T','N'};
  std::string ref;
  auto subseq_6_0_it = subseq_6_0.begin();
  for (auto rlen: subseq_7_0) {
    for (uint64_t i = 0; i < rlen; i++)
      ref.push_back(int_to_char[*(subseq_6_0_it++)]);
  }
  return ref;
}

void decompress(const std::string &temp_dir, const std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> &ref_descriptorFilesPerAU, uint32_t num_blocks, bool eru_abs_flag, bool paired_end) {
  std::string basedir = temp_dir;
  std::string file_subseq_0_0 = basedir + "/subseq_0_0";
  std::string file_subseq_1_0 = basedir + "/subseq_1_0";
  std::string file_subseq_3_0 = basedir + "/subseq_3_0";
  std::string file_subseq_3_1 = basedir + "/subseq_3_1";
  std::string file_subseq_4_0 = basedir + "/subseq_4_0";
  std::string file_subseq_4_1 = basedir + "/subseq_4_1";
  std::string file_subseq_6_0 = basedir + "/subseq_6_0";
  std::string file_subseq_7_0 = basedir + "/subseq_7_0";
  std::string file_subseq_8_0 = basedir + "/subseq_8_0";
  std::string file_subseq_8_10 = basedir + "/subseq_8_10";
  std::string file_subseq_8_11 = basedir + "/subseq_8_11";
  std::string file_subseq_8_12 = basedir + "/subseq_8_12";
  std::string file_subseq_12_0 = basedir + "/subseq_12_0";
  std::string file_quality = basedir + "/quality";
  std::string file_id = basedir + "/id";
  std::string file_decompressed_fastq = basedir + "/decompressed.fastq";
  
  // decode ref
  std::string ref;
  for (auto listDescriptorFiles: ref_descriptorFilesPerAU) {
    auto subseq_6_0 = read_vector_from_file(listDescriptorFiles[6][0]);
    auto subseq_7_0 = read_vector_from_file(listDescriptorFiles[7][0]);
    ref.append(decode_ref_AU(subseq_6_0, subseq_7_0));
  }
  std::ofstream fout(file_decompressed_fastq);

  subsequences_t subseq;
  for (uint32_t i = 0; i < num_blocks; i++) {
    subseq.subseq_0_0 = read_vector_from_file(file_subseq_0_0 + '.' + std::to_string(i));
    subseq.subseq_1_0 = read_vector_from_file(file_subseq_1_0 + '.' + std::to_string(i));
    subseq.subseq_3_0 = read_vector_from_file(file_subseq_3_0 + '.' + std::to_string(i));
    subseq.subseq_3_1 = read_vector_from_file(file_subseq_3_1 + '.' + std::to_string(i));
    subseq.subseq_4_0 = read_vector_from_file(file_subseq_4_0 + '.' + std::to_string(i));
    subseq.subseq_4_1 = read_vector_from_file(file_subseq_4_1 + '.' + std::to_string(i));
    subseq.subseq_6_0 = read_vector_from_file(file_subseq_6_0 + '.' + std::to_string(i));
    subseq.subseq_7_0 = read_vector_from_file(file_subseq_7_0 + '.' + std::to_string(i));
    subseq.subseq_8_0 = read_vector_from_file(file_subseq_8_0 + '.' + std::to_string(i));
    subseq.subseq_8_10 = read_vector_from_file(file_subseq_8_10 + '.' + std::to_string(i));
    subseq.subseq_8_11 = read_vector_from_file(file_subseq_8_11 + '.' + std::to_string(i));
    subseq.subseq_8_12 = read_vector_from_file(file_subseq_8_12 + '.' + std::to_string(i));
    subseq.subseq_12_0 = read_vector_from_file(file_subseq_12_0 + '.' + std::to_string(i));
    subseq.quality_arr = read_file_as_string(file_quality + '.' + std::to_string(i));
    read_read_id_tokens_from_file(file_id + '.' + std::to_string(i), subseq.tokens);
    auto decoded_records = decode_streams(subseq, ref, eru_abs_flag, paired_end);
    for (auto record: decoded_records) {
      fout << record.title << "\n";
      fout << record.sequence << "\n";
      fout << "+" << "\n";
      fout << record.qualityScores << "\n";
    }
  }
}

}  // namespace spring
