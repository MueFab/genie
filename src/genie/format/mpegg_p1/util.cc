/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#include "genie/format/mpegg_p1/util.h"
#include <string>
#include <utility>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

// ---------------------------------------------------------------------------------------------------------------------

std::string readKey(util::BitReader &reader) {
    std::string key = "XXXX";
    for (uint8_t i = 0; i < (uint8_t)key.size(); i++) {
        auto c = reader.read<char>();
        key[i] = c;
    }
    return key;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string readFixedLengthChars(util::BitReader &reader, uint8_t n) {
    std::string str;
    for (uint8_t i = 0; i < n; i++) {
        auto c = reader.read<char>();
        str += c;
    }
    return str;
}

// ---------------------------------------------------------------------------------------------------------------------

std::string readNullTerminatedStr(util::BitReader &reader) {
    std::string string;
    char c = reader.read<char>();
    while (c != '\0'){
        string += c;
        c = reader.read<char>();
    }

    return string;
}

// ---------------------------------------------------------------------------------------------------------------------

void writeNullTerminatedStr(util::BitWriter &writer, const std::string &string) {
    writer.write(string);
    writer.write('\0', 8);
}

void skipRead(util::BitReader &reader, uint64_t length){
    for (uint64_t i = 0; i<length-8-4;i++) reader.read<uint8_t>();
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
