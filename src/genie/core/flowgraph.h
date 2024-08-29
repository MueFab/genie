/**
 * @file
 * @copyright This file is part of GENIE. See LICENSE and/or
 * https://github.com/mitogen/genie for more details.
 */

#ifndef SRC_GENIE_CORE_FLOWGRAPH_H_
#define SRC_GENIE_CORE_FLOWGRAPH_H_

// ---------------------------------------------------------------------------------------------------------------------

#include "genie/core/meta/blockheader/enabled.h"
#include "genie/core/meta/dataset.h"
#include "genie/core/stats/perf-stats.h"
#include "genie/util/make-unique.h"

// ---------------------------------------------------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class FlowGraph {
 public:
    /**
     * @brief
     */
    virtual void run() = 0;

    /**
     * @brief
     */
    virtual void stop(bool) {}

    /**
     * @brief
     * @return
     */
    [[nodiscard]] virtual core::meta::Dataset getMeta() const;

    /**
     * @brief
     * @return
     */
    virtual core::stats::PerfStats getStats() = 0;

    /**
     * @brief
     */
    virtual ~FlowGraph() = default;
};

// ---------------------------------------------------------------------------------------------------------------------

}  // namespace genie::core

// ---------------------------------------------------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_FLOWGRAPH_H_

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
