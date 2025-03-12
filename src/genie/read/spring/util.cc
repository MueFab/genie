/**
 * Copyright 2018-2024 The Genie Authors.
 * @file util.cc
 * @brief Implementation of utility functions for the Spring module.
 *
 * This file contains the implementation of various utility functions used in
 * the Spring module for DNA sequence manipulation, including reading vectors
 * from files, generating reverse complements, creating random DNA strings, and
 * encoding/decoding DNA sequences in binary format.
 *
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/read/spring/util.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "genie/util/runtime_exception.h"

// -----------------------------------------------------------------------------

namespace genie::read::spring {

// -----------------------------------------------------------------------------

std::vector<int64_t> ReadVectorFromFile(const std::string& file_name) {
  std::ifstream f_in(file_name, std::ios::binary);
  UTILS_DIE_IF(!f_in, "Cannot open file to read: " + file_name);
  std::vector<int64_t> vec;
  if (!f_in.is_open()) return vec;
  int64_t val;
  f_in.read(reinterpret_cast<char*>(&val), sizeof(int64_t));
  while (!f_in.eof()) {
    vec.push_back(val);
    f_in.read(reinterpret_cast<char*>(&val), sizeof(int64_t));
  }
  return vec;
}

// -----------------------------------------------------------------------------

void ReverseComplement(const char* s, char* s1, const int read_len) {
  for (int j = 0; j < read_len; j++)
    s1[j] = kCharToRevChar[static_cast<uint8_t>(s[read_len - j - 1])];
  s1[read_len] = '\0';
}

// -----------------------------------------------------------------------------

std::string ReverseComplement(const std::string& s, const int read_len) {
  std::string s1;
  s1.resize(read_len);
  for (int j = 0; j < read_len; j++)
    s1[j] = kCharToRevChar[static_cast<uint8_t>(s[read_len - j - 1])];
  return s1;
}

// -----------------------------------------------------------------------------

std::string RandomString(const size_t length) {
  static constexpr char charset[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"   // NOLINT
      "abcdefghijklmnopqrstuvwxyz";  // NOLINT
  static constexpr size_t max_index = sizeof(charset) - 1;

  // Use a random device and a Mersenne Twister engine for better randomness
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_int_distribution<> distribution(0, max_index - 1);

  auto random_char = [&]() -> char { return charset[distribution(generator)]; };

  std::string str(length, 0);
  std::generate_n(str.begin(), length, random_char);
  return str;
}

// -----------------------------------------------------------------------------

void WriteDnaInBits(const std::string& read, std::ofstream& f_out) {
  uint8_t dna2_int[128];
  dna2_int[static_cast<uint8_t>('A')] = 0;
  dna2_int[static_cast<uint8_t>('C')] =
      2;  // chosen to align with the bitset representation
  dna2_int[static_cast<uint8_t>('G')] = 1;
  dna2_int[static_cast<uint8_t>('T')] = 3;
  uint8_t bitarray[128];
  uint8_t pos_in_bitarray = 0;
  auto read_len = static_cast<uint16_t>(read.size());
  f_out.write(reinterpret_cast<char*>(&read_len), sizeof(uint16_t));
  for (int i = 0; i < read_len / 4; i++) {
    bitarray[pos_in_bitarray] = 0;
    for (int j = 0; j < 4; j++)
      bitarray[pos_in_bitarray] |=
          dna2_int[static_cast<uint8_t>(read[4 * i + j])] << 2 * j;
    pos_in_bitarray++;
  }
  if (read_len % 4 != 0) {
    const int i = read_len / 4;
    bitarray[pos_in_bitarray] = 0;
    for (int j = 0; j < read_len % 4; j++)
      bitarray[pos_in_bitarray] |=
          dna2_int[static_cast<uint8_t>(read[4 * i + j])] << 2 * j;
    pos_in_bitarray++;
  }
  f_out.write(reinterpret_cast<char*>(&bitarray[0]), pos_in_bitarray);
}

// -----------------------------------------------------------------------------

void ReadDnaFromBits(std::string& read, std::ifstream& fin) {
  uint16_t read_len;
  uint8_t bitarray[128];
  constexpr char int2dna[4] = {'A', 'G', 'C', 'T'};
  fin.read(reinterpret_cast<char*>(&read_len), sizeof(uint16_t));
  read.resize(read_len);
  const uint16_t num_bytes_to_read =
      (static_cast<uint32_t>(read_len) + 4 - 1) / 4;
  fin.read(reinterpret_cast<char*>(&bitarray[0]), num_bytes_to_read);
  uint8_t pos_in_bitarray = 0;
  for (int i = 0; i < read_len / 4; i++) {
    for (int j = 0; j < 4; j++) {
      read[4 * i + j] = int2dna[bitarray[pos_in_bitarray] & 3];
      bitarray[pos_in_bitarray] >>= 2;
    }
    pos_in_bitarray++;
  }
  if (read_len % 4 != 0) {
    const int i = read_len / 4;
    for (int j = 0; j < read_len % 4; j++) {
      read[4 * i + j] = int2dna[bitarray[pos_in_bitarray] & 3];
      bitarray[pos_in_bitarray] >>= 2;
    }
  }
}

// -----------------------------------------------------------------------------

void WriteDnaNInBits(const std::string& read, std::ofstream& f_out) {
  uint8_t dna2_int[128];
  dna2_int[static_cast<uint8_t>('A')] = 0;
  dna2_int[static_cast<uint8_t>('C')] =
      2;  // chosen to align with the bitset representation
  dna2_int[static_cast<uint8_t>('G')] = 1;
  dna2_int[static_cast<uint8_t>('T')] = 3;
  dna2_int[static_cast<uint8_t>('N')] = 4;
  uint8_t bitarray[256];
  uint8_t pos_in_bitarray = 0;
  auto read_len = static_cast<uint16_t>(read.size());
  f_out.write(reinterpret_cast<char*>(&read_len), sizeof(uint16_t));
  for (int i = 0; i < read_len / 2; i++) {
    bitarray[pos_in_bitarray] = 0;
    for (int j = 0; j < 2; j++)
      bitarray[pos_in_bitarray] |=
          dna2_int[static_cast<uint8_t>(read[2 * i + j])] << 4 * j;
    pos_in_bitarray++;
  }
  if (read_len % 2 != 0) {
    const int i = read_len / 2;
    bitarray[pos_in_bitarray] = 0;
    for (int j = 0; j < read_len % 2; j++)
      bitarray[pos_in_bitarray] |=
          dna2_int[static_cast<uint8_t>(read[2 * i + j])] << 4 * j;
    pos_in_bitarray++;
  }
  f_out.write(reinterpret_cast<char*>(&bitarray[0]), pos_in_bitarray);
}

// -----------------------------------------------------------------------------

void ReadDnaNFromBits(std::string& read, std::ifstream& fin) {
  uint16_t read_len = 0;
  uint8_t bitarray[256]{};
  constexpr char int2dna[5] = {'A', 'G', 'C', 'T', 'N'};
  fin.read(reinterpret_cast<char*>(&read_len), sizeof(uint16_t));
  read.resize(read_len);
  const uint16_t num_bytes_to_read =
      (static_cast<uint32_t>(read_len) + 2 - 1) / 2;
  fin.read(reinterpret_cast<char*>(&bitarray[0]), num_bytes_to_read);
  uint8_t pos_in_bitarray = 0;
  for (int i = 0; i < read_len / 2; i++) {
    for (int j = 0; j < 2; j++) {
      read[2 * i + j] = int2dna[bitarray[pos_in_bitarray] & 15];
      bitarray[pos_in_bitarray] >>= 4;
    }
    pos_in_bitarray++;
  }
  if (read_len % 2 != 0) {
    const int i = read_len / 2;
    for (int j = 0; j < read_len % 2; j++) {
      read[2 * i + j] = int2dna[bitarray[pos_in_bitarray] & 15];
      bitarray[pos_in_bitarray] >>= 4;
    }
  }
}

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
