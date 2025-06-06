/**
 * Copyright 2018-2024 The Genie Authors.
 * @file reorder_compress_quality_id.h
 * @copyright This file is part of Genie
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for reordering and compressing quality scores and read IDs
 * in paired-end reads.
 *
 * This file defines the functions and data structures used to handle reordering
 * and compression of quality scores and read identifiers for paired-end genomic
 * reads using different compression methods. The file includes functions to
 * manage read ordering, block segmentation, and integration with entropy
 * encoding modules.
 *
 * The file is part of the Spring module within the GENIE project.
 */

#ifndef SRC_GENIE_READ_SPRING_REORDER_COMPRESS_QUALITY_ID_H_
#define SRC_GENIE_READ_SPRING_REORDER_COMPRESS_QUALITY_ID_H_

// -----------------------------------------------------------------------------

#include <string>
#include <vector>

#include "genie/core/read_encoder.h"
#include "genie/read/spring/util.h"

// -----------------------------------------------------------------------------

namespace genie::read::spring {

/**
 * @brief Main function for reordering and compressing quality scores and read
 * IDs.
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
void ReorderCompressQualityId(const std::string& temp_dir,
                              const CompressionParams& cp,
                              core::ReadEncoder::qv_selector* qv_coder,
                              core::ReadEncoder::name_selector* name_coder,
                              core::ReadEncoder::entropy_selector* entropy,
                              std::vector<core::parameter::EncodingSet>& params,
                              core::stats::PerfStats& stats, bool write_raw);

/**
 * @brief Generates the order of reads from a file.
 *
 * @param file_order Path to the file containing the order information.
 * @param order_array Array to store the generated order.
 * @param num_reads Number of reads.
 */
void GenerateOrder(const std::string& file_order,
                   std::vector<uint32_t>& order_array,
                   const uint32_t& num_reads);

/**
 * @brief Reads the start and end positions of blocks from a file.
 *
 * @param file_blocks Path to the file containing block start and end positions.
 * @param block_start Vector to store the start positions of blocks.
 * @param block_end Vector to store the end positions of blocks.
 */
void ReadBlockStartEnd(const std::string& file_blocks,
                       std::vector<uint32_t>& block_start,
                       std::vector<uint32_t>& block_end);

/**
 * @brief Reorders and compresses read IDs in paired-end reads.
 *
 * @param id_array Array of read IDs.
 * @param temp_dir Directory for storing temporary files.
 * @param file_order_id File containing the order of read IDs.
 * @param block_start Vector of block start positions.
 * @param block_end Vector of block end positions.
 * @param cp Compression parameters.
 * @param name_coder Pointer to the name encoder.
 * @param entropy Pointer to the entropy encoder.
 * @param params Vector of encoding sets.
 * @param stats Reference to performance statistics.
 * @param write_raw Flag to indicate if raw data should be written.
 */
void ReorderCompressIdPe(const std::vector<std::string>& id_array,
                         const std::string& temp_dir,
                         const std::string& file_order_id,
                         const std::vector<uint32_t>& block_start,
                         const std::vector<uint32_t>& block_end,
                         const CompressionParams& cp,
                         core::ReadEncoder::name_selector* name_coder,
                         core::ReadEncoder::entropy_selector* entropy,
                         std::vector<core::parameter::EncodingSet>& params,
                         core::stats::PerfStats& stats, bool write_raw);

/**
 * @brief Reorders and compresses quality scores in paired-end reads.
 *
 * @param file_quality Array of file paths for quality scores of paired reads.
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
void ReorderCompressQualityPe(
    std::string file_quality[2], const std::string& temp_dir,
    std::vector<std::string>& quality_array, const uint64_t& quality_array_size,
    const std::vector<uint32_t>& order_array, const std::vector<uint32_t>&
    block_start,
    const std::vector<uint32_t>& block_end, const CompressionParams& cp,
    core::ReadEncoder::qv_selector* qv_coder,
    core::ReadEncoder::entropy_selector* entropy,
    std::vector<core::parameter::EncodingSet>& params,
    core::stats::PerfStats& stats, bool write_raw);

/**
 * @brief General reorder and compression function for quality scores or read
 * IDs.
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
void ReorderCompress(const std::string& file_name, const std::string& temp_dir,
                     const uint32_t& num_reads_per_file, const int& num_thr,
                     const uint32_t& num_reads_per_block,
                     std::vector<std::string>& str_array,
                     const uint32_t& str_array_size,
                     const std::vector<uint32_t>& order_array,
                     const std::string& mode,
                     core::ReadEncoder::qv_selector* qv_coder,
                     core::ReadEncoder::name_selector* name_coder,
                     core::ReadEncoder::entropy_selector* entropy,
                     std::vector<core::parameter::EncodingSet>& params,
                     core::stats::PerfStats& stats, bool write_raw, uint32_t
                     num_reads);

// -----------------------------------------------------------------------------

}  // namespace genie::read::spring

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_REORDER_COMPRESS_QUALITY_ID_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
