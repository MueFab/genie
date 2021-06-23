/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_UTIL_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_UTIL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <string>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace format {
namespace mpegg_p1 {

/**
 * @brief
 * @param reader
 * @return
 */
std::string readKey(util::BitReader &reader);

/**
 * @brief
 * @param reader
 * @return
 */
std::string readNullTerminatedStr(util::BitReader &reader);

/**
 * @brief
 * @param writer
 * @param string
 */
void writeNullTerminatedStr(util::BitWriter &writer, const std::string &string);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_UTIL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
