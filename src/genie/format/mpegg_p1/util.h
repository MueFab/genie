#ifndef GENIE_FORMAT_PART1_UTIL_H
#define GENIE_FORMAT_PART1_UTIL_H

#include <string>
#include <genie/util/bitreader.h>
#include <genie/util/bitwriter.h>

namespace genie {
namespace format {
namespace mpegg_p1 {

std::string&& readKey(util::BitReader &reader, std::string&& key);

std::string&& readNullTerminatedStr(util::BitReader &reader, std::string&& string);

void writeNullTerminatedStr(util::BitWriter &writer, const std::string &string);

}
}
}

#endif  // GENIE_FORMAT_PART1_UTIL_H
