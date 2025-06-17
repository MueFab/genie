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
#include "genie/core/classifier_regroup.h"
#include "genie/core/flow_graph_convert.h"
#include "genie/core/flow_graph_decode.h"
#include "genie/core/flow_graph_encode.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace module {

/**
 * @brief
 * @param threads
 * @param working_dir
 * @param externalref
 * @param rawref
 * @param writeRawStreams
 * @return
 */
std::unique_ptr<core::FlowGraphEncode> BuildDefaultEncoder(size_t threads, const std::string& working_dir, size_t,
                                                           core::ClassifierRegroup::RefMode externalref, bool rawref,
                                                           bool writeRawStreams);

/**
 * @brief
 * @param threads
 * @param combinePairsFlag
 * @return
 */
std::unique_ptr<core::FlowGraphDecode> build_default_decoder(size_t threads, const std::string&, bool combinePairsFlag,
                                                           size_t);

/**
 * @brief
 * @param threads
 * @return
 */
std::unique_ptr<core::FlowGraphConvert> build_default_converter(size_t threads);

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace module
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_MODULE_DEFAULT_SETUP_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
