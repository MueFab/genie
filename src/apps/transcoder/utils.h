/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_APPS_TRANSCODER_UTILS_H_
#define SRC_APPS_TRANSCODER_UTILS_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/record/record.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace transcoder {

/**
 * @brief
 * @param r
 * @return
 */
uint64_t getMinPos(const genie::core::record::Record& r);

/**
 * @brief
 * @param r1
 * @param r2
 * @return
 */
bool compare(const genie::core::record::Record& r1, const genie::core::record::Record& r2);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace transcoder
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_APPS_TRANSCODER_UTILS_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
