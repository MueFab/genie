#ifndef SPRING_REORDER_COMPRESS_QUALITY_ID_ERU_H_
#define SPRING_REORDER_COMPRESS_QUALITY_ID_ERU_H_

#include <string>
#include "descriptors/spring/util.h"

namespace spring {

void reorder_compress_quality_id(const std::string &temp_dir,
                                 const compression_params &cp);

void reorder_compress(const std::string &file_name, const std::string &outfilenameprefix,
                      const uint32_t &num_reads_per_file, const int &num_thr,
                      const uint32_t &num_reads_per_block,
                      std::string *str_array, const uint32_t &str_array_size,
                      uint32_t *order_array, const std::string &mode);
// mode can be "quality" or "id"

void reorder_compress_quality_pe(const std::string file_name[2], const std::string &outfilenameprefix,
                      const uint32_t &num_reads, const int &num_thr,
                      const uint32_t &num_reads_per_block,
                      std::string *str_array_1, std::string *str_array_2, const uint32_t &str_array_size,
                      uint32_t *order_array);

void generate_read_id_tokens(std::string *id_array, const uint32_t &num_ids, std::vector<int64_t> tokens[128][8]);

}  // namespace spring

#endif  // SPRING_REORDER_COMPRESS_QUALITY_ID_ERU_H_
