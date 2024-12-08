/**
 * Copyright 2018-2024 The Genie Authors.
 * @file
 * @copyright This file is part of Genie See LICENSE and/or
 * https://github.com/MueFab/genie for more details.
 */

#ifndef SRC_GENIE_CORE_FLOW_GRAPH_DECODE_H_
#define SRC_GENIE_CORE_FLOW_GRAPH_DECODE_H_

// -----------------------------------------------------------------------------

#include <memory>
#include <string>
#include <vector>

#include "genie/core/entropy_decoder.h"
#include "genie/core/flow_graph.h"
#include "genie/core/format_exporter.h"
#include "genie/core/format_importer_compressed.h"
#include "genie/core/read_decoder.h"
#include "genie/core/ref_decoder.h"
#include "genie/core/reference_source.h"
#include "genie/util/selector.h"
#include "genie/util/side_selector.h"
#include "genie/util/thread_manager.h"

// -----------------------------------------------------------------------------

namespace genie::core {

/**
 * @brief
 */
class FlowGraphDecode final : public FlowGraph {
  util::ThreadManager mgr_;  //!< @brief
  std::vector<std::unique_ptr<FormatImporterCompressed>>
      importers_;  //!< @brief

  std::unique_ptr<ReferenceManager> ref_mgr_;                  //!< @brief
  std::vector<std::unique_ptr<ReferenceSource>> ref_sources_;  //!< @brief
  RefDecoder* ref_decoder_{nullptr};                           //!< @brief

  std::vector<std::unique_ptr<ReadDecoder>> read_coders_;    //!< @brief
  util::Selector<AccessUnit, record::Chunk> read_selector_;  //!< @brief

  std::vector<std::unique_ptr<QvDecoder>> qv_coders_;  //!< @brief
  ReadDecoder::qv_selector qv_selector_;               //!< @brief

  std::vector<std::unique_ptr<NameDecoder>> name_coders_;  //!< @brief
  ReadDecoder::name_selector name_selector_;               //!< @brief

  std::vector<std::unique_ptr<EntropyDecoder>> entropy_coders_;  //!< @brief
  ReadDecoder::entropy_selector entropy_selector_;               //!< @brief

  std::vector<std::unique_ptr<FormatExporter>> exporters_;  //!< @brief
  util::SelectorHead<record::Chunk> exporter_selector_;     //!< @brief

 public:
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
   * @param ref_decoder
   */
  void SetRefDecoder(RefDecoder* ref_decoder);

  /**
   * @brief
   * @return
   */
  [[nodiscard]] RefDecoder* GetRefDecoder() const;

  /**
   * @brief
   * @param threads
   */
  explicit FlowGraphDecode(size_t threads);

  /**
   * @brief
   * @param dat
   */
  void AddImporter(std::unique_ptr<FormatImporterCompressed> dat);

  /**
   * @brief
   * @param dat
   * @param index
   */
  void SetImporter(std::unique_ptr<FormatImporterCompressed> dat, size_t index);

  /**
   * @brief
   * @param dat
   */
  void AddReadCoder(std::unique_ptr<ReadDecoder> dat);

  /**
   * @brief
   * @param dat
   * @param index
   */
  void SetReadCoder(std::unique_ptr<ReadDecoder> dat, size_t index);

  /**
   * @brief
   * @param dat
   */
  void AddEntropyCoder(std::unique_ptr<EntropyDecoder> dat);

  /**
   * @brief
   * @param dat
   * @param index
   */
  void SetEntropyCoder(std::unique_ptr<EntropyDecoder> dat, size_t index);

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
  void SetReadCoderSelector(
      const std::function<size_t(const AccessUnit&)>& fun);

  /**
   * @brief
   * @param fun
   */
  void SetQvSelector(
      std::function<size_t(const parameter::QualityValues& param,
                           const std::vector<std::string>& ecigar,
                           const std::vector<uint64_t>& positions,
                           AccessUnit::Descriptor& desc)>
          fun);

  /**
   * @brief
   * @param fun
   */
  void SetNameSelector(std::function<size_t(AccessUnit::Descriptor& desc)> fun);

  /**
   * @brief
   * @param fun
   */
  void SetEntropyCoderSelector(
      const std::function<size_t(const parameter::DescriptorSubSequenceCfg&,
                                 AccessUnit::Descriptor&, bool)>& fun);

  /**
   * @brief
   * @param fun
   */
  void SetExporterSelector(
      const std::function<size_t(const record::Chunk&)>& fun);

  /**
   * @brief
   * @param dat
   */
  void AddNameCoder(std::unique_ptr<NameDecoder> dat);

  /**
   * @brief
   * @param dat
   * @param index
   */
  void SetNameCoder(std::unique_ptr<NameDecoder> dat, size_t index);

  /**
   * @brief
   * @param dat
   */
  void AddQvCoder(std::unique_ptr<QvDecoder> dat);

  /**
   * @brief
   * @param dat
   * @param index
   */
  void SetQvCoder(std::unique_ptr<QvDecoder> dat, size_t index);

  /**
   * @brief
   */
  void Run() override;

  /**
   * @brief
   * @return
   */
  stats::PerfStats GetStats() override;
};

// -----------------------------------------------------------------------------

}  // namespace genie::core

// -----------------------------------------------------------------------------

#endif  // SRC_GENIE_CORE_FLOW_GRAPH_DECODE_H_

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
