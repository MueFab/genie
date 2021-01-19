#include "util.h"
#include <string>

namespace genie {
namespace format {
namespace mpegg_p1 {

std::string&& readKey(util::BitReader &reader){
    std::string key = "XXXX";
    for (uint8_t i = 0; i < key.size(); i++){
        auto c = reader.read<uint8_t>();
        key[i] = c;
    }

    return std::move(key);
}

std::string&& readNullTerminatedStr(util::BitReader &reader) {
    std::string string;
    char c = 0;
    do {
        c = reader.read<uint8_t>();
//        string.push_back(c);
        string += c;
    } while (c);

    return std::move(string);
}

void writeNullTerminatedStr(util::BitWriter &writer, const std::string &string){
    writer.write(string);
    writer.write('\0', 8);
}

}
}
}
