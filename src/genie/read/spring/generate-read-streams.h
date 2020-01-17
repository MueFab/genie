/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SPRING_GENERATE_READ_STREAMS_H_
#define SPRING_GENERATE_READ_STREAMS_H_

#include <map>
#include <string>

#include <genie/core/access-unit-raw.h>
#include <genie/stream-saver.h>
#include <genie/util/perf-stats.h>
#include "util.h"

namespace genie {
namespace read {
namespace spring {

struct se_data {
    compression_params cp;
    std::vector<bool> flag_arr;
    std::vector<uint64_t> pos_arr;
    std::vector<uint16_t> read_length_arr;
    std::vector<char> noise_arr;
    std::vector<uint16_t> noisepos_arr;
    std::vector<uint64_t> pos_in_noise_arr;
    std::vector<uint16_t> noise_len_arr;
    std::vector<char> unaligned_arr;
    std::string seq;
    std::vector<char> RC_arr;
    std::vector<uint32_t> order_arr;
};

struct pe_block_data {
    std::vector<uint32_t> block_start;
    std::vector<uint32_t> block_end;  // block start and end positions wrt
    std::vector<uint32_t> block_num;
    std::vector<uint32_t> genomic_record_index;
    std::vector<uint32_t> read_index_genomic_record;
    std::vector<uint32_t> block_seq_start;
    std::vector<uint32_t> block_seq_end;
};

struct pe_statistics {
    std::vector<uint32_t> count_same_rec;
    std::vector<uint32_t> count_split_same_AU;
    std::vector<uint32_t> count_split_diff_AU;
};

struct subseq_data {
    uint32_t block_num;
    std::map<uint8_t, std::map<uint8_t, std::string>> listDescriptorFiles;
    std::map<uint8_t, std::map<uint8_t, std::vector<int64_t>>> subseq_vector;
    AcessUnitStreams streamsAU;
};

uint64_t getNumBlocks(const compression_params &data);

void compress_subseqs(subseq_data *data,
                      const std::vector<std::vector<entropy::gabac::EncodingConfiguration>> &configs);

core::AccessUnitRaw generate_read_streams(const std::string &temp_dir, const compression_params &cp,
                                          util::FastqStats *stats);

core::AccessUnitRaw generate_read_streams_se(const std::string &temp_dir, const compression_params &cp,
                                             util::FastqStats *stats);

core::AccessUnitRaw generate_read_streams_pe(const std::string &temp_dir, const compression_params &cp,
                                             util::FastqStats *stats);

void loadSE_Data(const compression_params &cp, const std::string &temp_dir, se_data *data);
void loadPE_Data(const compression_params &cp, const std::string &temp_dir, se_data *data);
core::AccessUnitRaw generate_subseqs(const se_data &data, uint64_t block_num);
core::AccessUnitRaw generate_streams_pe(const se_data &data, const pe_block_data &bdata, uint64_t cur_block_num,
                                        pe_statistics *pest);
void generateBlocksPE(const se_data &data, pe_block_data *bdata);

}  // namespace spring
}  // namespace read
}  // namespace genie

#endif  // SPRING_GENERATE_READ_STREAMS_H_
