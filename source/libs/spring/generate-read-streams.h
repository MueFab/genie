#ifndef SPRING_GENERATE_READ_STREAMS_H_
#define SPRING_GENERATE_READ_STREAMS_H_

#include <string>
#include <map>

#include "util.h"

#include <genie/stream-saver.h>

namespace spring {

    struct subseq_data {
        uint32_t block_num;
        std::map<uint8_t, std::map<uint8_t, std::string>> listDescriptorFiles;
        std::map<uint8_t, std::map<uint8_t, std::vector<int64_t>>> subseq_vector;
        dsg::AcessUnitStreams streamsAU;
    };

    void compress_subseqs(subseq_data* data, dsg::StreamSaver &st);

    void pack_subseqs(subseq_data* data, dsg::StreamSaver &st, std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>>* descriptorFilesPerAU);

    std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>>  generate_read_streams(const std::string &temp_dir, const compression_params &cp, bool analyze, dsg::StreamSaver& st);

    std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>>  generate_read_streams_se(const std::string &temp_dir, const compression_params &cp, bool analyze, dsg::StreamSaver& st);

    std::vector<std::map<uint8_t, std::map<uint8_t, std::string>>>  generate_read_streams_pe(const std::string &temp_dir, const compression_params &cp, bool analyze, dsg::StreamSaver& st);

}  // namespace spring

#endif  // SPRING_GENERATE_READ_STREAMS_H_
