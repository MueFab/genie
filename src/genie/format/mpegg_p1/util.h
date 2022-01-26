/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_FORMAT_MPEGG_P1_UTIL_H_
#define SRC_GENIE_FORMAT_MPEGG_P1_UTIL_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <list>
#include <memory>
#include <string>
#include <vector>
#include "genie/util/bitreader.h"
#include "genie/util/bitwriter.h"
#include "genie/util/exception.h"
#include "genie/util/make-unique.h"
#include "genie/util/runtime-exception.h"

#define ROUNDTRIP_CONSTRUCTOR false

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
 * @param n
 * @return
 */
std::string readFixedLengthChars(util::BitReader &reader, uint8_t n);

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

/**
 * @brief
 * @param reader
 * @param length
 */
void skipRead(util::BitReader &reader, uint64_t length);

/**
 * @brief
 * @param box
 * @param reader
 * @param start_pos
 * @param length
 */
void readRawBox(std::list<uint8_t> &box, util::BitReader &reader, size_t start_pos, size_t length);

/**
 * @brief
 * @param box
 * @param writer
 */
void writeRawBox(const std::list<uint8_t> &box, util::BitWriter &writer);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace mpegg_p1
}  // namespace format
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_FORMAT_MPEGG_P1_UTIL_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
