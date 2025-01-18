/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_FLOW_GRAPH_H_
#define SRC_GENIE_CORE_FLOW_GRAPH_H_

// -----------------------------------------------------------------------------

#include "genie/core/meta/dataset.h"
#include "genie/core/stats/perf_stats.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class FlowGraph {
 public:
  /**
   * @brief
   */
  virtual void Run() = 0;

  /**
   * @brief
   */
  virtual void Stop(bool) {}

  /**
   * @brief
   * @return
   */
  [[nodiscard]] virtual meta::Dataset GetMeta() const;

  /**
   * @brief
   * @return
   */
  virtual stats::PerfStats GetStats() = 0;

  /**
   * @brief
   */
  virtual ~FlowGraph() = default;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_FLOW_GRAPH_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
