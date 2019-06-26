#ifndef SPRING_GENERATE_READ_STREAMS_H_
#define SPRING_GENERATE_READ_STREAMS_H_

#include <string>
#include <map>
#include "spring/util.h"

namespace spring {

std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> generate_read_streams(const std::string &temp_dir, const compression_params &cp);

std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> generate_read_streams_se(const std::string &temp_dir, const compression_params &cp);

std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>> generate_read_streams_pe(const std::string &temp_dir, const compression_params &cp);

}  // namespace spring

#endif  // SPRING_GENERATE_READ_STREAMS_H_
