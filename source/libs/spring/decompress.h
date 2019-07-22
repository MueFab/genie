#ifndef SPRING_DECOMPRESS_H_
#define SPRING_DECOMPRESS_H_

#include <vector>
#include <string>
#include <map>
#include <genie/stream-saver.h>
#include "util.h"
#include <utils/fastq-record.h>

namespace spring {

    struct decoded_desc_t {
        std::map<uint8_t, std::map<uint8_t, std::vector<int64_t>>> subseq_vector;
        std::string quality_arr;
        std::vector<int64_t> tokens[128][8];
    };

    std::pair<std::vector<utils::FastqRecord>, std::vector<bool>>
    decode_streams(decoded_desc_t &desc, const std::vector<std::array<uint8_t, 2>> &subseq_indices, bool paired_end);

    bool decompress(const std::string &temp_dir, dsg::StreamSaver *ld);

}  // namespace spring

#endif  // SPRING_DECOMPRESS_H_
