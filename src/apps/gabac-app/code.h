/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_GABAC_APP_CODE_H_
#define SRC_APPS_GABAC_APP_CODE_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <cstddef>
#include <string>

// ---------------------------------------------------------------------------------------------------------------------

namespace gabacify {

/**
 * @brief
 * @param inputFilePath
 * @param outputFilePath
 * @param blocksize
 * @param descID
 * @param subseqID
 * @param decode
 * @param dependencyFilePath
 */
void code(const std::string& inputFilePath, const std::string& outputFilePath, size_t blocksize, uint8_t descID,
          uint8_t subseqID, bool decode, const std::string& dependencyFilePath = "");

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace gabacify

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_GABAC_APP_CODE_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
