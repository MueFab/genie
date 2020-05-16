/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_DEFAULTSETUP_H
#define GENIE_DEFAULTSETUP_H

// ---------------------------------------------------------------------------------------------------------------------

#include <genie/core/flowgraph-decode.h>
#include <genie/core/flowgraph-encode.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace module {

/**
 *
 * @param threads
 * @param working_dir
 * @return
 */
std::unique_ptr<core::FlowGraphEncode> buildDefaultEncoder(size_t threads, const std::string& working_dir, size_t);

/**
 *
 * @param threads
 * @return
 */
std::unique_ptr<core::FlowGraphDecode> buildDefaultDecoder(size_t threads, const std::string&, size_t);

// ---------------------------------------------------------------------------------------------------------------------

}
}

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_DEFAULTSETUP_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------