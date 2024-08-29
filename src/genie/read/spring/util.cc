/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/read/spring/util.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::spring {

// ---------------------------------------------------------------------------------------------------------------------

std::vector<int64_t> read_vector_from_file(const std::string &file_name) {
    std::ifstream f_in(file_name, std::ios::binary);
    std::vector<int64_t> vec;
    if (!f_in.is_open()) return vec;
    int64_t val;
    f_in.read(reinterpret_cast<char *>(&val), sizeof(int64_t));
    while (!f_in.eof()) {
        vec.push_back(val);
        f_in.read(reinterpret_cast<char *>(&val), sizeof(int64_t));
    }
    return vec;
}

// ---------------------------------------------------------------------------------------------------------------------

void reverse_complement(char *s, char *s1, const int readlen) {
    for (int j = 0; j < readlen; j++) s1[j] = chartorevchar[(uint8_t)s[readlen - j - 1]];
    s1[readlen] = '\0';
    return;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string reverse_complement(const std::string &s, const int readlen) {
    std::string s1;
    s1.resize(readlen);
    for (int j = 0; j < readlen; j++) s1[j] = chartorevchar[(uint8_t)s[readlen - j - 1]];
    return s1;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string random_string(size_t length) {
    auto randchar = []() -> char {
        const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

// ---------------------------------------------------------------------------------------------------------------------

void write_dna_in_bits(const std::string &read, std::ofstream &fout) {
    uint8_t dna2int[128];
    dna2int[(uint8_t)'A'] = 0;
    dna2int[(uint8_t)'C'] = 2;  // chosen to align with the bitset representation
    dna2int[(uint8_t)'G'] = 1;
    dna2int[(uint8_t)'T'] = 3;
    uint8_t bitarray[128];
    uint8_t pos_in_bitarray = 0;
    auto readlen = static_cast<uint16_t>(read.size());
    fout.write(reinterpret_cast<char *>(&readlen), sizeof(uint16_t));
    for (int i = 0; i < readlen / 4; i++) {
        bitarray[pos_in_bitarray] = 0;
        for (int j = 0; j < 4; j++) bitarray[pos_in_bitarray] |= (dna2int[(uint8_t)read[4 * i + j]] << (2 * j));
        pos_in_bitarray++;
    }
    if (readlen % 4 != 0) {
        int i = readlen / 4;
        bitarray[pos_in_bitarray] = 0;
        for (int j = 0; j < readlen % 4; j++)
            bitarray[pos_in_bitarray] |= (dna2int[(uint8_t)read[4 * i + j]] << (2 * j));
        pos_in_bitarray++;
    }
    fout.write(reinterpret_cast<char *>(&bitarray[0]), pos_in_bitarray);
    return;
}

// ---------------------------------------------------------------------------------------------------------------------

void read_dna_from_bits(std::string &read, std::ifstream &fin) {
    uint16_t readlen;
    uint8_t bitarray[128];
    const char int2dna[4] = {'A', 'G', 'C', 'T'};
    fin.read(reinterpret_cast<char *>(&readlen), sizeof(uint16_t));
    read.resize(readlen);
    uint16_t num_bytes_to_read = ((uint32_t)readlen + 4 - 1) / 4;
    fin.read(reinterpret_cast<char *>(&bitarray[0]), num_bytes_to_read);
    uint8_t pos_in_bitarray = 0;
    for (int i = 0; i < readlen / 4; i++) {
        for (int j = 0; j < 4; j++) {
            read[4 * i + j] = int2dna[bitarray[pos_in_bitarray] & 3];
            bitarray[pos_in_bitarray] >>= 2;
        }
        pos_in_bitarray++;
    }
    if (readlen % 4 != 0) {
        int i = readlen / 4;
        for (int j = 0; j < readlen % 4; j++) {
            read[4 * i + j] = int2dna[bitarray[pos_in_bitarray] & 3];
            bitarray[pos_in_bitarray] >>= 2;
        }
        pos_in_bitarray++;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void write_dnaN_in_bits(const std::string &read, std::ofstream &fout) {
    uint8_t dna2int[128];
    dna2int[(uint8_t)'A'] = 0;
    dna2int[(uint8_t)'C'] = 2;  // chosen to align with the bitset representation
    dna2int[(uint8_t)'G'] = 1;
    dna2int[(uint8_t)'T'] = 3;
    dna2int[(uint8_t)'N'] = 4;
    uint8_t bitarray[256];
    uint8_t pos_in_bitarray = 0;
    auto readlen = static_cast<uint16_t>(read.size());
    fout.write(reinterpret_cast<char *>(&readlen), sizeof(uint16_t));
    for (int i = 0; i < readlen / 2; i++) {
        bitarray[pos_in_bitarray] = 0;
        for (int j = 0; j < 2; j++) bitarray[pos_in_bitarray] |= (dna2int[(uint8_t)read[2 * i + j]] << (4 * j));
        pos_in_bitarray++;
    }
    if (readlen % 2 != 0) {
        int i = readlen / 2;
        bitarray[pos_in_bitarray] = 0;
        for (int j = 0; j < readlen % 2; j++)
            bitarray[pos_in_bitarray] |= (dna2int[(uint8_t)read[2 * i + j]] << (4 * j));
        pos_in_bitarray++;
    }
    fout.write(reinterpret_cast<char *>(&bitarray[0]), pos_in_bitarray);
    return;
}

// ---------------------------------------------------------------------------------------------------------------------

void read_dnaN_from_bits(std::string &read, std::ifstream &fin) {
    uint16_t readlen;
    uint8_t bitarray[256];
    const char int2dna[5] = {'A', 'G', 'C', 'T', 'N'};
    fin.read(reinterpret_cast<char *>(&readlen), sizeof(uint16_t));
    read.resize(readlen);
    uint16_t num_bytes_to_read = ((uint32_t)readlen + 2 - 1) / 2;
    fin.read(reinterpret_cast<char *>(&bitarray[0]), num_bytes_to_read);
    uint8_t pos_in_bitarray = 0;
    for (int i = 0; i < readlen / 2; i++) {
        for (int j = 0; j < 2; j++) {
            read[2 * i + j] = int2dna[bitarray[pos_in_bitarray] & 15];
            bitarray[pos_in_bitarray] >>= 4;
        }
        pos_in_bitarray++;
    }
    if (readlen % 2 != 0) {
        int i = readlen / 2;
        for (int j = 0; j < readlen % 2; j++) {
            read[2 * i + j] = int2dna[bitarray[pos_in_bitarray] & 15];
            bitarray[pos_in_bitarray] >>= 4;
        }
        pos_in_bitarray++;
    }
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::spring

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
