/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_MODULE_DEFAULT_SETUP_H_
#define SRC_GENIE_MODULE_DEFAULT_SETUP_H_

// ---------------------------------------------------------------------------------------------------------------------

#include <memory>
#include <string>
#include "genie/core/classifier-regroup.h"
#include "genie/core/flowgraph-convert.h"
#include "genie/core/flowgraph-decode.h"
#include "genie/core/flowgraph-encode.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace module {

/**
 *
 * @param threads
 * @param working_dir
 * @param externalref
 * @param rawref
 * @return
 */
std::unique_ptr<core::FlowGraphEncode> buildDefaultEncoder(size_t threads, const std::string& working_dir, size_t,
                                                           core::ClassifierRegroup::RefMode externalref, bool rawref);

/**
 *
 * @param threads
 * @return
 */
std::unique_ptr<core::FlowGraphDecode> buildDefaultDecoder(size_t threads, const std::string&, bool combinePairsFlag,
                                                           size_t);

/**
 *
 * @param threads
 * @return
 */
std::unique_ptr<core::FlowGraphConvert> buildDefaultConverter(size_t threads);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace module
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_MODULE_DEFAULT_SETUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
