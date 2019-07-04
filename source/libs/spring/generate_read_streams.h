#ifndef SPRING_GENERATE_READ_STREAMS_H_
#define SPRING_GENERATE_READ_STREAMS_H_

#include <string>
#include <map>
#include "spring/util.h"

#include "genie/StreamSaver.h"

namespace spring {

    std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>>  generate_read_streams(const std::string &temp_dir, const compression_params &cp, dsg::StreamSaver& st);

    std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>>  generate_read_streams_se(const std::string &temp_dir, const compression_params &cp, dsg::StreamSaver& st);

    std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>>  generate_read_streams_pe(const std::string &temp_dir, const compression_params &cp, dsg::StreamSaver& st);

}  // namespace spring

#endif  // SPRING_GENERATE_READ_STREAMS_H_
