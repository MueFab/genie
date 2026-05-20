/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_FLOW_GRAPH_CONVERT_H_
#define SRC_GENIE_CORE_FLOW_GRAPH_CONVERT_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <vector>

#include "genie/core/flow_graph.h"
#include "genie/core/format_exporter.h"
#include "genie/core/format_importer.h"
#include "genie/util/selector.h"
#include "genie/util/thread_manager.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class FlowGraphConvert final : public FlowGraph {
  util::ThreadManager mgr_;                                 //!< @brief
  std::vector<std::unique_ptr<FormatImporter>> importers_;  //!< @brief
  std::unique_ptr<Classifier> classifier_;                  //!< @brief
  std::unique_ptr<ReferenceManager> ref_mgr_;               //!< @brief

  std::vector<std::unique_ptr<FormatExporter>> exporters_;  //!< @brief
  util::SelectorHead<record::Chunk> exporter_selector_;     //!< @brief

 public:
  /**
   * @brief
   * @param threads
   */
  explicit FlowGraphConvert(size_t threads);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] ReferenceManager& GetRefMgr() const;

  /**
   * @brief
   * @param dat
   */
  void AddImporter(std::unique_ptr<FormatImporter> dat);

  /**
   * @brief
   * @param dat
   * @param index
   */
  void SetImporter(std::unique_ptr<FormatImporter> dat, size_t index);

  /**
   * @brief
   * @param dat
   */
  void AddExporter(std::unique_ptr<FormatExporter> dat);

  /**
   * @brief
   * @param dat
   * @param index
   */
  void SetExporter(std::unique_ptr<FormatExporter> dat, size_t index);

  /**
   * @brief
   * @param fun
   */
  void SetExporterSelector(
      const std::function<size_t(const record::Chunk&)>& fun);

    /**
     * @brief
     * @param _classifier
     */
    void SetClassifier(std::unique_ptr<genie::core::Classifier> _classifier);

  /**
   * @brief
   */
  void Run() override;

  /**
   * @brief
   * @return
   */
  stats::PerfStats GetStats() override;

  /**
   * @brief
   * @param abort
   */
  void Stop(bool abort) override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_FLOW_GRAPH_CONVERT_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
