/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie. See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_FLOW_GRAPH_ENCODE_H_
#define SRC_GENIE_CORE_FLOW_GRAPH_ENCODE_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <vector>

#include "genie/core/entropy_encoder.h"
#include "genie/core/flow_graph.h"
#include "genie/core/format_exporter_compressed.h"
#include "genie/core/format_importer.h"
#include "genie/core/read_encoder.h"
#include "genie/core/reference_source.h"
#include "genie/util/selector.h"
#include "genie/util/thread_manager.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class FlowGraphEncode final : public FlowGraph {
  util::ThreadManager mgr_;                                    //!< @brief
  std::unique_ptr<ReferenceManager> ref_mgr_;                  //!< @brief
  std::vector<std::unique_ptr<ReferenceSource>> ref_sources_;  //!< @brief
  std::unique_ptr<Classifier> classifier_;                     //!< @brief

  std::vector<std::unique_ptr<FormatImporter>> importers_;  //!< @brief

  std::vector<std::unique_ptr<ReadEncoder>> read_coders_;    //!< @brief
  util::Selector<record::Chunk, AccessUnit> read_selector_;  //!< @brief

  std::vector<std::unique_ptr<QvEncoder>> qv_coders_;  //!< @brief
  util::SideSelector<QvEncoder, QvEncoder::qv_coded,
                     const record::Chunk&>
      qv_selector_;  //!< @brief

  std::vector<std::unique_ptr<NameEncoder>> name_coders_;  //!< @brief
  ReadEncoder::name_selector name_selector_;               //!< @brief

  std::vector<std::unique_ptr<EntropyEncoder>> entropy_coders_;  //!< @brief
  util::SideSelector<EntropyEncoder, EntropyEncoder::entropy_coded,
                     AccessUnit::Descriptor&>
      entropy_selector_;  //!< @brief

  std::vector<std::unique_ptr<FormatExporterCompressed>>
      exporters_;                                     //!< @brief
  util::SelectorHead<AccessUnit> exporter_selector_;  //!< @brief

 public:
  /**
   * @brief
   * @param threads
   */
  explicit FlowGraphEncode(size_t threads);

  /**
   * @brief
   * @param dat
   */
  void AddImporter(std::unique_ptr<FormatImporter> dat);

  /**
   * @brief
   * @param dat
   */
  void AddReferenceSource(std::unique_ptr<ReferenceSource> dat);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] ReferenceManager& GetRefMgr() const;

  /**
   * @brief
   * @param classifier
   */
  void SetClassifier(std::unique_ptr<Classifier> classifier);

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
  void AddReadCoder(std::unique_ptr<ReadEncoder> dat);

  /**
   * @brief
   * @param dat
   * @param index
   */
  void SetReadCoder(std::unique_ptr<ReadEncoder> dat, size_t index);

  /**
   * @brief
   * @param dat
   */
  void AddEntropyCoder(std::unique_ptr<EntropyEncoder> dat);

  /**
   * @brief
   * @param dat
   * @param index
   */
  void SetEntropyCoder(std::unique_ptr<EntropyEncoder> dat, size_t index);

  /**
   * @brief
   * @param dat
   */
  void AddExporter(std::unique_ptr<FormatExporterCompressed> dat);

  /**
   * @brief
   * @param dat
   * @param index
   */
  void SetExporter(std::unique_ptr<FormatExporterCompressed> dat, size_t index);

  /**
   * @brief
   * @param dat
   */
  void AddNameCoder(std::unique_ptr<NameEncoder> dat);

  /**
   * @brief
   * @param dat
   * @param index
   */
  void SetNameCoder(std::unique_ptr<NameEncoder> dat, size_t index);

  /**
   * @brief
   * @param dat
   */
  void AddQvCoder(std::unique_ptr<QvEncoder> dat);

  /**
   * @brief
   * @param dat
   * @param index
   */
  void SetQvCoder(std::unique_ptr<QvEncoder> dat, size_t index);

  /**
   * @brief
   * @param fun
   */
  void SetReadCoderSelector(
      const std::function<size_t(const record::Chunk&)>& fun);

  /**
   * @brief
   * @param fun
   */
  void SetQvSelector(std::function<size_t(const record::Chunk&)> fun);

  /**
   * @brief
   * @param fun
   */
  void SetNameSelector(std::function<size_t(const record::Chunk&)> fun);

  /**
   * @brief
   * @param fun
   */
  void SetEntropyCoderSelector(
      const std::function<size_t(const AccessUnit::Descriptor&)>& fun);

  /**
   * @brief
   * @param fun
   */
  void SetExporterSelector(const std::function<size_t(const AccessUnit&)>& fun);

  /**
   * @brief
   */
  void Run() override;

  /**
   * @brief
   * @param abort
   */
  void Stop(bool abort) override;

  /**
   * @brief
   * @return
   */
  stats::PerfStats GetStats() override;

  /**
   * @brief
   * @return
   */
  [[nodiscard]] meta::Dataset GetMeta() const override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_FLOW_GRAPH_ENCODE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
