/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "util.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace read {
namespace spring {

// ---------------------------------------------------------------------------------------------------------------------

std::vector<int64_t> read_vector_from_file(const std::string &file_name) {
    std::ifstream f_in(file_name, std::ios::binary);
    std::vector<int64_t> vec;
    if (!f_in.is_open()) return vec;
    int64_t val;
    f_in.read((char *)&val, sizeof(int64_t));
    while (!f_in.eof()) {
        vec.push_back(val);
        f_in.read((char *)&val, sizeof(int64_t));
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

}  // namespace spring
}  // namespace read
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------