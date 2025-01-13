/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#include "apps/genie/gabac/code.h"

#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>

#include "genie/core/constants.h"
#include "genie/entropy/gabac/gabac.h"

// -----------------------------------------------------------------------------

namespace genie_app::gabac {

// -----------------------------------------------------------------------------

void code(const std::string& input_file_path,
          const std::string& output_file_path, const std::string& config_path,
          size_t blocksize, uint8_t desc_id, uint8_t subseq_id, bool decode,
          const std::string& dependency_file_path) {
  std::ifstream input_file;
  std::ifstream dependency_file;
  std::ofstream output_file;
  genie::entropy::gabac::NullStream nullstream;

  std::istream* istream = &std::cin;
  std::istream* dstream = nullptr;
  std::ostream* ostream = &std::cerr;
  std::ostream* logstream = &std::cerr;

  if (!input_file_path.empty()) {
    // Read in the entire input file
    input_file = std::ifstream(input_file_path, std::ios::binary);
    UTILS_DIE_IF(!input_file, "Cannot open file to read: " + input_file_path);
    istream = &input_file;
  }

  if (!dependency_file_path.empty()) {
    // Read in the entire dependency file
    dependency_file = std::ifstream(dependency_file_path, std::ios::binary);
    UTILS_DIE_IF(!dependency_file,
                 "Cannot open file to read: " + dependency_file_path);
    dstream = &dependency_file;
  }

  if (!output_file_path.empty()) {
    // Write the bytestream
    output_file = std::ofstream(output_file_path, std::ios::binary);
    if (!output_file) {
      UTILS_DIE("Could not open output file");
    }
    ostream = &output_file;
  } else {
    logstream = &nullstream;
  }

  genie::entropy::gabac::IoConfiguration ioconf = {
      istream,   1,
      dstream,   ostream,
      1,         blocksize,
      logstream, genie::entropy::gabac::IoConfiguration::LogLevel::LOG_INFO};

  genie::core::GenSubIndex genie_subseq_id =
      std::pair(static_cast<genie::core::GenDesc>(desc_id), subseq_id);

  genie::entropy::gabac::EncodingConfiguration config;

  if (config_path.empty()) {
    config = genie::entropy::gabac::EncodingConfiguration(
        genie::entropy::gabac::GetEncoderConfigManual(genie_subseq_id));
    //  std::ofstream tmp("./cfg.json");
    //  tmp << config.ToJson().dump(4);
  } else {
    std::ifstream t(config_path);
    UTILS_DIE_IF(!t, "Cannot open file to read: " + config_path);
    std::string str((std::istreambuf_iterator(t)),
                    std::istreambuf_iterator<char>());
    config = genie::entropy::gabac::EncodingConfiguration(
        nlohmann::json::parse(str));
  }
  Run(ioconf, config, decode);

  /* GABACIFY_LOG_INFO << "Wrote buffer of Size "
                    << outStream.bytesWritten()
                    << " to: "
                    << outputFilePath;*/
}

// -----------------------------------------------------------------------------

}  // namespace genie_app::gabac

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
