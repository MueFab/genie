/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_GENIE_GABAC_CODE_H_
#define SRC_APPS_GENIE_GABAC_CODE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>
#include <cstdint>
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace genieapp {
namespace gabac {

/**
 * @brief
 * @param inputFilePath
 * @param outputFilePath
 * @param config_path
 * @param blocksize
 * @param descID
 * @param subseqID
 * @param decode
 * @param dependencyFilePath
 */
void code(const std::string& inputFilePath, const std::string& outputFilePath, const std::string& config_path,
          size_t blocksize, uint8_t descID, uint8_t subseqID, bool decode, const std::string& dependencyFilePath = "");

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabac
}  // namespace genieapp

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_GENIE_GABAC_CODE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
