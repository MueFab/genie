/**
 * @file reorder-compress-quality-id.h
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for reordering and compressing quality scores and read IDs in paired-end reads.
 *
 * This file defines the functions and data structures used to handle reordering and compression of
 * quality scores and read identifiers for paired-end genomic reads using different compression methods.
 * The file includes functions to manage read ordering, block segmentation, and integration with
 * entropy encoding modules.
 *
 * The file is part of the Spring module within the GENIE project.
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
 * @brief Main function for reordering and compressing quality scores and read IDs.
 *
 * @param temp_dir Directory for storing temporary files during the operation.
 * @param cp Compression parameters.
 * @param qv_coder Pointer to the quality value (QV) encoder.
 * @param name_coder Pointer to the name encoder.
 * @param entropy Pointer to the entropy encoder.
 * @param params Vector of encoding sets for storing parameters.
 * @param stats Reference to performance statistics.
 * @param write_raw Flag to indicate if raw data should be written.
 */
void reorder_compress_quality_id(const std::string &temp_dir, const compression_params &cp,
                                 genie::core::ReadEncoder::QvSelector *qv_coder,
                                 genie::core::ReadEncoder::NameSelector *name_coder,
                                 genie::core::ReadEncoder::EntropySelector *entropy,
                                 std::vector<core::parameter::EncodingSet> &params, core::stats::PerfStats &stats,
                                 bool write_raw);

/**
 * @brief Generates the order of reads from a file.
 *
 * @param file_order Path to the file containing the order information.
 * @param order_array Array to store the generated order.
 * @param numreads Number of reads.
 */
void generate_order(const std::string &file_order, uint32_t *order_array, const uint32_t &numreads);

/**
 * @brief Reads the start and end positions of blocks from a file.
 *
 * @param file_blocks Path to the file containing block start and end positions.
 * @param block_start Vector to store the start positions of blocks.
 * @param block_end Vector to store the end positions of blocks.
 */
void read_block_start_end(const std::string &file_blocks, std::vector<uint32_t> &block_start,
                          std::vector<uint32_t> &block_end);

/**
 * @brief Reorders and compresses read IDs in paired-end reads.
 *
 * @param id_array Array of read IDs.
 * @param temp_dir Directory for storing temporary files.
 * @param file_order_id File containing the order of read IDs.
 * @param block_start Vector of block start positions.
 * @param block_end Vector of block end positions.
 * @param file_name Output file name.
 * @param cp Compression parameters.
 * @param name_coder Pointer to the name encoder.
 * @param entropy Pointer to the entropy encoder.
 * @param params Vector of encoding sets.
 * @param stats Reference to performance statistics.
 * @param write_raw Flag to indicate if raw data should be written.
 */
void reorder_compress_id_pe(std::string *id_array, const std::string &temp_dir, const std::string &file_order_id,
                            const std::vector<uint32_t> &block_start, const std::vector<uint32_t> &block_end,
                            const std::string &file_name, const compression_params &cp,
                            genie::core::ReadEncoder::NameSelector *name_coder,
                            genie::core::ReadEncoder::EntropySelector *entropy,
                            std::vector<core::parameter::EncodingSet> &params, core::stats::PerfStats &stats,
                            bool write_raw);

/**
 * @brief Reorders and compresses quality scores in paired-end reads.
 *
 * @param file_quality Array of file paths for quality scores of paired reads.
 * @param outfile_quality Output file name for compressed quality scores.
 * @param temp_dir Directory for storing temporary files.
 * @param quality_array Array of quality scores.
 * @param quality_array_size Size of the quality array.
 * @param order_array Array containing the order of reads.
 * @param block_start Vector of block start positions.
 * @param block_end Vector of block end positions.
 * @param cp Compression parameters.
 * @param qv_coder Pointer to the quality value encoder.
 * @param entropy Pointer to the entropy encoder.
 * @param params Vector of encoding sets.
 * @param stats Reference to performance statistics.
 * @param write_raw Flag to indicate if raw data should be written.
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
 * @brief General reorder and compression function for quality scores or read IDs.
 *
 * @param file_name File name of the input data.
 * @param temp_dir Directory for storing temporary files.
 * @param num_reads_per_file Number of reads per file.
 * @param num_thr Number of threads.
 * @param num_reads_per_block Number of reads per block.
 * @param str_array Array of strings representing quality scores or read IDs.
 * @param str_array_size Size of the string array.
 * @param order_array Array containing the order of reads.
 * @param mode Compression mode, can be either "quality" or "id".
 * @param qv_coder Pointer to the quality value encoder.
 * @param name_coder Pointer to the name encoder.
 * @param entropy Pointer to the entropy encoder.
 * @param params Vector of encoding sets.
 * @param stats Reference to performance statistics.
 * @param write_raw Flag to indicate if raw data should be written.
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
