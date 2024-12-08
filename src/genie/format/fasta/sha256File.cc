/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "genie/format/fasta/sha256File.h"

#include <algorithm>
#include <string>
#include <vector>

#include "picosha2/picosha2.h"

// -----------------------------------------------------------------------------

namespace genie::format::fasta {

// -----------------------------------------------------------------------------
std::vector<std::pair<std::string, std::string>> Sha256File::read(
    std::istream& file) {
  std::vector<std::pair<std::string, std::string>> ret;
  std::string buffer;
  while (std::getline(file, buffer)) {
    if (buffer.empty()) {
      continue;
    }
    const size_t delimiter = buffer.find_last_of(';');
    auto key = buffer.substr(0, delimiter);
    auto value = buffer.substr(delimiter + 1);
    ret.emplace_back(key, value);
  }
  return ret;
}

// -----------------------------------------------------------------------------
void Sha256File::Write(
    std::ostream& file,
    const std::vector<std::pair<std::string, std::string>>& val) {
  for (const auto& [fst, snd] : val) {
    file.write(fst.c_str(), static_cast<std::streamsize>(fst.length()));
    file.write(";", 1);
    file.write(snd.c_str(), static_cast<std::streamsize>(snd.length()));
    file.write("\n", 1);
  }
}

// -----------------------------------------------------------------------------
std::string Sha256File::Hex2Bytes(const std::string& hex) {
  const int len = static_cast<int>(hex.length());
  std::string new_string;
  for (int i = 0; i < len; i += 2) {
    std::string byte = hex.substr(i, 2);
    const char chr =
        static_cast<char>(static_cast<int>(strtol(byte.c_str(), nullptr, 16)));
    new_string.push_back(chr);
  }
  return new_string;
}

// -----------------------------------------------------------------------------
std::string Sha256File::hash(std::istream& file, const size_t pos_start,
                             size_t length) {
  file.seekg(static_cast<std::streamoff>(pos_start));
  constexpr size_t chunk_size = 1 * 1024 * 1024;
  picosha2::hash256_one_by_one hash_computer;
  hash_computer.init();
  while (length) {
    const size_t this_size = std::min(chunk_size, length);
    std::string buffer(this_size, 0);
    file.read(&buffer[0], static_cast<std::streamsize>(this_size));
    buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
    hash_computer.process(buffer.begin(), buffer.end());
    length -= buffer.size();
  }
  hash_computer.finish();
  return get_hash_hex_string(hash_computer);
}

// -----------------------------------------------------------------------------

}  // namespace genie::format::fasta

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
