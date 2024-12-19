/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_APPS_GENIE_GABAC_CODE_H_
#define SRC_APPS_GENIE_GABAC_CODE_H_

// -----------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <string>

// -----------------------------------------------------------------------------

namespace genie_app::gabac {

/**
 * @brief
 * @param input_file_path
 * @param output_file_path
 * @param config_path
 * @param blocksize
 * @param desc_id
 * @param subseq_id
 * @param decode
 * @param dependency_file_path
 */
void code(const std::string& input_file_path,
          const std::string& output_file_path, const std::string& config_path,
          size_t blocksize, uint8_t desc_id, uint8_t subseq_id, bool decode,
          const std::string& dependency_file_path = "");

// -----------------------------------------------------------------------------

}  // namespace genie_app::gabac

// -----------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_GABAC_CODE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
