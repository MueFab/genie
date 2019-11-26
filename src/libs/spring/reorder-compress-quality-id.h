#ifndef SPRING_REORDER_COMPRESS_QUALITY_ID_H_
#define SPRING_REORDER_COMPRESS_QUALITY_ID_H_

#include <string>

#include "util.h"

namespace spring {

void reorder_compress_quality_id(const std::string &temp_dir, const compression_params &cp, const std::vector<std::vector<gabac::EncodingConfiguration>>& configs);

void generate_order(const std::string &file_order, uint32_t *order_array, const uint32_t &numreads);

void read_block_start_end(const std::string &file_blocks, std::vector<uint32_t> &block_start,
                          std::vector<uint32_t> &block_end);

void reorder_compress_id_pe(std::string *id_array, const std::string &temp_dir, const std::string &file_order_id,
                            const std::vector<uint32_t> &block_start, const std::vector<uint32_t> &block_end,
                            const compression_params &cp, const std::vector<std::vector<gabac::EncodingConfiguration>>& configs);

void reorder_compress_quality_pe(std::string file_quality[2], const std::string &temp_dir,
                                 std::string *quality_array, const uint64_t &quality_array_size, uint32_t *order_array,
                                 const std::vector<uint32_t> &block_start, const std::vector<uint32_t> &block_end,
                                 const compression_params &cp, const std::vector<std::vector<gabac::EncodingConfiguration>>& configs);

void reorder_compress(const std::string &file_name, const std::string &temp_dir, const uint32_t &num_reads_per_file, const int &num_thr,
                     const uint32_t &num_reads_per_block, std::string *str_array, const uint32_t &str_array_size,
                     uint32_t *order_array, const std::string &mode, const std::vector<std::vector<gabac::EncodingConfiguration>>& configs);
// mode can be "quality" or "id"

void generate_read_id_tokens(std::string *id_array, const uint32_t &num_ids, std::vector<int64_t> tokens[128][6]);

void generate_read_id_tokens(std::string *id_array, const uint32_t &num_ids, std::vector<gabac::DataBlock> &tokens);

}  // namespace spring

#endif  // SPRING_REORDER_COMPRESS_QUALITY_ID_H_
