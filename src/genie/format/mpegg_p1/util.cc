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

std::string readNullTerminatedStr(util::BitReader &reader) {
    std::string string;
    char c = 0;
    do {
        c = reader.read<char>();
        //        string.push_back(c);
        string += c;
    } while (c);

    return string;
}

// ---------------------------------------------------------------------------------------------------------------------

void writeNullTerminatedStr(util::BitWriter &writer, const std::string &string) {
    writer.write(string);
    writer.write('\0', 8);
}

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
