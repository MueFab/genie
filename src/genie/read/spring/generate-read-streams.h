#ifndef SPRING_GENERATE_READ_STREAMS_H_
#define SPRING_GENERATE_READ_STREAMS_H_

#include <map>
#include <string>

#include <genie/core/read-encoder.h>
#include "util.h"

namespace genie {
namespace read {
namespace spring {

struct subseq_data {
    uint32_t block_num;
    std::map<uint8_t, std::map<uint8_t, std::string>> listDescriptorFiles;
    std::map<uint8_t, std::map<uint8_t, std::vector<int64_t>>> subseq_vector;
};

void generate_read_streams(const std::string &temp_dir, const compression_params &cp,
                           core::ReadEncoder::EntropySelector *entropycoder, std::vector<core::parameter::ParameterSet>&);

void generate_read_streams_se(const std::string &temp_dir, const compression_params &cp,
                              core::ReadEncoder::EntropySelector *entropycoder, std::vector<core::parameter::ParameterSet>&);

void generate_read_streams_pe(const std::string &temp_dir, const compression_params &cp,
                              core::ReadEncoder::EntropySelector *entropycoder, std::vector<core::parameter::ParameterSet>&);

}  // namespace spring
}  // namespace read
}  // namespace genie

#endif  // SPRING_GENERATE_READ_STREAMS_H_
