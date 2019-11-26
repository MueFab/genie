#ifndef SPRING_GENERATE_READ_STREAMS_H_
#define SPRING_GENERATE_READ_STREAMS_H_

#include <map>
#include <string>

#include <genie/stream-saver.h>
#include <util/perf-stats.h>
#include "util.h"

namespace spring {

struct subseq_data {
    uint32_t block_num;
    std::map<uint8_t, std::map<uint8_t, std::string>> listDescriptorFiles;
    std::map<uint8_t, std::map<uint8_t, std::vector<int64_t>>> subseq_vector;
    dsg::AcessUnitStreams streamsAU;
};

void compress_subseqs(subseq_data *data, const std::vector<std::vector<gabac::EncodingConfiguration>> &configs);

void generate_read_streams(const std::string &temp_dir,
                           const compression_params &cp,
                           const std::vector<std::vector<gabac::EncodingConfiguration>> &configs);

void generate_read_streams_se(const std::string &temp_dir,
                              const compression_params &cp,
                              const std::vector<std::vector<gabac::EncodingConfiguration>> &configs);

void generate_read_streams_pe(const std::string &temp_dir,
                              const compression_params &cp,
                              const std::vector<std::vector<gabac::EncodingConfiguration>> &configs);

}  // namespace spring

#endif  // SPRING_GENERATE_READ_STREAMS_H_
