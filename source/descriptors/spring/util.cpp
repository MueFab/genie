#include "descriptors/spring/util.h"
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>

namespace spring {

uint32_t read_fastq_block(std::ifstream &fin, std::string *id_array,
                          std::string *read_array, std::string *quality_array,
                          const uint32_t &num_reads) {
  uint32_t num_done = 0;
  std::string comment;
  for (; num_done < num_reads; num_done++) {
    if (!std::getline(fin, id_array[num_done])) break;
    if (!std::getline(fin, read_array[num_done]))
      throw std::runtime_error(
          "Invalid FASTQ file. Number of lines not multiple of 4");
    if (!std::getline(fin, comment))
      throw std::runtime_error(
          "Invalid FASTQ file. Number of lines not multiple of 4");
    if (!std::getline(fin, quality_array[num_done]))
      throw std::runtime_error(
          "Invalid FASTQ file. Number of lines not multiple of 4");
  }
  return num_done;
}

void write_fastq_block(std::ofstream &fout, std::string *id_array,
                       std::string *read_array, std::string *quality_array,
                       const uint32_t &num_reads, const bool preserve_quality) {
  for (uint32_t i = 0; i < num_reads; i++) {
    fout << id_array[i] << "\n";
    fout << read_array[i] << "\n";
    if (preserve_quality) {
      fout << "+\n";
      fout << quality_array[i] << "\n";
    }
  }
}

void write_vector_to_file(const std::vector<int64_t> &subseq, const std::string &file_name) {
  std::ofstream f_out(file_name,std::ios::binary);
  for (int64_t val : subseq)
    f_out.write((char*)&val, sizeof(int64_t));
  f_out.close();
}

std::vector<int64_t> read_vector_from_file(const std::string &file_name) {
  std::ifstream f_in(file_name, std::ios::binary);
  std::vector<int64_t> vec;
  if (!f_in.is_open()) return vec;
  int64_t val;
  f_in.read((char*)&val, sizeof(int64_t));
  while(!f_in.eof()) {
    vec.push_back(val);
    f_in.read((char*)&val, sizeof(int64_t));
  }
  return vec;
}

void quantize_quality(std::string *quality_array, const uint32_t &num_lines,
                      char *quantization_table) {
  for (uint32_t i = 0; i < num_lines; i++)
    for (uint32_t j = 0; j < quality_array[i].size(); j++)
      quality_array[i][j] = quantization_table[(uint8_t)quality_array[i][j]];
  return;
}

void generate_illumina_binning_table(char *illumina_binning_table) {
  for (uint8_t i = 0; i <= 33 + 1; i++) illumina_binning_table[i] = 33 + 0;
  for (uint8_t i = 33 + 2; i <= 33 + 9; i++) illumina_binning_table[i] = 33 + 6;
  for (uint8_t i = 33 + 10; i <= 33 + 19; i++)
    illumina_binning_table[i] = 33 + 15;
  for (uint8_t i = 33 + 20; i <= 33 + 24; i++)
    illumina_binning_table[i] = 33 + 22;
  for (uint8_t i = 33 + 25; i <= 33 + 29; i++)
    illumina_binning_table[i] = 33 + 27;
  for (uint8_t i = 33 + 30; i <= 33 + 34; i++)
    illumina_binning_table[i] = 33 + 33;
  for (uint8_t i = 33 + 35; i <= 33 + 39; i++)
    illumina_binning_table[i] = 33 + 37;
  for (uint8_t i = 33 + 40; i <= 127; i++) illumina_binning_table[i] = 33 + 40;
}

void generate_binary_binning_table(char *binary_binning_table, const unsigned int thr, const unsigned int high, const unsigned int low) {
  for (uint8_t i = 0; i < 33 + thr; i++) binary_binning_table[i] = 33 + low;
  for (uint8_t i = 33 + thr; i <= 127; i++) binary_binning_table[i] = 33 + high;
}

// ID patterns
// code 0: no pattern found
// code 1: */1 and */2 where * are same in both
// code 2: * and * where * are same in both
// code 3: * 1:# and * 2:# where * and # are common to both and * contains no
// space (used in new versions)
uint8_t find_id_pattern(const std::string &id_1, const std::string &id_2) {
  if (id_1.length() != id_2.length()) return 0;
  if (id_1 == id_2) return 2;
  size_t len = id_1.length();
  size_t i;
  if (id_1[len - 1] == '1' && id_2[len - 1] == '2') {
    // compare rest
    for (i = 0; i < len - 1; i++)
      if (id_1[i] != id_2[i]) break;
    if (i == len - 1) return 1;
  }
  for (i = 0; i < len; i++) {
    if (id_1[i] != id_2[i]) break;
    if (id_1[i] == ' ') {
      if (i < len - 1 && id_1[i + 1] == '1' && id_2[i + 1] == '2')
        i++;
      else
        break;
    }
  }
  if (i == len) return 3;
  return 0;
}

bool check_id_pattern(const std::string &id_1, const std::string &id_2,
                      const uint8_t paired_id_code) {
  if (id_1.length() != id_2.length()) return false;
  size_t len = id_1.length();
  size_t i;
  switch (paired_id_code) {
    case 1:
      if (id_1[len - 1] == '1' && id_2[len - 1] == '2') {
        // compare rest
        for (i = 0; i < len - 1; i++)
          if (id_1[i] != id_2[i]) break;
        if (i == len - 1) return true;
      }
      break;
    case 2:
      if (id_1 == id_2) return true;
      break;
    case 3:
      for (i = 0; i < len; i++) {
        if (id_1[i] != id_2[i]) break;
        if (id_1[i] == ' ') {
          if (i < len - 1 && id_1[i + 1] == '1' && id_2[i + 1] == '2')
            i++;
          else
            break;
        }
      }
      if (i == len) return true;
      break;
    default:
      throw std::runtime_error("Invalid paired id code.");
  }
  return false;
}

void modify_id(std::string &id, const uint8_t paired_id_code) {
  if (paired_id_code == 2)
    return;
  else if (paired_id_code == 1) {
    id.back() = '2';
    return;
  } else if (paired_id_code == 3) {
    int i = 0;
    while (id[i] != ' ') i++;
    id[i + 1] = '2';
    return;
  }
}

void write_dna_in_bits(const std::string &read, std::ofstream &fout) {
  uint8_t dna2int[128];
  dna2int[(uint8_t)'A'] = 0;
  dna2int[(uint8_t)'C'] = 1;
  dna2int[(uint8_t)'G'] = 2;
  dna2int[(uint8_t)'T'] = 3;
  uint16_t readlen = read.size();
  fout.write((char *)&readlen, sizeof(uint16_t));
  uint8_t byte;
  for (int i = 0; i < readlen / 4; i++) {
    byte = 0;
    for (int j = 0; j < 4; j++)
      byte = byte * 4 + dna2int[(uint8_t)read[4 * i + j]];
    fout.write((char *)&byte, sizeof(uint8_t));
  }
  if (readlen % 4 != 0) {
    int i = readlen / 4;
    byte = 0;
    for (int j = 0; j < readlen % 4; j++)
      byte = byte * 4 + dna2int[(uint8_t)read[4 * i + j]];
    fout.write((char *)&byte, sizeof(uint8_t));
  }
  return;
}

void read_dna_from_bits(std::string &read, std::ifstream &fin) {
  uint16_t readlen;
  fin.read((char *)&readlen, sizeof(uint16_t));
  read.resize(readlen);
  uint8_t byte;
  for (int i = 0; i < readlen / 4; i++) {
    fin.read((char *)&byte, sizeof(uint8_t));
    for (int j = 0; j < 4; j++) {
      if (byte % 4 == 0)
        read[4 * i + 3 - j] = 'A';
      else if (byte % 4 == 1)
        read[4 * i + 3 - j] = 'C';
      else if (byte % 4 == 2)
        read[4 * i + 3 - j] = 'G';
      else if (byte % 4 == 3)
        read[4 * i + 3 - j] = 'T';
      byte /= 4;
    }
  }
  if (readlen / 4 != 0) {
    int i = readlen / 4;
    fin.read((char *)&byte, sizeof(uint8_t));
    for (int j = 0; j < readlen % 4; j++) {
      if (byte % 4 == 0)
        read[4 * i + readlen % 4 - 1 - j] = 'A';
      else if (byte % 4 == 1)
        read[4 * i + readlen % 4 - 1 - j] = 'C';
      else if (byte % 4 == 2)
        read[4 * i + readlen % 4 - 1 - j] = 'G';
      else if (byte % 4 == 3)
        read[4 * i + readlen % 4 - 1 - j] = 'T';
      byte /= 4;
    }
  }
}
void reverse_complement(char *s, char *s1, const int readlen) {
  for (int j = 0; j < readlen; j++)
    s1[j] = chartorevchar[(uint8_t)s[readlen - j - 1]];
  s1[readlen] = '\0';
  return;
}

std::string reverse_complement(const std::string &s, const int readlen) {
  std::string s1;
  s1.resize(readlen);
  for (int j = 0; j < readlen; j++)
    s1[j] = chartorevchar[(uint8_t)s[readlen - j - 1]];
  return s1;
}

std::string read_file_as_string(const std::string &file_seq) {
  std::string seq;
  std::ifstream f_seq(file_seq);
  f_seq.seekg(0,f_seq.end);
  uint64_t seq_len = f_seq.tellg();
  seq.resize(seq_len);
  f_seq.seekg(0);
  f_seq.read(&seq[0], seq_len);
  f_seq.close();
  return seq;
}

void generate_order_array(const std::string &file_order, uint32_t *order_array,
                       const uint32_t &numreads) {
  std::ifstream fin_order(file_order, std::ios::binary);
  uint32_t order;
  for (uint32_t i = 0; i < numreads; i++) {
    fin_order.read((char *)&order, sizeof(uint32_t));
    order_array[order] = i;
  }
  fin_order.close();
}

}  // namespace spring
