/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef GENIE_FLOWGRAPH_H
#define GENIE_FLOWGRAPH_H

#include <genie/core/stats/perf-stats.h>

// ---------------------------------------------------------------------------------------------------------------------

namespace genie {
namespace core {

/**
 *
 */
class FlowGraph {
   public:
    /**
     *
     */
    virtual void run() = 0;

    /**
     *
     */
    virtual void stop(bool) {}

    /**
     *
     * @return
     */
    virtual core::stats::PerfStats getStats() = 0;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace core
}  // namespace genie

// ---------------------------------------------------------------------------------------------------------------------

#endif  // GENIE_FLOWGRAPH_H

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------