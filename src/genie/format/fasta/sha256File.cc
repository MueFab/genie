/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/fasta/sha256File.h"
#include <algorithm>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::format::fasta {

// ---------------------------------------------------------------------------------------------------------------------

std::vector<std::pair<std::string, std::string>> Sha256File::read(std::istream& file) {
    std::vector<std::pair<std::string, std::string>> ret;
    std::string buffer;
    while (std::getline(file, buffer)) {
        if (buffer.empty()) {
            continue;
        }
        size_t delimiter = buffer.find_last_of(';');
        auto key = buffer.substr(0, delimiter);
        auto value = buffer.substr(delimiter + 1);
        ret.emplace_back(key, value);
    }
    return ret;
}

// ---------------------------------------------------------------------------------------------------------------------

void Sha256File::write(std::ostream& file, const std::vector<std::pair<std::string, std::string>>& val) {
    for (const auto& p : val) {
        file.write(p.first.c_str(), p.first.length());
        file.write(";", 1);
        file.write(p.second.c_str(), p.second.length());
        file.write("\n", 1);
    }
}

// ---------------------------------------------------------------------------------------------------------------------

std::string Sha256File::hex2bytes(const std::string& hex) {
    int len = static_cast<int>(hex.length());
    std::string newString;
    for (int i = 0; i < len; i += 2) {
        std::string byte = hex.substr(i, 2);
        char chr = static_cast<char>(static_cast<int>(strtol(byte.c_str(), nullptr, 16)));
        newString.push_back(chr);
    }
    return newString;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string Sha256File::hash(std::istream& file, size_t posStart, size_t length) {
    file.seekg(posStart);
    size_t CHUNK_SIZE = 1 * 1024 * 1024;
    picosha2::hash256_one_by_one hasher;
    hasher.init();
    while (length) {
        size_t this_size = std::min(CHUNK_SIZE, length);
        std::string buffer(this_size, 0);
        file.read(&buffer[0], this_size);
        buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
        hasher.process(buffer.begin(), buffer.end());
        length -= buffer.size();
    }
    hasher.finish();
    std::string bytes_str(picosha2::k_digest_size, 0);
    return picosha2::get_hash_hex_string(hasher);
    // return bytes_str;
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::format::fasta

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
