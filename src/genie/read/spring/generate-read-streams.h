/**
 * @file generate-read-streams.h
 * @copyright This file is part of GENIE.
 * See LICENSE and/or visit https://github.com/MueFab/genie for more details.
 * @brief Header file for generating read streams in the Spring module of GENIE.
 *
 * This file declares the `generate_read_streams` function used for creating encoded read streams
 * from temporary directory files. It utilizes the `compression_params` structure and various encoders
 * provided by the GENIE core. The resulting read streams are written in a format that can be used
 * for efficient encoding and compression within the Spring module.
 *
 * This file is part of the Spring module within the GENIE project.
 */

#ifndef SRC_GENIE_READ_SPRING_GENERATE_READ_STREAMS_H_
#define SRC_GENIE_READ_SPRING_GENERATE_READ_STREAMS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <map>
#include <string>
#include <vector>
#include "genie/core/read-encoder.h"
#include "genie/read/spring/util.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::read::spring {

/**
 * @brief Generates encoded read streams from temporary directory files.
 *
 * This function reads the temporary files created during the preprocessing stage and generates
 * the encoded read streams. It uses the provided entropy encoder and compression parameters
 * to optimize the storage of read sequences. The results are stored in the specified temporary
 * directory and can be used for further encoding steps.
 *
 * @param temp_dir The directory where temporary files are stored.
 * @param cp Compression parameters for the Spring module.
 * @param entropycoder Entropy encoder for compressing the read streams.
 * @param params Encoding parameters to be used for the streams.
 * @param stats Performance statistics for tracking encoding efficiency.
 * @param write_raw Flag indicating whether to write raw streams.
 */
void generate_read_streams(const std::string &temp_dir, const compression_params &cp,
                           core::ReadEncoder::EntropySelector *entropycoder,
                           std::vector<core::parameter::EncodingSet> &params, core::stats::PerfStats &stats,
                           bool write_raw);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::read::spring

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_READ_SPRING_GENERATE_READ_STREAMS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
