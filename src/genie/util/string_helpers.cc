/**
 * Copyright 2018-2024 The Genie Authors.
 * @file string_helpers.cc
 * @brief Implementation of string utility functions for manipulating and
 * processing strings.
 *
 * This file contains the implementation of various utility functions for
 * performing common string operations such as trimming, splitting, and
 * converting between binary and hexadecimal formats. These helper functions are
 * designed to simplify string manipulation and provide a consistent interface
 * for string processing tasks.
 *
 * @details The file defines functions for removing specified characters from
 * strings
 * (`Rtrim()`, `Ltrim()`, and `Trim()`), splitting strings into tokens
 * (`Tokenize()`), and converting between binary and hexadecimal string
 * representations (`ToHex()` and `FromHex()`).
 *
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 */

#include "genie/util/string_helpers.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------

namespace genie::util {

// -----------------------------------------------------------------------------

std::string& Rtrim(std::string& s, const char* t) {
  s.erase(s.find_last_not_of(t) + 1);
  return s;
}

// -----------------------------------------------------------------------------

std::string& Ltrim(std::string& s, const char* t) {
  s.erase(0, s.find_first_not_of(t));
  return s;
}

// -----------------------------------------------------------------------------

std::string& Trim(std::string& s, const char* t) {
  return Ltrim(Rtrim(s, t), t);
}

// -----------------------------------------------------------------------------

std::vector<std::string> Tokenize(const std::string& str, const char delim) {
  size_t start;
  size_t end = 0;
  std::vector<std::string> out;
  while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
    end = str.find(delim, start);
    out.push_back(str.substr(start, end - start));
  }
  return out;
}

// -----------------------------------------------------------------------------

std::string ToHex(const std::string& bin) {
  static constexpr char lut[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                 '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  std::string ret;
  for (const auto& c : bin) {
    ret += lut[(c & 0xf0) >> 4];
    ret += lut[c & 0xf];
  }
  return ret;
}

// -----------------------------------------------------------------------------

std::string FromHex(const std::string& hex) {
  static constexpr std::array<char, 256> lut = []() -> std::array<char, 256> {
    std::array<char, 256> ret{};
    ret['0'] = 0;
    ret['1'] = 1;
    ret['2'] = 2;
    ret['3'] = 3;
    ret['4'] = 4;
    ret['5'] = 5;
    ret['6'] = 6;
    ret['7'] = 7;
    ret['8'] = 8;
    ret['9'] = 9;
    ret['a'] = 10;
    ret['b'] = 11;
    ret['c'] = 12;
    ret['d'] = 13;
    ret['e'] = 14;
    ret['f'] = 15;
    return ret;
  }();
  std::string ret;
  char buffer = 0;
  bool step = false;
  for (const auto& c : hex) {
    if (!step) {
      buffer = static_cast<char>((static_cast<uint8_t>(lut[c]) & 0xf) << 4);
      step = true;
    } else {
      buffer =
          static_cast<char>(static_cast<uint8_t>(buffer) |
                            (static_cast<uint8_t>(lut[c]) & 0xf));  // NOLINT
      ret += buffer;
      step = false;
    }
  }
  if (step) {
    ret += buffer;
  }
  return ret;
}

// -----------------------------------------------------------------------------

}  // namespace genie::util

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
