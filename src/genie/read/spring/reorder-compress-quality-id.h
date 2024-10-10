/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_READ_SPRING_REORDER_COMPRESS_QUALITY_ID_H_
#define SRC_GENIE_READ_SPRING_REORDER_COMPRESS_QUALITY_ID_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include "genie/core/entropy-encoder.h"
#include "genie/core/name-encoder.h"
#include "genie/core/qv-encoder.h"
#include "genie/core/read-encoder.h"
#include "genie/read/spring/util.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::spring {

/**
 * @brief
 * @param temp_dir
 * @param cp
 * @param qv_coder
 * @param name_coder
 * @param entropy
 * @param params
 * @param stats
 * @param write_raw
 */
void reorder_compress_quality_id(const std::string &temp_dir, const compression_params &cp,
                                 genie::core::ReadEncoder::QvSelector *qv_coder,
                                 genie::core::ReadEncoder::NameSelector *name_coder,
                                 genie::core::ReadEncoder::EntropySelector *entropy,
                                 std::vector<core::parameter::EncodingSet> &params, core::stats::PerfStats &stats,
                                 bool write_raw);

/**
 * @brief
 * @param file_order
 * @param order_array
 * @param numreads
 */
void generate_order(const std::string &file_order, uint32_t *order_array, const uint32_t &numreads);

/**
 * @brief
 * @param file_blocks
 * @param block_start
 * @param block_end
 */
void read_block_start_end(const std::string &file_blocks, std::vector<uint32_t> &block_start,
                          std::vector<uint32_t> &block_end);

/**
 * @brief
 * @param id_array
 * @param temp_dir
 * @param file_order_id
 * @param block_start
 * @param block_end
 * @param file_name
 * @param cp
 * @param name_coder
 * @param entropy
 * @param params
 * @param stats
 * @param write_raw
 */
void reorder_compress_id_pe(std::string *id_array, const std::string &temp_dir, const std::string &file_order_id,
                            const std::vector<uint32_t> &block_start, const std::vector<uint32_t> &block_end,
                            const std::string &file_name, const compression_params &cp,
                            genie::core::ReadEncoder::NameSelector *name_coder,
                            genie::core::ReadEncoder::EntropySelector *entropy,
                            std::vector<core::parameter::EncodingSet> &params, core::stats::PerfStats &stats,
                            bool write_raw);

/**
 * @brief
 * @param file_quality
 * @param outfile_quality
 * @param temp_dir
 * @param quality_array
 * @param quality_array_size
 * @param order_array
 * @param block_start
 * @param block_end
 * @param cp
 * @param qv_coder
 * @param entropy
 * @param params
 * @param stats
 * @param write_raw
 */
void reorder_compress_quality_pe(std::string file_quality[2], const std::string &outfile_quality,
                                 const std::string &temp_dir, std::string *quality_array,
                                 const uint64_t &quality_array_size, const uint32_t *order_array,
                                 const std::vector<uint32_t> &block_start, const std::vector<uint32_t> &block_end,
                                 const compression_params &cp, genie::core::ReadEncoder::QvSelector *qv_coder,
                                 genie::core::ReadEncoder::EntropySelector *entropy,
                                 std::vector<core::parameter::EncodingSet> &params, core::stats::PerfStats &stats,
                                 bool write_raw);

/**
 * @brief
 * @param file_name
 * @param temp_dir
 * @param num_reads_per_file
 * @param num_thr
 * @param num_reads_per_block
 * @param str_array
 * @param str_array_size
 * @param order_array
 * @param mode can be "quality" or "id"
 * @param qv_coder
 * @param name_coder
 * @param entropy
 * @param params
 * @param stats
 * @param write_raw
 */
void reorder_compress(const std::string &file_name, const std::string &temp_dir, const uint32_t &num_reads_per_file,
                      const int &num_thr, const uint32_t &num_reads_per_block, std::string *str_array,
                      const uint32_t &str_array_size, const uint32_t *order_array, const std::string &mode,
                      genie::core::ReadEncoder::QvSelector *qv_coder,
                      genie::core::ReadEncoder::NameSelector *name_coder,
                      genie::core::ReadEncoder::EntropySelector *entropy,
                      std::vector<core::parameter::EncodingSet> &params, core::stats::PerfStats &stats, bool write_raw);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::spring

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_REORDER_COMPRESS_QUALITY_ID_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
